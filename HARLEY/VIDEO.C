#include <stdlib.h>
//#include <stdio.h>
#include <conio.h>
#include <lib2\fasm.h>
#include <lib2\asm.h>
#include <lib2\mem.h>
#include "video.h"

typedef struct _Copystuf Copystuf;
typedef struct _Lfb Lfb;
int use_lfb=1;
int force_ramp=0;


typedef struct { unsigned short off, seg; } Ptr16;
typedef struct {
   //char sign[4];
   int sign;
   short ver;
   Ptr16 oemstr;
   int cap;
   Ptr16 modelist;
   short totmem;
   short oemrev;
   Ptr16 oemvendor;
   Ptr16 oemproduct;
   Ptr16 oemprev;
   char Reserved[222];
   char OemData[256];
} Vbeinfo;
typedef struct {
     // Mandatory information for all VBE revisions
  unsigned short modeattr; //dw ?      ; mode attributes
                           //  bit 0: 1 mode supported
                           //  bit 1: always 1
                           //  bit 2: 1 bios supports tty
                           //  bit 3: 1 color/0 mono
                           //  bit 4: 1 gfx/0 text
                           //  bit 5: 1 non-vga-compatible/0 compatible
                           //  bit 6: 1 no windowing/0 windowing
                           //  bit 7: 1 yes lfb/0 no lfb
  char wa_attr;            //db ?      ; window A attributes
  char wb_attr;            //db ?      ; window B attributes
                           //  bit 0: 1 relocatable windows supported/0 if not
                           //  bit 1: 1 readable/0 non-readable
                           //  bit 2: 1 writeable/0 non-writeable
  unsigned short wgran;    //dw ?      ; window granularity (in kb)
  unsigned short wsize;    //dw ?      ; window size (in kb)
  unsigned short wa_seg;   //dw ?      ; window A start segment
  unsigned short wb_seg;   //dw ?      ; window B start segment
  long winfuncptr;         //dd ?      ; pointer to window function
  unsigned short bytes;    //dw ?      ; bytes per scan line

     // Mandatory information for VBE 1.2 and above
  unsigned short xres;     //dw ?      ; horizontal resolution in pixels or chars
  unsigned short yres;     //dw ?      ; vertical resolution in pixels or chars
  char xcharsize;          //db ?      ; character cell width in pixels
  char ycharsize;          //db ?      ; character cell height in pixels
  char planes;             //db ?      ; number of memory planes
  char bits;               //db ?      ; bits per pixel
  char banks;              //db ?      ; number of banks
  char memmodel;           //db ?      ; memory model type
                           //  0: textmode
                           //  1: cga
                           //  2: hercules
                           //  3: planar
                           //  4: packed pixel
                           //  5: non-chain4
                           //  6: directcolor
                           //  7: yuv
                           //  8-f: reserved by vesa
                           //  10-ff: reserved by oem
  char banksize;           //db ?      ; bank size in KB
  char imagepages;         //db ?      ; number of images
  char reserved;           //db ?      ; reserved for page function

     // Direct Color fields (required for direct/6 and YUV/7 memory models)
  char redsize, redpos, greensize, greenpos,
    bluesize, bluepos, rsvdsize, rsvdpos;
  char dcattr;
     // Mandatory information for VBE 2.0 and above
  unsigned long lfbaddr;           //dd ?      ; physical address for flat frame buffer
  unsigned long offscreenmemoffset;//dd ?      ; pointer to start of off screen memory
  unsigned short offscreenmemsize; //dw ?      ; amount of off screen memory in 1k units
  char Reserved[206];        //db 206 dup (?)  ; blahblahblah to 256 bytes
} Vbemode;



struct _Video {
  Lfb buf, view, page, vidmem;
  int mnum;
  int kerroin;
  Vbeinfo *vbeinfo;
  Vbemode *vbemode;
  void *dosbuf;
};
struct _Copystuf {
  Lfb src, dst;
  char *p;
  int x, y, bytes;
};



typedef struct {
  union { unsigned int edi; unsigned short di; };
  union { unsigned int esi; unsigned short si; };
  union { unsigned int ebp; unsigned short bp; };
  unsigned int ___1;
  union { unsigned int ebx; unsigned short bx; struct { char bl, bh; }; };
  union { unsigned int edx; unsigned short dx; struct { char dl, dh; }; };
  union { unsigned int ecx; unsigned short cx; struct { char cl, ch; }; };
  union { unsigned int eax; unsigned short ax; struct { char al, ah; }; };
  unsigned short flags, es, ds, fs, gs, ip, cs, sp, ss;
} Regs;




#define ptr16to32(x) ((void*)((x).seg*16+(x).off))

void vgasetmode(int);
#pragma aux vgasetmode="int 0x10" parm [eax] modify [eax ebx ecx edx esi edi];
int vesasetmode(int);
#pragma aux vesasetmode="mov eax, 0x4f02" "int 0x10" "and eax, 0xff" parm [ebx] value [eax] modify [eax ebx ecx edx esi edi];
static int cur_bank=-1;
void vesasetbank(int);
#pragma aux vesasetbank="cmp edx, [cur_bank]" "jz over" "mov [cur_bank], edx" "mov eax, 0x4f05" "xor ebx, ebx" "int 0x10" "over:" parm [edx] modify [eax ebx ecx edx esi edi];
unsigned short allocdosmem(unsigned short paragraphs);
#pragma aux allocdosmem="mov ax, 0x100" "int 0x31" "cmc" "sbb ebx, ebx" "and eax, eax" parm [bx] value [ax] modify [eax ebx ecx edx esi edi];
void *mapphys(long physptr,long howmuch);
#pragma aux mapphys=     \
  "mov ax,0x800"         \
  "mov ecx,ebx"          \
  "mov edi,esi"          \
  "shr ebx,16"           \
  "shr esi,16"           \
  "int 0x31"             \
  "jc perkele"           \
  "shl ebx,16"           \
  "mov bx,cx"            \
  "jmp ok"               \
  "perkele:"             \
  "xor ebx,ebx"          \
  "ok:"                  \
  parm [ebx] [esi] value [ebx] modify [eax ebx ecx edx esi edi];
void unmapphys(long linptr);
#pragma aux unmapphys=   \
  "mov ecx,ebx"          \
  "mov ax,0x801"         \
  "shr ebx,16"           \
  "int 0x31"             \
  parm [ebx] modify [eax ebx ecx edx esi edi];
int vesascroll(int, int);
#pragma aux vesascroll="mov eax, 0x4f07" "xor ebx, ebx" "int 0x10" \
  "and eax, 0xff" parm [ecx][edx] value [eax] modify [eax ebx ecx edx esi edi];



void rmint(char intno, Regs *regs);
#pragma aux rmint="mov ax, 0x300" "xor bh, bh" "xor ecx, ecx" "int 0x31" parm [bl] [edi] modify [eax ebx ecx edx esi edi];





#define pcast(v, t) (*(t*)&(v))

void vcpy_n_n(Copystuf *c) {
  //int i;
  //int *s=(int*)(c->src.buf+c->y*c->dst.pitch+c->x), *d=(int*)c->p;
  //for (i=c->bytes>>2; i; i--) *d++=*s++;
  movsd(c->p, c->src.buf+c->y*c->dst.pitch+c->x, c->bytes>>2);
}


Lfb *vid_openlfb(Video *v) {
  if (use_lfb) {
    v->view.buf-=(int)v->page.buf;
    v->page.buf+=v->page.height*v->page.pitch;
    if (v->page.buf+v->page.height*v->page.pitch>v->vidmem.buf+v->vidmem.height*v->vidmem.pitch)
      v->page.buf=v->vidmem.buf;
    v->view.buf+=(int)v->page.buf;
    return &v->view;
  } else return &v->buf;
}
void vid_copy(Video *v) {
  int w=v->buf.pitch, d, p;
  int y, x2;
  Copyfnc cp;
  void *bf;
  static Copystuf stuph;

  cp=vcpy_n_n;
  bf=v->buf.buf;
  d=v->view.buf-v->vidmem.buf;
  stuph.src.buf=bf; stuph.dst.pitch=v->buf.pitch;
  for (y=0; y<v->buf.height; y++) {
    p=d+0x10000&0xffff0000;
    vesasetbank((d>>16)*v->kerroin);
    stuph.p=v->vidmem.buf+(d&0xffff);
    stuph.x=0; stuph.y=y;
    if (d+v->buf.pitch>=p) {
      stuph.bytes=p-d;
      cp(&stuph);
      vesasetbank((p>>16)*v->kerroin);
      stuph.p=v->vidmem.buf;
      stuph.x=p-d; stuph.bytes=v->buf.pitch+d-p;
      cp(&stuph);
    } else {
      stuph.bytes=v->buf.pitch;
      cp(&stuph);
    }
    d+=v->view.pitch;
  }
}
void vid_closelfb(Video *v) {
  if (use_lfb) {
    static int tmp;
    vesascroll(0, (v->page.buf-v->vidmem.buf)/v->vidmem.pitch);
  } else vid_copy(v);
}
void vid_maximize(Video *v) {
  v->page.width =v->vbemode->xres;
  v->page.height=v->vbemode->yres;
  v->page.depth =v->vbemode->bits+7>>3;
  v->page.pitch =v->vbemode->bytes;
  if (use_lfb) {
    stosb(&v->buf, 0, sizeof(v->buf));
    v->page.buf=mapphys(v->vbemode->lfbaddr, (v->vbemode->imagepages+1)*v->page.height*v->page.pitch);
  } else {
    v->buf=v->page; v->buf.pitch=v->page.width*v->page.depth+3&~3;
    v->page.buf=(char*)0xa0000;
    v->buf.buf=zgetmem(v->buf.height*v->buf.pitch);
  }
  v->vidmem=v->view=v->page;
  v->vidmem.height*=v->vbemode->imagepages+1;
  v->vidmem.width=v->vidmem.pitch/v->vidmem.depth;
}


static int test(Video *v, int x, int y, int b) {
  unsigned short *mnum;
  Vbeinfo *inf;
  Vbemode *mod;

  if (!v->dosbuf ) v->dosbuf =(void*)(allocdosmem(1024>>4)<<4);
  inf=v->vbeinfo; mod=v->vbemode;
  if (!inf) inf=v->vbeinfo=(void*)(allocdosmem(sizeof(Vbeinfo)>>4)<<4);
  if (!mod) mod=v->vbemode=(void*)(allocdosmem(sizeof(Vbemode)>>4)<<4);

  stosb(inf, 0, sizeof(Vbeinfo));
  inf->sign=0x32424556; // 'VBE2'
  {
    Regs r;
    stosb(&r, 0, sizeof(r));
    r.ax=0x4f00;
    r.di=0;
    r.es=((int)inf)>>4;
    rmint(0x10, &r);
    if (r.ax!=0x4f) return 0;
    if (inf->sign!=0x41534556) return 0; // 'VESA'
    if (inf->ver<258) return 0;
    {
      for (mnum=ptr16to32(inf->modelist); *mnum; mnum++) {
        stosb(&r, 0, sizeof(r));
        r.ax=0x4f01;
        r.cx=*mnum;
        r.di=0;
        r.es=((int)mod)>>4;
        rmint(0x10, &r);
        if (r.ax!=0x4f) return 0;
        if (mod->xres==x && mod->yres==y && mod->bits==b &&
            (mod->memmodel==4||mod->memmodel==6) && (mod->modeattr&0x11)==0x11) break;
      }
      if (!*mnum) return 0;
    }
  }

  if (*mnum<0x100) return 0;
  if (mod->imagepages<2) use_lfb=0;
  if (inf->ver<512) use_lfb=0;
  if (mod->lfbaddr<0x100000) use_lfb=0;
  if (!(mod->modeattr&0x80)) use_lfb=0;
  if (mod->wgran) v->kerroin=64/mod->wgran;
  //printf("speaking to %s ver %i.%i\nusing %i at %i with %i knowing that %i.. right?\n",
    //ptr16to32(inf->oemstr), inf->ver>>8, inf->ver&255,
    //mod->xres, mod->yres, mod->bits, mod->wgran);
  if ((vesasetmode(*mnum|-use_lfb&0x4000)&0xff)!=0x4f) return 0;
  v->mnum=*mnum;
  vid_maximize(v);
  return 1;
}
Video *new_vid_screen(int width, int height, int bpp) {
  Video *v=zgetmem(sizeof(Video));
  if (test(v, width, height, bpp)) return v;
  return 0;
}
int vid_cut(Video *v, int x0, int y0, int wid, int hgt) {
  if (x0<0 || y0<0 || x0+wid>v->view.width || y0+hgt>v->view.height
    || wid<1 || hgt<1) return 0;
  v->view.width=wid; v->view.height=hgt;
  v->view.buf+=x0*v->view.depth+y0*v->view.pitch;
  if (!use_lfb) {
    v->buf.width=v->view.width; v->buf.height=v->view.height;
    v->buf.pitch=v->buf.width*v->buf.depth+3&~3;
  }
  return 1;
}
int vid_ydiv(Video *v, int divisor) {
  v->view.pitch*=divisor; v->view.height/=divisor;
  if (!use_lfb) {
    v->buf.width=v->view.width; v->buf.height=v->view.height;
    v->buf.pitch=v->buf.width*v->buf.depth+3&~3;
  }
  return 1;
}
void vid_close(Video *v) {
  if (v->mnum) vesasetmode(0x3), vgasetmode(0x3), v->mnum=0;
}

int vid_set_gamma(Video *v, float gma) {
  int i;
  Regs r;
  if (!(v->vbemode->dcattr&1) && !force_ramp) return 0;
  stosb(&r, 0, sizeof(r)); r.ax=0x4f08; r.bh=8; rmint(0x10, &r);
  if (r.ax!=0x4f) return 0;
  for (i=0; i<255; i++) {
    int c=fpow(i*(1/255.0), gma)*255.4;
    ((int*)v->dosbuf)[i]=c|c<<8|c<<16;
  }
  for (i=0; i<256; i++) {
    outb(0x3c8, i);
    outb(0x3c9, ((char*)v->dosbuf)[4*i+0]);
    outb(0x3c9, ((char*)v->dosbuf)[4*i+1]);
    outb(0x3c9, ((char*)v->dosbuf)[4*i+2]);
  }
  stosb(&r, 0, sizeof(r)); r.ax=0x4f09; r.cx=256; r.es=(int)v->dosbuf>>4; rmint(0x10, &r);
  if (r.ax!=0x4f) return 0;
  return 1;
}

