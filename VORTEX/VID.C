#include "tinyheap.h"
#include "vid.h"

struct Vesastuf vesastuf;

void dosputs(char *stringi);
#pragma aux dosputs=\
  "mov ah,9"        \
  "int 21h"         \
  parm [edx] modify [eax ebx ecx edx esi edi];
void fill32(void *dezt, int kount, int num);
#pragma aux fill32= "cld" "rep stosd" parm[edi][ecx][eax];

char cube[65536];
void vgasetmode(int);
#pragma aux vgasetmode="int 0x10" parm [eax] modify [eax ebx ecx edx esi edi];
int vesasetmode(int);
#pragma aux vesasetmode="mov eax,0x4f02" "int 0x10" "and eax,0xff" parm [ebx] value [eax] modify [eax ebx ecx edx esi edi];
void vesasetbank(int);
#pragma aux vesasetbank="mov eax,0x4f05""xor ebx,ebx""int 0x10" parm [edx] modify [eax ebx ecx edx esi edi];

#define pcast(v, t) (*(t*)&(v))
void vcpy_my_16(void *_dest2, void *_src2, int _x, int _y, int _bytes, int _lines, int _width);
void vcpy_16_08(void *dest, void *src, int x, int y, int pixels, int lines, int width) {
  int x2, y2;
  pcast(src, short*)+=y*width+x;
  for (y2=0; y2<lines; y2++, pcast(src, short*)+=width, pcast(dest, char*)+=width)
    for (x2=0; x2<pixels; x2++) pcast(dest, char*)[x2]=cube[pcast(src, unsigned short*)[x2]];
}
void vcpy_n_n(void *dest, void *src, int x, int y, int pixels, int lines, int width) {
  int x2, y2;
  pcast(src, char*)+=y*width+x;
  for (y2=0; y2<lines; y2++, pcast(src, char*)+=width, pcast(dest, char*)+=width)
    for (x2=0; x2<pixels>>2; x2++) pcast(dest, long*)[x2]=pcast(src, long*)[x2];
}

//extern char *logoptr;
//extern unsigned short logopal[16];

int v_width() { return vesastuf._w; }
int v_height() { return vesastuf._h; }
int v_bpp() { return vesastuf._b; }
void *v_getlfb() { return vesastuf._buf; }
void v_copy() {
  int w=vesastuf._w*(vesastuf._b+7>>3), d=vesastuf._ysh*w, p;
  int x, y;
  Copyfnc cp;
  void *bf;
  static void *tmpbf=0;
  static int tmpsz=0;
  unsigned short *des;
  char *sou;

  if (vesastuf._h*vesastuf._w*2!=tmpsz) {
    if (tmpbf) free(tmpbf);
    tmpbf=malloc(tmpsz=vesastuf._h*vesastuf._w*2);
  }
/*  if (vesastuf._b==16 && !logoptr) {
    cp=vcpy_my_16; bf=vesastuf._buf;
  } else {
    if (vesastuf._b!=16) cp=vcpy_16_08; else cp=vcpy_n_n;
    bf=tmpbf;
    vcpy_my_16(tmpbf, vesastuf._buf, 0, 0, vesastuf._w*2, vesastuf._h, vesastuf._w*2);
    if (logoptr) {
      sou=logoptr;
      for (y=0; y<200; y++) {
        des=((unsigned short*)tmpbf)+((vesastuf._h-200>>1)+y)*vesastuf._w+(vesastuf._w-320>>1);
        for (x=0; x<320; x+=2, sou++, des+=2) {
          if (*sou&0xf) des[0]=logopal[*sou&0xf];
          if (*sou>>4&0xf) des[1]=logopal[*sou>>4&0xf];
        }
      }
    }
  }*/
  if (vesastuf._b!=16) cp=vcpy_16_08; else cp=vcpy_n_n;
  bf=vesastuf._buf;

  if (vesastuf._vesa) vesasetbank(d>>16);
  for (y=0; y<vesastuf._h;) {
    p=d+0x10000&0xffff0000;
    if (d+w>=p) {
      cp(vesastuf._vbuf+(d&0xffff), bf, 0, y, p-d, 1, w);
      if (vesastuf._vesa) vesasetbank(p>>16);
      cp(vesastuf._vbuf, bf, p-d, y, w+d-p, 1, w);
      d+=w; y++;
    } else {
      int n;
      for (n=1; d+(n+1)*w<p && y+n<vesastuf._h; n++) ;
      cp(vesastuf._vbuf+(d&0xffff), bf, 0, y, w, n, w);
      d+=w*n; y+=n;
    }
  }
}

void calccube() {
  static char pal[2048];
  int c, cc, e, e2, r, g, b, i, rr, gg, bb, ii;
  _asm {
    xor ecx, ecx
    luup:
      mov edx, 3c7h
      mov al, cl
      out dx, al
      add edx, 2
      in al, dx
      mov byte ptr[ecx*8+pal+0], al
      in al, dx
      mov byte ptr[ecx*8+pal+1], al
      in al, dx
      mov byte ptr[ecx*8+pal+2], al
      inc cl
    jnz luup
  };
  _asm {
    mov eax,3h
    int 10h
  };
  for (c=0; c<256; c++) {
    pal[c*8+3]=pal[c*8+0]*0.4+pal[c*8+1]*0.5+pal[c*8+2]*0.2;
  }

  dosputs("Colormaps: $");
  for (cc=0; cc<65536; cc++) {
    e=0x7fffffff; cube[cc]=0;
    r=cc>>10&0x3e; g=cc>>5&0x3f; b=cc<<1&0x3e; i=r*0.4+g*0.5+b*0.2;
    for (c=0; c<256; c++) {
      rr=r-pal[c*8+0];
      gg=g-pal[c*8+1];
      bb=b-pal[c*8+2];
      ii=i-pal[c*8+3];
      e2=rr*rr+gg*gg+bb*bb+ii*ii;
      if (e2<e) { cube[cc]=c; e=e2; }
    }
    if ((cc&0x3ff)==0x3ff) dosputs(".$");
  }
}


int test(int x, int y, int ysh, int b, int mnum) {
  if (mnum>=0x100) {
    vesastuf._vesa=1;
    if ((vesasetmode(mnum)&0xff)!=0x4f) return 0;
  } else {
    vesastuf._vesa=0;
    _asm {
      mov eax,13h
      int 10h
    };
  }
  if (b==8) {
    calccube();
    if (mnum>=0x100) {
      vesastuf._vesa=1;
      if ((vesasetmode(mnum)&0xff)!=0x4f) return 0;
    } else {
      vesastuf._vesa=0;
      _asm {
        mov eax,13h
        int 10h
      };
    }
  }
  vesastuf._w=x; vesastuf._h=y; vesastuf._b=b; vesastuf._ysh=ysh; vesastuf._mde=mnum;
  if (vesastuf._buf) free(vesastuf._buf);
  vesastuf._buf=malloc(vesastuf._h*vesastuf._w*(vesastuf._b+7>>3)*4);
  fill32(vesastuf._buf, vesastuf._h*vesastuf._w*(vesastuf._b+7>>3), 0);
  vesastuf._vbuf=(char*)0xa0000;
  return 1;
}
int v_open(int x) {
//136=320  151=400  13d=512  111=640  114=800  117=1024 */
/*  switch (x) {
    case 3:*/
//      if (test(320, 200, 20, 16, 0x136)) return 1;
      if (test(320, 200, 0, 16, 0x10e)) return 1;
//      if (test(320, 200, 20, 8, 0x164)) return 1;
      if (test(320, 200, 0, 8, 0x163)) return 1;
      if (test(320, 200, 0, 8, 0x13)) return 1;
/*      break;
    case 4:
      if (test(400, 240, 30, 16, 0x151)) return 1;
      if (test(400, 240, 30, 8, 0x14f)) return 1;
      break;
    case 5:
      if (test(512, 320, 32, 16, 0x13d)) return 1;
      if (test(512, 320, 32, 8, 0x12d)) return 1;
      break;
    case 6:
      if (test(640, 400, 40, 16, 0x111)) return 1;
      if (test(640, 400, 40, 8, 0x101)) return 1;
      break;
    case 8:
      if (test(800, 480, 60, 16, 0x114)) return 1;
      if (test(800, 480, 60, 8, 0x103)) return 1;
      break;
    case 10:
      if (test(1024, 640, 64, 16, 0x117)) return 1;
      if (test(1024, 640, 64, 8, 0x105)) return 1;
      break;
  }*/
  return 0;
}
int v_close() {
  if (vesastuf._mde) {
    vesasetmode(0x3); vesastuf._mde=0;
    return 1;
  } else return 0;
}

