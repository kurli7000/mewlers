#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"
#include "obuja.h"
#include "functs.h"
#include "font.h"
#include "video.h"
#include "phile.h"
#include "mesh.h"
#include <lib2\modplyer.h>
#include "inifile.h"


#define radixres 512
#define radiyres 256
int *radi2norm, *norm2radi, exptbl1[1024], exptbl2[1024];



extern void setatmo(int);
float guess(float t) { return 2.0/(fexp2(t)+fexp2(-t)); }



static Video *video;
static Inifile *cfg;
Fontti *fnt;
int tabloid[512];
int rtbl2[16384];
#define rtbl (rtbl2+8192)
int radi_xmov, radi_ymov;
char thxlogo[320*180], digital[320*180], thxblur[320*180];
extern float jytol_kludge;
extern int   koamk_kludge;
Lfb *khuva1, *khuva2, *khuva3;
Lfb *lobuf8, *lobuf16;


static Texture *maa_txt, *taivas_txt, *la_txt, *hava_txt, *anta_txt, *pilvet_txt;
static Material *maa, *taivas, *la, *hava, *anta, *pilvet, *blasm1, *blasm2;
static Mesh *maapallo, *maataivas, *los, *havaji, *puolihava, *antar, *pilvitaivas, *matto;
static Space *spc;


Matrix *teamatrix(float dx, float dy, float dz) {
  float l=fsqrt(dx*dx+dy*dy+dz*dz), r=fcos(l*.5), ll=fsin(l*.5)/l;
  float r2, a2, b2, c2, ar, br, cr, ab, bc, ca;
  Matrix *m=new_m();
  dx*=ll; dy*=ll; dz*=ll;
  r2=r*r; a2=dx*dx; b2=dy*dy; c2=dz*dz;
  ar=dx*r; br=dy*r; cr=dz*r;
  ab=dx*dy; bc=dy*dz; ca=dz*dx;
  m->i.x=r2+a2-b2-c2;
  m->i.y=2*(ab+cr);
  m->i.z=2*(ca-br);
  m->j.x=2*(ab-cr);
  m->j.y=r2-a2+b2-c2;
  m->j.z=2*(bc+ar);
  m->k.x=2*(ca+br);
  m->k.y=2*(bc-ar);
  m->k.z=r2-a2-b2+c2;
  m->l.x=m->l.y=m->l.z=0;
  return m;
}



Lfb *new_img(int wid, int hgt, int dep) {
  Lfb *l=getmem(sizeof(Lfb));
  l->buf=getmem((l->pitch=(l->width=wid)*(l->depth=dep))*(l->height=hgt));
  return l;
}

Lfb *reallfb, *fakelfb;
Lfb *xopenlfb() {
  reallfb=vid_openlfb(video);
  if (reallfb->depth==1) return fakelfb=new_img(reallfb->width, reallfb->height, 2);
    else return reallfb;
}
void xcloselfb() {
  if (reallfb->depth==1) {
    int x, y;
    for (y=0; y<reallfb->height; y++) {
      short *p=(short*)(fakelfb->buf+y*fakelfb->pitch);
      char *q=(char*)(reallfb->buf+y*reallfb->pitch);
      for (x=0; x<reallfb->width; x++, p++, q++)
        *q=(*p>>10&0x3e)+(*p>>4&0x7e)+(*p<<1&0x3e);
    }
  }
  vid_closelfb(video);
}


Lfb *lfb_clear(Lfb *l) { stosd(l->buf, 0, l->height*l->pitch>>2); return l; }
Lfb *new_img_read_tga(Phile *ph) {
  struct {
    char idlen;
    char paltype;
    char imtype;
    char palstuf[5];
    short x, y;
    short width, height;
    char bpp;
    char flags;
  } hdr;
  int size, x, y, i, j, k, pitch;
  Lfb *lfb;//=zgetmem(sizeof(Lfb));
  //FILE *fp=fopen(fname, "rb");
  phile_seek(ph, 0); phile_read(ph, &hdr, 18);
  //fread(&hdr, 1, 18, fp);
  phile_seek(ph, hdr.idlen+18);
  //lfb->width=hdr.width;
  //lfb->height=hdr.height;
  //lfb->depth=hdr.bpp+7>>3;
  //lfb->pitch=lfb->depth*lfb->width;
  lfb=new_img(hdr.width, hdr.height, hdr.bpp+7>>3);
  size=lfb->pitch*lfb->height;
  phile_read(ph, lfb->buf=getmem(size), size);
  //fclose(fp);
  if (~hdr.flags&0x20) for (y=0; y*2<lfb->height; y++) {
    int y1=y*lfb->pitch, y2=(lfb->height-1-y)*lfb->pitch;
    for (x=0; x<lfb->pitch; x++)
      j=lfb->buf[y1+x], lfb->buf[y1+x]=lfb->buf[y2+x], lfb->buf[y2+x]=j;
  }
  if (hdr.bpp==16||hdr.bpp==15) for (x=0; x+x<lfb->width*lfb->pitch; x++)
    ((short*)lfb->buf)[x]+=((short*)lfb->buf)[x]&0xffe0;
  return lfb;
}
Lfb *img_to16bpp(Lfb *l) {
  if (l->depth==3) {
    int x, y;
    for (y=0; y<l->height; y++)
      for (x=0; x<l->width; x++) ((short*)(l->buf+y*l->pitch))[x]=l->buf[y*l->pitch+x*3+2]<<8&0xf800|l->buf[y*l->pitch+x*3+1]<<3&0x7e0|l->buf[y*l->pitch+x*3]>>3&0x1f;
    l->depth=2;
  }
  return l;
}


Lfb *lfb;

void fadeimg(Lfb *dest, Lfb *src, int fade1, int fade2) {
  static int rt[1024], gt[4096], bt[1024];
  int i, j, k, x, y;
  int sinc=65536*src->width/dest->width, sx=0;
  if (fade1>65000 && fade1<66000 && fade2<1000) return;
  for (j=0; j<64; j++)  for (i=0; i<64; i++)
    k=i*fade1+j*fade2>>16, gt[j*64+i]=(k>63?63:k)<<6;
  for (j=0; j<32; j++)  for (i=0; i<32; i++)
    k=i*fade1+j*fade2>>15, bt[j*32+i]=k>31?31:k;
  for (j=0; j<32; j++)  for (i=0; i<32; i++)
    k=i*fade1+j*fade2>>15, rt[j*32+i]=(k>31?31:k)<<11;
  for (y=0; y<dest->height; y++) {
    int *dp=(int*)(dest->buf+y*dest->pitch), *dpm=dp+(dest->width>>1);
    short *sp=(short*)(src->buf+(y*src->height/dest->height)*src->pitch);
    int a, b, c;
    for (sx=0; dp<dpm; dp++, sx+=sinc+sinc) {
      a=*dp; b=sp[sx>>16]; c=sp[sx+sinc>>16];
      *dp=(rt[(a>>27&0x1f)+(c>>6&0x3e0)]+gt[(a>>21&0x3f)+(c<<1&0xfc0)]+bt[(a>>16&0x1f)+(c<<5&0x3e0)])<<16
         |(rt[(a>>11&0x1f)+(b>>6&0x3e0)]+gt[(a>> 5&0x3f)+(b<<1&0xfc0)]+bt[(a    &0x1f)+(b<<5&0x3e0)]);
    }
  }
}
void fadeimg8(Lfb *dest, Lfb *src, int fade1, int fade2) {
  static char tbl[16384];
  int i, j, k, x, y;
  int sinc=65536*src->width/dest->width, sx=0;
  for (j=0; j<256; j++) for (i=0; i<64; i++)
    k=i*fade1*4+j*fade2>>18, tbl[j*64+i]=k>63?63:k;
  for (y=0; y<dest->height; y++) {
    int *dp=(int*)(dest->buf+y*dest->pitch);
    char *sp=src->buf+(y*src->height/dest->height)*src->pitch;
    for (x=sx=0; x<dest->width; x+=2, dp++, sx+=sinc+sinc) {
      int a=*dp, b=sp[sx>>16], c=sp[sx+sinc>>16];
      *dp=tbl[a>>26&0x3e|c<<6&0x3fc0]<<26&0xf8000000
         |tbl[a>>21&0x3e|c<<6&0x3fc0]<<21&0x7e00000
         |tbl[a>>15&0x3e|c<<6&0x3fc0]<<15&0x1f0000
         |tbl[a>>10&0x3e|b<<6&0x3fc0]<<10&0xf800
         |tbl[a>> 5&0x3e|b<<6&0x3fc0]<< 5&0x7e0
         |tbl[a<< 1&0x3e|b<<6&0x3fc0]>> 1&0x1f;
    }
  }
}
Lfb *kewlplur(Lfb *l) {
  int x, y, i, j;
  for (y=0; y<l->height; y++) {
    short *p=(short*)(l->buf+y*l->pitch);
    i=p[0]*256;
    for (x=0; x<l->width; x++, p++) {
      //p[0]=(i+=p[0]*256-i+4>>3)+128>>8; p++;
       j=p[0]*4+((i+=p[0]*256-i+4>>3)+2>>2); if (j>65535) j=65535; p[0]=j;
    }
  }
  return l;
}
Lfb *vihrea(Lfb *l) {
  int x, y, i;
  for (y=0; y<l->height; y++) {
    unsigned short *p=(unsigned short*)(l->buf+y*l->pitch);
    i=p[0]*256;
    for (x=0; x<l->width; x+=4, p+=4) {
      p[0]=p[0]>>5&0x7e0; p[1]=p[1]>>5&0x7e0; p[2]=p[2]>>5&0x7e0; p[3]=p[3]>>5&0x7e0;
    }
  }
  return l;
}


volatile char keys[144], kcnt[144];
void (__interrupt __far __loadds *oldk)();
static void __interrupt __far __loadds newk() {
  int a;
  a=inb(0x60);
  if (a<0x80) { keys[a]=1; kcnt[a]++; } else keys[a-0x80]=0;

  a=keys[128]; keys[128]=keys[29]&&keys[56]&&keys[83]; kcnt[128]+=keys[128]&&!a;
  a=keys[129]; keys[129]=keys[29]&&keys[69]; kcnt[129]+=keys[129]&&!a;
  outb(0x20, 0x20);

  if ((kcnt[128]&1)&&!keys[128]) {
    unint();
    _asm {
      mov eax,3h
      int 10h
    };
    dosputs("Now it's safe to turn off your computer.\r\n$");
    exit(1);
  }
}
static void setint() {
  oldk=get_intr(0x9); set_intr(0x9, newk);
}
static void unint() {
  set_intr(0x9, oldk);
}



void init_engine() {
  extern float monitor_gamma;
  extern float texel_size;
  cfg=lueini("demo.ini");
  //texture_contrast=ini_float(cfg, "texture_contrast", 15.);
  monitor_gamma=ini_float(cfg, "monitor_gamma", 1.8);
  //palette_gamma=ini_float(cfg, "ramp_gamma_correct", .6);
  //force_ramp=ini_int(cfg, "force_ramp", 0);
  texel_size=ini_float(cfg, "texel_size", 0.);
  if (ini_int(cfg, "disable_lfb", 0)) use_lfb=0;
  {
    int wid=ini_int(cfg, "width", 320), hgt=ini_int(cfg, "height", 200),
      bpp=ini_int(cfg, "bpp", 16), ydiv=ini_int(cfg, "y_divisor", 1),
      wid2=ini_int(cfg, "window_width", wid), hgt2=ini_int(cfg, "window_height", hgt/ydiv);
    video=new_vid_screen(wid, hgt, bpp);
    if (!video) {
      int ch, i;
      printf("%ix%ix%i not found\nuse 320x200x8 grayscale instead? [no/yes]? ", wid, hgt, bpp); fflush(stdout);
      ch=getch(); if (ch!='Y' && ch!='y') exit(1);
      printf("\nargh..\n\n");
      ydiv=1; wid=320; hgt=200; bpp=8; wid2=320; hgt2=150; bpp=8;
      video=new_vid_screen(320, 200, 8);
      if (!video) erreur("eh, 320x200x256 vesa:)");
      for (i=0; i<256; i++) outb(0x3c8, i), outb(0x3c9, i>>2), outb(0x3c9, i>>2), outb(0x3c9, i>>2);
    //erreur("cannot init video");
    }
    vid_ydiv(video, ydiv);
    vid_cut(video, wid-wid2>>1, hgt/ydiv-hgt2>>1, wid2, hgt2);
  }
  //if (!vid_set_gamma(video, 1.0/palette_gamma)) palette_gamma=1.;
}


/*********************** PREKALKIT ***********************/

/*
typedef struct {
  int u, v;
} Tunneli;
Tunneli *tunneli;
void precalctaul(int wid, int hgt) {
  int x, y, wid, hgt;
  float t, x2, y2, u, v;

  Lfb *l=xopenlfb(); wid=l->width; hgt=l->height; xcloselfb(video);
  tunneli=getmem(wid*hgt*sizeof(*tunneli));
  for (y=0; y<hgt; y++) for (x=0; x<wid; x++) {
    x2=(x/(float)wid-.499); y2=(y/(float)hgt-.499)*9.0/16;
    u=fpatan(x2, y2)*16.*128.0/3.14159265358979323846264;
    v=-flog2(x2*x2+y2*y2)*.5*16.*256.;
    //v=64.0/fsqrt(x2*x2+y2*y2)*.5*256.;
    //v=-flog2(fsqrt(x2*x2+y2*y2))*256.;
    tunneli[y*wid+x].u=u;
    //if (v>8191) v=8191; else if (v<0) v=0;
    tunneli[y*wid+x].v=v;
  }
}
*/





void premfxthx() {
  int x, y, i, a;
  float u, v, s, t;
  int xsize, ysize;
  Phile *ph;

  ph=new_phile_disk_ro("mfxthx3.raw"); phile_seek(ph, 320*10); phile_read(ph, thxlogo, 320*180); phile_close(ph);
  ph=new_phile_disk_ro("mfxthx5.raw"); phile_seek(ph, 320*10); phile_read(ph, thxblur, 320*180); phile_close(ph);
  ph=new_phile_disk_ro("mfxthx5j.raw"); phile_seek(ph, 320*10); phile_read(ph, digital, 320*180); phile_close(ph);
  norm2radi=getmem(640*360*4);
  radi2norm=getmem(radixres*radiyres*4);

  for (i=0; i<1024; i++) {
    float f=i/128.; f=f<1?f:1; f=fsqrt(f);
    exptbl1[i]=fpow(i/1024.+.0001, fpow( .25,f+.0001))*512.;
  }
  for (i=0; i<256; i++) {
    float f=i/128.; f=f<1?f:1; f=fsqrt(f);
    exptbl2[i]=fpow(i/256. +.0001, fpow( 4.00,f+.0001))*16384.;
  }
  for (i=0; i<512; i++) tabloid[i]=fsin(i*pi/256.)*2047+2048;

  for (x=0; x<8192; x++) rtbl2[x]=0;
  for (x=8192; x<8192+256; x++) rtbl2[x]=x-8192;
  for (x=8192+256; x<16384; x++) rtbl2[x]=255;

  xsize=640; ysize=360;
  a=radixres*radiyres;
  for (y=0; y<ysize; y++) for (x=0; x<xsize; x++) {
    u=(xsize*.5-.25-x)/xsize;
    v=(ysize*.5-.25-y)*.75/ysize;
    t=fpatan(u, v)/pi*(radixres/2.)+(radixres/2.);
    s=fsqrt(u*u+v*v)*1.4;
    s=fpow(s, .6);
    s*=radiyres;
    s-=t*(1.0/radixres);
    if (s<0) s=0;//erreur("s<0");
    //norm2radi[y*xsize+x]=(fist2(s)&(radiyres-1))*radixres+((int)(t)&(radixres-1));
    norm2radi[y*xsize+x]=((int)(s)&(radiyres-1))*radixres+((int)(t)&(radixres-1));
  }
  for (y=0; y<radiyres; y++) for (x=0; x<radixres; x++) {
    t=x*(1.0/radixres);
    s=fpow((y+1.00001-t)*(1.0/radiyres), 1/0.6)/1.4;
    t*=pi*2;
    u=319.75+fcos(t)*s*640;
    v=179.75-fsin(t)*s*360*(9.0/16);
    if (u<10) u=10; if (u>629) u=629; if (v<10) v=10; if (v>349) v=349;
    radi2norm[y*radixres+radixres-1-x]=(int)v*640+(int)u;
  }
}

/*********************** EFEKTIT ***********************/


/*********************** LES INNERLOUPES ***********************/




void teeradiaali(char *sourke, int *dest) {
  int x, y;
  for (y=0; y<radiyres*radixres; y+=radixres*2) {
    for (x=y; x<y+radixres; x++) {
      dest[x         ]=exptbl2[sourke[radi2norm[x         ]]];
      dest[x+radixres]=exptbl2[sourke[radi2norm[x+radixres]]];
    }
  }
  //for (x=0; x<xres*2*yres*2; x++) dest[norm2radi[x]]=tbl2[sourke[x]];
}
void blur2(int *dest, int delta, int p) {
  int *d;
  int size=radixres*radiyres;

  p>>=2;

  if (delta<0) {
    //dest[0]=16384;
    for (d=dest-delta; d<dest+size;  d++) { d[0]+=(d[delta]-d[0])*p>>14; }
  } else {
    //for (d=dest+size-1; d>=dest+size-1-delta; d--) *d=16384;
    for (d=dest+(size-1)-delta; d>=dest; d--) { d[0]+=(d[delta]-d[0])*p>>14; }
  }
}



void tee2dvalo(char *dest, int t, int br) {
  int x, y, i, j, k;
  int xtemp[320], ytemp[180];

  for (x=0; x<320; x++) xtemp[x]=fsqr(320-x-radi_xmov);
  for (y=0; y<180; y++) ytemp[y]=fsqr(180-y-radi_ymov);
  for (y=0; y<180; y++) for (x=0; x<320; x++) {
    if (thxlogo[y*320+x]>0) i=0;
    else {
      i=70000-fsqrt(xtemp[x]+ytemp[y]+.5)*270.;
      k=tabloid[t+y-x&511]>>2;
      j=(tabloid[y*5-t+k&511]+tabloid[y*7+t&511]+tabloid[-x*8-k&511]+tabloid[x*4+t&511]>>6)+256;
      i=i*j>>17;
      //i=i>>8;
      if (i>255) i=255; if (i<0) i=0;
    }
    if (br==65536) dest[(y+radi_ymov)*640+x+radi_xmov]=i;
      else dest[(y+radi_ymov)*640+x+radi_xmov]=i*br>>16;
  }
}

void logotus(char *dest, int t) {
  int x, y, i, j;
  int xtemp[320];
  if (t<128) {
    for (i=0; i<320*180; i++) {
      dest[i]=(thxlogo[i]>>1)+(digital[i]*(t*512)>>16);
    }
  } else if (t<512) {
    for (x=0; x<320; x++) {
      i=40000/((x-t*1.3+300)*(x-t*1.3+300)*.0003+.5);
      if (i<0) i=0;
      xtemp[x]=i;
    }
    for (i=0, y=0; y<180; y++) for (x=0; x<320; x++, i++) {
      dest[i]=rtbl[(thxlogo[i]>>1)+(thxblur[i]*xtemp[x]>>16)+digital[i]];
    }
  } else if (t<1024) {
    j=65535-(t-512)*128; if (j<0) j=0;
    for (i=0; i<320*180; i++) {
      dest[i]=(((thxlogo[i]>>1)+digital[i])*j>>16);
    }
  }
}
Lfb *mfxthx(float t) {
  int x, y, radi_xmov, radi_ymov, k;
  int *radibuf=getmem(radixres*radiyres*4);
  float j;
  Lfb *dest=zgetmem(sizeof(Lfb));
  //char *buf=getmem(lfb->width*lfb->height*4);

  dest->width=dest->pitch=640; dest->height=360; dest->depth=1;
  dest->buf=getmem(640*360);

  radi_xmov=fsin(t*.044)*120+160;
  radi_ymov=fcos(t*.062)*60+90;
  if (t<32) j=t*2048; else j=(400-t)*2048; if (j>65536) j=65536;
  k=(t-(400-32))*2048; if (k>65536) k=65536;

  if (t<400) {
    tee2dvalo(dest->buf, t*5, j);
    teeradiaali(dest->buf, radibuf);
    //blur2(radibuf, -radixres, 60000);
    //blur2(radibuf, -radixres+1, 40000);
    for (y=0; y<180; y++) for (x=0; x<320; x++)
      dest->buf[y*320+x]=exptbl1[radibuf[ norm2radi[(y+radi_ymov)*640+x+radi_xmov] ]>>4];
  }
  if (t>=368 && t<400) {
    for (y=0; y<180; y++) for (x=0; x<320; x++)
      dest->buf[y*320+x]=rtbl[dest->buf[y*320+x]+(thxlogo[y*320+x]*k>>17)];
  }
  if (t>=400) logotus(dest->buf, t-400);
  dest->width=dest->pitch=320; dest->height=180;
  return dest;
}


//Mesh *gridply(Vector *v1, Vector *v2, Vector *v3, Vector *v4, int cnt) {

//}


void pyoritamaata(float t) {
  float dist;
  float xp, yp, zp;
  float xt, yt, zt;

  dist=500;
  xp=fsin(t*1.2256)*dist;
  yp=fsin(t*.8775)*300;
  zp=fcos(t*1.2256)*dist;
  xt=0;
  yt=50*t;
  zt=0;

  //maa
  spc=new_space();
  space_fill(spc, new_v_c(.0000001, .0000001, .0000001));
  space_setambient(spc, new_v_c(.04, .03, .07));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(1, .3, 0)), 20000.),
    new_v_c(500000000., 500000000., 500000000.));
  {
    float t1=t*.5;
    Matrix *m1=new_m_create(new_v_c(fcos(t1), 0, fsin(t1)), &v_cj, new_v_c(-fsin(t1), 0, fcos(t1)), &v_c0);
    space_addmesh(spc, maapallo, m1);
  }
  space_render(spc, lfb, new_m_lookat(new_v_c(xp, yp, zp),
                                      new_v_c(xt, yt, zt),
                                      v_norm(new_v_c(-.3, 1, 0))), 5, 0);


  //taivas
/*  spc=new_space();
  space_setambient(spc, new_v_c(.004, .003, .007));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(1, .3, 0)), 20000.),
    new_v_c(500000000., 500000000., 500000000.));
  {
    float t1=t*.8;
    Matrix *m1=new_m_create(new_v_c(fcos(t1), 0, fsin(t1)), &v_cj, new_v_c(-fsin(t1), 0, fcos(t1)), &v_c0);
    space_addmesh(spc, maataivas, m1);
  }
  space_render(spc, lfb, new_m_lookat(new_v_c(xp, yp, zp),
                                      new_v_c(xt, yt, zt),
                                      v_norm(new_v_c(-.3, 1, 0))), 5, 0);*/
}


void losangeles(float t) {
  Vector cam, tgt, v1, v2, v3;
  float range, juttu;
  jytol_kludge=1000.; setatmo(0);

  if (t<1) juttu=fsqr(1-t)*500+30; else juttu=30;
  if (t<1) range=(fsin(t*pi*.5)+1)*.5; else range=1;
  if (t>10) koamk_kludge=1;

  //v_create(&cam, fsin(t*.7423)*220*range, 70+juttu+guess((t-14)*1.3)*100-guess((t-14)*10-1)*150, fsin(t*1.2123)*80*range);
  //v_create(&tgt, fsin(t*.7423+.1)*220*range*.8, 75-guess((t-14)*10-1)*200, fsin(t*1.2123+.1)*80*range*.8);
  v_create(&cam, fcos(t*.2423)*270*range, 90, fsin(t*.3123)*270*range);
  v_create(&v1, fcos((t+.01)*.2423)*270*range, 90, fsin((t+.01)*.3123)*270*range);
  v_sub(&v1, &v1, &cam); v_norm(&v1);
  v_create(&v2, fsin((t+.01)*2.2423)*70*range, 70+fcos((t+.01)*1.7123)*20, fsin((t+.01)*1.7123)*70*range);
  v_sub(&v2, &v2, &cam); v_norm(&v2);

  juttu=(1+v_dot(&v1, &v2))*0.5; juttu*=juttu;
  v_mul(&v1, &v1, juttu);
  v_mul(&v2, &v2, 1-juttu);
  v_add(&tgt, &v1, &v2);
  v_add(&tgt, &tgt, &cam);
  //v_mul(&tgt, &tgt, 0.99);
  //v_create(&tgt, fsin(t*.7423+.01)*320*range, 90, fsin(t*1.2123+.01)*320*range);

  spc=new_space();
  space_fill(spc, v_mul(new_v(), new_v_c(.3, .4, .5), 1.));
  space_setambient(spc, new_v_c(.04, .03, .07));
  //space_addlight(spc,
    //v_mul(new_v(), v_norm(new_v_c(1, .3, 0)), 20000.),
    //new_v_c(500000000., 500000000., 500000000.));
  {
    float t1=t*.01;
    Matrix *m1=new_m_create(new_v_c(fcos(t1), 0, fsin(t1)), &v_cj, new_v_c(-fsin(t1), 0, fcos(t1)), &v_c0);
    space_addmesh(spc, los, m1);
  }

  //space_addmesh(spc, los,
    //new_m_create(new_v_c(0,0,0), &v_cj,
                 //new_v_c(0,0,0), &v_c0));
  space_render(spc, lfb, new_m_lookat(&cam, &tgt,
    v_norm(new_v_c(-.3, 1, 0))), 2, 0);

  jytol_kludge=0;
}



void hawaii(float t) {
  float aa=1+guess(t*3)*2, bee=1-guess((t-8)*.5)*.7;
  float per=0.7+guess((t-8)*6-1)*5;
  Vector *cam=new_v_c(fsin(t*1.551)*300*aa, 52+fsin(t*.5433)*5+guess(t-18), fcos(t*2.123)*300*aa),
         *tgt=new_v_c(fsin(t*1.551*1.05+.1)*240*bee, 45+guess((t-8)*4)*1500, fcos(t*2.123*1.05+.1)*240*bee);
  Matrix *kamera=new_m_lookat(cam, tgt, v_norm(new_v_c(-.05, 1, .05)));
  static char temprgb[3*1600];
  jytol_kludge=2000.; setatmo(1);

  //vesi=new_mesh_xube(hava, -1000, -3, -1000, 1000, 3, 1000);

  //heijastus (taivas+saari)
  stosd(lfb->buf, 0xd71cd71c, lfb->height*lfb->pitch>>2);
  //stosd(lfb->buf, 0xffffffff, lfb->height*lfb->pitch>>2);
  spc=new_space();
  space_addmesh(spc, pilvitaivas, new_m_create(&v_ci, &v_cmj, &v_ck, &v_c0));
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, puolihava, new_m_create(&v_ci, v_mul(new_v(), &v_cj, .9), &v_ck, &v_c0));
  space_addmesh(spc, puolihava, new_m_create(&v_ci, &v_cmj, &v_ck, &v_c0));
  space_render(spc, lfb, kamera, per, f_antialias);

  if (1) { // plurri
    int x, y;
    #define ip16(x, y, f) ((((x)&0xf81f)*(f)+((y)&0xf81f)*(64-(f))+0x10020>>6&0xf81f)+(((x)&0x07e0)*(f)+((y)&0x07e0)*(64-(f))+0x800>>6&0x07e0))
    for (y=lfb->height>>2; y<lfb->height-1; y++) {
      short *p=(short*)(lfb->buf+y*lfb->pitch), *q=(short*)(lfb->buf+(y+1)*lfb->pitch);
      int bl, bl2;
      bl=y/(float)lfb->height*400-220;
      if (bl<0) bl=0; //else if (bl>128) bl=128;
      bl2=256-bl;
      //for (x=0; x<lfb->width; x++, p++, q++) {
        //*q=ip16(*q, p[0], 16); *p=ip16(0x0008, *p, bl);
      //}
      for (x=3; x<lfb->width; x++) temprgb[x]+=temprgb[x-3]-temprgb[x]+4>>3;
      for (x=lfb->width-4; x>=0; x--) temprgb[x]+=temprgb[x+3]-temprgb[x]+4>>3;
      for (x=0; x<lfb->width; x++, p++, q++) {
        temprgb[x*3  ]+=(*q>>8&0xf8)-temprgb[x*3  ]+2>>2;
        temprgb[x*3+1]+=(*q>>3&0xfc)-temprgb[x*3+1]+2>>2;
        temprgb[x*3+2]+=(*q<<3&0xf8)-temprgb[x*3+2]+2>>2;
        *q=temprgb[x*3]*bl2&0xf800|temprgb[x*3+1]*bl2>>5&0x7e0|temprgb[x*3+2]*bl2>>11&0x1f;
        //temprgb[x*3  ]=temprgb[x*3]
        //*q=ip16(*q, p[0], 16); *p=ip16(0x0008, *p, bl);
      }
    }
  }

  //itse taivas+itse saari
  spc=new_space();
  space_addmesh(spc, pilvitaivas, &m_c_ident);
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, puolihava, &m_c_ident);
  space_render(spc, lfb, kamera, per, f_antialias);
  jytol_kludge=0;
}

void antarktis(float t) {
  jytol_kludge=8000.;
  spc=new_space();
  space_fill(spc, v_mul(new_v(), new_v_c(.3, .4, .5), 1.));
  space_setambient(spc, new_v_c(.04, .03, .07));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(1, .3, 0)), 20000.),
    new_v_c(500000000., 500000000., 500000000.));
  {
    //float t1=t*3.551;
    //Matrix *m1=new_m_create(new_v_c(fcos(t1), 0, fsin(t1)), &v_cj, new_v_c(-fsin(t1), 0, fcos(t1)), &v_c0);
    Plane vt;
    vt.n.x=0; vt.n.y=-1; vt.n.z=0; vt.d=(1-fcos(t))*20;
    space_addmesh(spc, mesh_clip_in(antar, &vt, 1), &m_c_ident);
  }
  space_render(spc, lfb,
    new_m_lookat(
      new_v_c(fsin(t*.17655674)*400, 60, fcos(t*.27655674)*400),
      new_v_c(fsin(t*.43534534)*1, fsin(t*.35645684)*1, fcos(t*.57567534)*1),
      v_norm(new_v_c(-.3, 1, 0))),
    1, 0);
  jytol_kludge=0;
}
Lfb *matriisi(float t) {
  Material *mat;
  Mesh *abu, *obu, *lattia;
  Matrix *kamera;
  Vector cam, tgt;
  Lfb *buf=new_img(320, 180, 2);

  if (t>10) koamk_kludge=1;

  v_create(&cam, fsin(t*.7423)*22, 30+guess((t-1)*1.3)*100-guess((t-1)*10-1)*150, fsin(t*1.2123)*22);
  v_create(&tgt, fsin(t*.7423+.1)*22*.8, 15-guess((t-1)*10-1)*200, fsin(t*1.2123+.1)*22*.8);
  {
    void filltri_matrix(Tri2d *);
    mat_set_filler(mat=new_mat(), filltri_matrix);
  }
  //obu=mesh_wraparound(new_mesh_sphere(mat, 30, 30, 50));
  abu   =new_mesh_xube(mat, -1.1, -1.1, -1.1, 1.1, 1.1, 1.1);
  obu   =new_mesh_xube(mat, -5, -10, -5, 5, 10, 5);
  lattia=new_mesh_xube(mat, -100,  -5, -100, 100,  5, 100);

  {
    float t2=t+5.;
/*    kamera=new_m_lookat(
      cp=new_v_c(fsin(t *1.7655674)*38.5,  7., fcos(t *1.7655674)*38.5),
      op=new_v_c(fsin(t2*1.7655674)*36.5,  6., fcos(t2*1.7655674)*36.5),
      v_norm(new_v_c(-1.3, .2, 1)));*/
    kamera=new_m_lookat(&cam, &tgt,
      v_norm(new_v_c(-1.3, .2, 1)));
  }

  {
    spc=new_space();
    space_fill(spc, v_mul(new_v(), new_v_c(.2, 0., .4), .01));
    //space_addmesh(spc, lattia, &m_c_ident);
    //space_addmesh(spc, lattia, new_m_create(&v_ci, &v_cj, &v_ck, v_mul(new_v(), &v_cj, 50)));
    space_render(spc, buf, kamera, .2, 0);
  }

  spc=new_space();
  //space_fill(spc, v_mul(new_v(), new_v_c(.2, ., .4), .2));
  {
    float t1=t*.02;
    int i, j, k;
    for (i=-2; i<3; i+=2) for (j=-2; j<3; j+=2) {
      Matrix *m1=new_m_create(&v_ci, &v_cj, &v_ck,
        new_v_c(i*15, 15, j*15));
      space_addmesh(spc, obu, m1);
    }
    //if (0) {
      //Matrix *m1=new_m_lookat(op, &v_c0, v_norm(new_v_c(-1.3, .2, 1)));
      //space_addmesh(spc, abu, m1);
    //}
  }
  space_render(spc, buf, kamera, .2, 0);
  return buf;
}


void blaapart(float t) {
  Vector cam, tgt;
  Mesh *abu, *abu2, *obu, *obu2;
  Plane plaat[32];
  Matrix *kamera, *abum, *obum;
  //if (rand()>>14) koamk_kludge=1;

  v_create(&cam, fsin(t*2.7423)*32, fsin(t*2.9234)*32,  fsin(t*2.2123)*32);
  v_create(&tgt, 0, 0, 0);
  //obu=mesh_wraparound(new_mesh_sphere(mat, 30, 30, 50));
  //abu   =new_mesh_zube(blasm1, -10, -2, -10, 10, 2, 10, 1, 1, 1);
  //abu2  =new_mesh_xube(blasm1, -10, -2, -10, 10, 2, 10);
  obu   =new_mesh_zube(blasm2, -5, -5, -5, 5, 5, 5, 1, 1, 1);
  obu2  =new_mesh_xube(blasm2, -5, -5, -5, 5, 5, 5);
  abu   =new_mesh_zube(blasm1, -5, -5, -5, 5, 5, 5, 1, 1, 1);
  abu2  =new_mesh_xube(blasm1, -5, -5, -5, 5, 5, 5);
  kamera=new_m_lookat(&cam, &tgt, &v_cj);
  {
    int plc;
    spc=new_space();
    space_fill(spc, v_mul(new_v(), new_v_c(.2, 0., .4), .01));
    abum=teamatrix(fsin(t*.323143)*3, fsin(t*.464564)*3, fcos(t*.556464)*3);
    obum=teamatrix(fsin(t*.423143)*3, fcos(t*.364564)*3, fsin(t*.256464)*3);
    obum->l.x=2+fsin(t*.24353)*5;


    plc=mesh_to_planes(plaat, obu2, new_m_mmul(abum, new_m_inv(obum)));
    space_addmesh(spc, mesh_recalc(mesh_clip_out(abu, plaat, 6)), abum);

    plc=mesh_to_planes(plaat, abu2, new_m_mmul(obum, new_m_inv(abum)));
    space_addmesh(spc, mesh_recalc(mesh_clip_out(obu, plaat, 6)), obum);

    plc=mesh_to_planes(plaat, abu2, new_m_mmul(obum, new_m_inv(abum)));
    space_addmesh(spc, mesh_set_mat(mesh_flipnormals(mesh_clip_in(obu, plaat, 6)), blasm1), obum);

    space_setambient(spc, new_v_c(.2, .2, .2));
    space_addlight(spc,
      v_mul(new_v(), v_norm(new_v_c(fcos(t*3.23143), fcos(t*4.64564), fsin(t*5.56464))), 200.),
      new_v_c(50000., 50000., 50000.));
    space_addlight(spc,
      v_mul(new_v(), v_norm(new_v_c(fcos(t*5.23143), fcos(t*2.64564), fsin(t*4.56464))), 200.),
      new_v_c(50000., 50000., 50000.));
    space_addlight(spc,
      v_mul(new_v(), v_norm(new_v_c(fcos(t*4.23143), fcos(t*3.64564), fsin(t*6.56464))), 200.),
      new_v_c(50000., 50000., 50000.));
    //space_addlight(spc, abu, teamatrix(fsin(t*3.23143), fsin(t*4.64564), fcos(t*5.56464)));
    space_render(spc, lfb, kamera, 1.2, 0);
  }
}

void blaa2part(float t) {
  spc=new_space();
  space_fill(spc, v_mul(new_v(), new_v_c(.3, .4, .5), 1.));
  space_setambient(spc, new_v_c(.04, .03, .07));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(1, .3, 0)), 20000.),
    new_v_c(500000000., 500000000., 500000000.));
  {
    //float t1=t*3.551;
    //Matrix *m1=new_m_create(new_v_c(fcos(t1), 0, fsin(t1)), &v_cj, new_v_c(-fsin(t1), 0, fcos(t1)), &v_c0);
    Plane vt;
    vt.n.x=0; vt.n.y=-1; vt.n.z=0; vt.d=(1-fcos(t))*20;
    space_addmesh(spc, matto, &m_c_ident);
  }
  space_render(spc, lfb,
    new_m_lookat(
      new_v_c(fsin(t*.17655674)*400, 60, fcos(t*.27655674)*400),
      new_v_c(fsin(t*.43534534)*1, fsin(t*.35645684)*1, fcos(t*.57567534)*1),
      v_norm(new_v_c(-.3, 1, 0))),
    1, 0);
}





void loader(int i) {
  int x;

  lfb=xopenlfb();
  for (x=0; x<lfb->pitch*lfb->height; x++) lfb->buf[x]=0;
  if (i>0) gfxprintf(lfb, fnt, 20, 20, "loading...");
  if (i>1) gfxprintf(lfb, fnt, 20, 40, "music               ok");
  if (i>2) gfxprintf(lfb, fnt, 20, 60, "zoomerdata       ok");
  if (i>3) gfxprintf(lfb, fnt, 20, 80, "maapallo          ok");
  if (i>4) gfxprintf(lfb, fnt, 20, 100, "los angeles    ok");
  if (i>5) gfxprintf(lfb, fnt, 20, 120, "hawaii              ok");
  if (i>6) gfxprintf(lfb, fnt, 20, 140, "antarctis         ok");
  if (i>7) gfxprintf(lfb, fnt, 20, 160, "pilvitaivas     ok");
  xcloselfb(video);
}



__int64 read_tsc();
#pragma aux read_tsc=".586" "rdtsc" value[edx eax] modify exact[edx eax];


int main() {
  float t;
  Modplayer *musa;
  FILE *f;
  Phile *ph;
  Lfb *fonbuf=new_img(320, 180, 2);
  int i;
  __int64 oldtsc, tsc;

  mark();
  fnt=getfont_scifi();
  init_engine();
  setint();

  {
    lobuf8=zgetmem(sizeof(Lfb)); lobuf8->pitch=lobuf8->width=320; lobuf8->height=180; lobuf8->depth=1;
    lobuf16=zgetmem(sizeof(Lfb)); lobuf16->pitch=(lobuf8->width=320)*2; lobuf16->height=180; lobuf16->depth=2;
  }
  lfb=xopenlfb(); xcloselfb(video);
  //precalctaul(320, 180);
  premfxthx();
  //khuva1=img_to16bpp(new_img_read_tga(new_phile_disk_ro("goijus4.tga")));
  //khuva2=img_to16bpp(new_img_read_tga(new_phile_disk_ro("mfx_gi4.tga")));
  //khuva3=img_to16bpp(new_img_read_tga(new_phile_disk_ro("mfx_new.tga")));

  loader(1);

  musa=new_modplayer("shaman6d.it");
  loader(2);

/*
  maa_txt=new_txt_load_tga("maapin51.tga", 2.5);
  maa=mat_set_txt1(new_mat(), maa_txt);
  maapallo=mesh_wraparound(new_mesh_sphere(maa, 20, 20, 50));
  {
    void filltri_noshade(Tri2d *);
    mat_set_filler(maa, filltri_noshade);
  }

  taivas_txt=new_txt_load_tga("maapil51.tga", 1.8);
  taivas=mat_set_txt1(new_mat(), taivas_txt);
  maataivas=mesh_wraparound(new_mesh_sphere(taivas, 25, 25, 50.5));
  {
    void filltri_ouverlei(Tri2d *);
    mat_set_filler(taivas, filltri_ouverlei);
  }
  loader(4);
*/

/*
  hava_txt=new_txt_load_tga("ymap512.tga", 2.);
  hava=mat_set_txt1(new_mat(), hava_txt);
  {
    void filltri_tv(Tri2d *);
    mat_set_filler(hava, filltri_tv);
  }
  if (0) {
    Plane pl; pl.n.x=0; pl.n.y=-1; pl.n.z=0; pl.d=0;
    puolihava=mesh_recalc(mesh_clip_in(new_mesh_maasto(hava, new_img_read_tga(new_phile_disk_ro("ydem555.tga")), 1000, 150, -96), &pl, 1));
    mesh_write(puolihava, new_phile_disk_create("havajji.obu"));
  } else {
    puolihava=new_mesh_read(ph=new_phile_disk_ro("havajji.obu"), hava); phile_close(ph);
  }


  pilvet_txt=new_txt_load_tga("taevas.tga", 1.);
  //pilvet_txt=new_txt_load_mfb("mapconv\\taivas.mfb", 1.);
  pilvet=mat_set_txt1(new_mat(), pilvet_txt);
  pilvitaivas=mesh_wraparound(new_mesh_hemisphere(pilvet, 15, 8, 5000));
  mesh_flipnormals(pilvitaivas);
  {
    void filltri_noshade(Tri2d *);
    mat_set_filler(pilvet, filltri_noshade);
  }
  loader(6);
*/



  la_txt=new_txt_load_tga("mapconv\\xrainmap.tga", 2.);
  //la_txt=new_txt_load_mfb("xbigmap.tga", 2.);
  la=mat_set_txt1(new_mat(), la_txt);
  los=/*mesh_levitaedget*/(new_mesh_read(ph=new_phile_disk_ro("rainier.obu"), la)/*, 2*/); phile_close(ph);
  {
    void filltri_tv(Tri2d *);
    mat_set_filler(la, filltri_tv);
  }
  loader(5);

/*
  anta_txt=new_txt_load_tga("antar512.tga", 2.);
  anta=mat_set_txt1(new_mat(), anta_txt);
  antar=new_mesh_read(ph=new_phile_disk_ro("antar.obu"), anta); phile_close(ph);
  {
    void filltri_tv(Tri2d *);
    mat_set_filler(anta, filltri_tv);
  }


  loader(7);
  loader(8);

  {
    void filltri_ouverlei(Tri2d *);
    Texture *joo=new_txt_load_tga("tblblasm.tga", 2.);
    //blasm1=mat_set_txt1(new_mat(), joo);
    blasm1=mat_set_txt1(new_mat(), joo);
  }

  //matto=new_mesh_matto(matto, blasm1, 20, 20);
*/


  modplayer_play(musa);
  //for (i=0; i<24; i++) modplayer_fwd(musa);
  for (t=0; !kcnt[1]; t=kcnt[31]&1?t+.003:modplayer_getpos(musa)) {
    mark();
    koamk_kludge=0;
    {
      lfb=xopenlfb();
      //lfb_clear(lfb);

/*      if (t<0) ;
      else if (t<6) fadeimg8(lfb, mfxthx(t), 0, 65536);
      else if (t<10) pyoritamaata(t-6);
      else if (t<18) hawaii(t);*/
      losangeles(t);
/*      else if (t<32) { losangeles(t-18); if (t>31 && 0) fadeimg(lfb, vihrea(kewlplur(matriisi(t-31))), 65536-guess((t-32)*64)*65536, guess((t-32)*16)*65536); }
      //else if (t<56) fadeimg(lfb, vihrea(kewlplur(matriisi(t-31))), 0, 65536);
      else if (t<68) antarktis(t-56);*/

      //blaapart(t);

      //if (t>18) fadeimg(lfb, khuva1, 65536+65536*guess((t-18)*6), 16384*guess((t-18)));
           //else fadeimg(lfb, khuva1, 65536+262144*guess((t-18)*32-1), 16384*guess((t-18)));

      //if (t<18) fadeimg(lfb, khuva1, 16300*(3+fsin(t*16)), 32768*(1-fsin(t*4)));
        //else if (t<32) fadeimg(lfb, khuva2, 16300*(3+fsin(t*16)), 32768*(1-fsin(t*4)));
        //else if (t<68) fadeimg(lfb, khuva3, 16300*(3+fsin(t*16)), 32768*(1-fsin(t*4)));

/*      lfb_clear(fonbuf);
      gfxprintf(fonbuf, fnt, 20, 20, "mem use: %ik + %ik",
        getsaveused()>>10, gettmpused()>>10);
      gfxprintf(fonbuf, fnt, 20, 40, "t = %.2f", t);

      oldtsc=tsc; tsc=read_tsc();
      gfxprintf(fonbuf, fnt, 20, 60, "fps = %.2f", 300000000.0/(tsc-oldtsc));
      fadeimg(lfb, fonbuf, 65536, 65536);*/


      //gfxprintf(lfb, fnt, 20, 20, "mem use: %ik + %ik",
        //getsaveused()>>10, gettmpused()>>10);
      //gfxprintf(lfb, fnt, 20, 40, "t = %.2f", t);



      oldtsc=tsc; tsc=read_tsc();
      gfxprintf(lfb, fnt, 20, 60, "fps = %.2f", 400000000.0/(tsc-oldtsc));
      //fadeimg(lfb, fonbuf, 65536, 65536);

      xcloselfb(video);
    }
    if (kcnt[78]) modplayer_fwd(musa), kcnt[78]=0;
    while (kcnt[129]&1) ;
    release();
  }
  modplayer_stop(musa);
  modplayer_close(musa);
  vid_close(video);
  unint();
  return 0;
}



