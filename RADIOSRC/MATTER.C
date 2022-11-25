#include "core.h"
#include <stdlib.h>

#include "matter.h"
//void aadrawline(Tri2d*);
//Material linemat={ aadrawline };
//Material portalmat={ portaltri };

float monitor_gamma;

int hajotbl[4096];


Texture  *new_txt() { Texture *x=newobj(Texture); return x; }


Texture *new_txt_shrink(Texture *s) {
  int sz, i, j, x, y, xx, yy, xs, xm, ym, r, g, b;
  short *dp, *sp;
  int *du, *dv, *su, *sv, ds, ss;
  Texture *d;
  if (s->size<2) return 0;
  *(d=new_txt())=*s;
  d->size=s->size-1; //d->raja=3*16777216>>d->size;
  sz=1<<2*d->size;
  d->bits16l=mem(sz+sz); dp=d->bits16l, sp=s->bits16l;
  ds=32-2*d->size; ss=32-2*s->size;
  #define dst(x, y) dp[(hajotbl[(x)&4095]|hajotbl[(y)&4095]<<1)&(1<<2*d->size)-1]
  #define src(x, y) sp[(hajotbl[(x)&4095]|hajotbl[(y)&4095]<<1)&(1<<2*s->size)-1]
  for (j=0; j<1<<2*d->size; j++) {
    x=j>>1&0x55555555; x=(x>>1&0x22222222)+(x&0x11111111); x=(x>>2&0x0c0c0c0c)+(x&0x03030303); x=(x>>4&0x00f000f0)+(x&0x000f000f); x=(x>>8&0x0000ff00)+(x&0x000000ff);
    y=j   &0x55555555; y=(y>>1&0x22222222)+(y&0x11111111); y=(y>>2&0x0c0c0c0c)+(y&0x03030303); y=(y>>4&0x00f000f0)+(y&0x000f000f); y=(y>>8&0x0000ff00)+(y&0x000000ff);
    r=g=b=0; yy=y*2; xx=x*2;
    i=src(xx  , yy  ); r+=(i>>11&31)*2; g+=(i>>5&63)*2; b+=(i&31)*2;
    i=src(xx+1, yy  ); r+=(i>>11&31)*2; g+=(i>>5&63)*2; b+=(i&31)*2;
    i=src(xx  , yy+1); r+=(i>>11&31)*2; g+=(i>>5&63)*2; b+=(i&31)*2;
    i=src(xx+1, yy+1); r+=(i>>11&31)*2; g+=(i>>5&63)*2; b+=(i&31)*2;
    i=src(xx-1, yy  ); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx-1, yy+1); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx+2, yy  ); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx+2, yy+1); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx  , yy-1); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx+1, yy-1); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx  , yy+2); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    i=src(xx+1, yy+2); r+=i>>11&31; g+=i>>5&63; b+=i&31;
    dst(x, y)=(r+11>>4<<11)+(g+11>>4<<5)+(b+11>>4);
  }
  #undef src
  #undef dst
  d->mipnext=new_txt_shrink(d);
  return d;
}
/*
Texture *txt_load(Texture *t, void *data, int xs, int ys, int sb,
                  int ds, int db, float gamma) {
  static int unlog[1024];
  int x, y, x2, y2, c, r, g, b, i, j;
  char *cdata=data, *p1, *p2, *p3, *p4;
  if (ds==0) ds=10;
  if (db!=16 || sb!=24) return 0;
  if (!hajotbl[1]) {
    for (i=0, j=0; i<4096; i++) j&=0x555555, hajotbl[i]=j, j+=0xaaaab;
  }
  for (x=1; x<1024; x++) {
    //float f=256.0+flog2(x/1024.0)*(256.0/texture_contrast)*gamma;
    //if (f<0.0) f=0.0; else if (f>255.0) f=255.0;
    float f=fpow(x/1024.0, gamma/monitor_gamma)*255.4;
    if (f<0.0) f=0.0; else if (f>255.0) f=255.0;
    unlog[x]=f;
  }
  t->size=ds; //t->raja=3*16777216>>t->size;
  t->bits16l=mem(2<<ds+ds);
  if (!t->bits16l) return t;
  #define dst(x, y) t->bits16l[hajotbl[(x)]|hajotbl[(y)]<<1]
  if (xs+xs-1==(xs^xs-1) && ys+ys-1==(ys^ys-1)) for (y=0; y<1<<ds; y++) for (x=0; x<1<<ds; x++) {
    p1=cdata+(((y*ys>>ds)  )%ys*xs+((x*xs>>ds)  )%xs)*3;
    x2=x*xs<<8>>ds&255; y2=y*ys<<8>>ds&255;
    dst(x, y)=(unlog[p1[0]<<2]>>3<<11)+(unlog[p1[1]<<2]>>2<<5)+(unlog[p1[2]<<2]>>3);
    //err("sgdfgsd");
  } else for (y=0; y<1<<ds; y++) for (x=0; x<1<<ds; x++) {
    p1=cdata+(((y*ys>>ds)  )%ys*xs+((x*xs>>ds)  )%xs)*3;
    p2=cdata+(((y*ys>>ds)  )%ys*xs+((x*xs>>ds)+1)%xs)*3;
    p3=cdata+(((y*ys>>ds)+1)%ys*xs+((x*xs>>ds)  )%xs)*3;
    p4=cdata+(((y*ys>>ds)+1)%ys*xs+((x*xs>>ds)+1)%xs)*3;
    x2=x*xs<<8>>ds&255; y2=y*ys<<8>>ds&255;
    r=unlog[(p1[0]*(256-x2)+p2[0]*x2)*(256-y2)+(p3[0]*(256-x2)+p4[0]*x2)*y2+8192>>14];//+(rand()>>12);
    g=unlog[(p1[1]*(256-x2)+p2[1]*x2)*(256-y2)+(p3[1]*(256-x2)+p4[1]*x2)*y2+8192>>14];//+(rand()>>13);
    b=unlog[(p1[2]*(256-x2)+p2[2]*x2)*(256-y2)+(p3[2]*(256-x2)+p4[2]*x2)*y2+8192>>14];//+(rand()>>12);
    //if (r>255) r=255; if (g>255) g=255; if (b>255) b=255;
    dst(x, y)=(r>>3<<11)+(g>>2<<5)+(b>>3);
  }
  #undef dst
  t->mipnext=_new_txt_shrink(t);
  return t;
}
Texture *new_txt_load_tga(char *fname, float gamma) {
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
  char *tmp;
  Texture *t;
  FILE *fp=fopen(fname, "rb");
  fread(&hdr, 1, 18, fp);
  fseek(fp, hdr.idlen, SEEK_CUR);
  pitch=(hdr.bpp+7>>3)*hdr.width;
  //pitch=3*hdr.width;
  size=pitch*hdr.height; tmp=malloc(size); if (!tmp) { err("memory!"); }
  fread(tmp, 1, (hdr.bpp+7>>3)*hdr.width, fp);
  fclose(fp);
  if (hdr.bpp==24) {
    for (i=0; i<size; i+=3) j=tmp[i], tmp[i]=tmp[i+2], tmp[i+2]=j;
  } else if (hdr.bpp==16) {
    //for (i=hdr.height*hdr.width-1; i>=0; i--) {
      //j=((short*)tmp)[i];
      //tmp[i*3]=j>>8&0xf8, tmp[i*3+1]=j>>3&0xf8, tmp[i*3+2]=j<<3&0xf8;
    //}
    for (i=0; i+i<size; i++) j=((short*)tmp)[i], ((short*)tmp)[i]=(j<<11&0xf800)+(j&0x07e0)+(j>>11&0x001f);
  }
  if (hdr.flags&0x20) {
    for (y=0; y<hdr.height; y++) for (x=0; x<pitch; x++)
      j=tmp[y*pitch+x], tmp[y*pitch+x]=tmp[(hdr.height-1-y)*pitch+x], tmp[(hdr.height-1-y)*pitch+x]=j;
  }
  t=txt_load(new_txt(), tmp, hdr.width, hdr.height, hdr.bpp, 10, 16, gamma);
  free(tmp); return t;
}
*/

Texture *txt_load(Texture *t, void *data, int xs, int ys, int sb,
                  int ds, int db, float gamma) {
  static int unlog[1024];
  int x, y, x2, y2, c, r, g, b, i, j;
  char *cdata=data, *p1, *p2, *p3, *p4;
  if (ds==0) ds=10;
  if (db!=16 || sb!=24) return 0;
  if (!hajotbl[1]) {
    for (i=0, j=0; i<4096; i++) j&=0x555555, hajotbl[i]=j, j+=0xaaaab;
  }
  for (x=1; x<1024; x++) {
    //float f=256.0+flog2(x/1024.0)*(256.0/texture_contrast)*gamma;
    //if (f<0.0) f=0.0; else if (f>255.0) f=255.0;
    float f=fpow(x/1024.0, gamma/monitor_gamma)*255.4;
    if (f<0.0) f=0.0; else if (f>255.0) f=255.0;
    unlog[x]=f;
  }
  t->size=ds; //t->raja=3*16777216>>t->size;
  t->bits16l=mem(2<<ds+ds);
  //if (!t->bits16l) return t;
  #define dst(x, y) t->bits16l[hajotbl[(x)]|hajotbl[(y)]<<1]
/*  if (xs+xs-1==(xs^xs-1) && ys+ys-1==(ys^ys-1)) for (y=0; y<1<<ds; y++) for (x=0; x<1<<ds; x++) {
    p1=cdata+(((y*ys>>ds)  )%ys*xs+((x*xs>>ds)  )%xs)*3;
    x2=x*xs<<8>>ds&255; y2=y*ys<<8>>ds&255;
    dst(x, y)=(unlog[p1[0]<<2]>>3<<11)+(unlog[p1[1]<<2]>>2<<5)+(unlog[p1[2]<<2]>>3);
    //err("sgdfgsd");
  } else */for (y=0; y<1<<ds; y++) for (x=0; x<1<<ds; x++) {
    p1=cdata+(((y*ys>>ds)  )%ys*xs+((x*xs>>ds)  )%xs)*3;
    p2=cdata+(((y*ys>>ds)  )%ys*xs+((x*xs>>ds)+1)%xs)*3;
    p3=cdata+(((y*ys>>ds)+1)%ys*xs+((x*xs>>ds)  )%xs)*3;
    p4=cdata+(((y*ys>>ds)+1)%ys*xs+((x*xs>>ds)+1)%xs)*3;
    //x2=x*xs<<8>>ds&255; y2=y*ys<<8>>ds&255;
    x2=x*xs*256>>ds&255; y2=y*ys*256>>ds&255;
    r=unlog[(p1[0]*(256-x2)+p2[0]*x2)*(256-y2)+(p3[0]*(256-x2)+p4[0]*x2)*y2+8192>>14];//+(rand()>>12);
    g=unlog[(p1[1]*(256-x2)+p2[1]*x2)*(256-y2)+(p3[1]*(256-x2)+p4[1]*x2)*y2+8192>>14];//+(rand()>>13);
    b=unlog[(p1[2]*(256-x2)+p2[2]*x2)*(256-y2)+(p3[2]*(256-x2)+p4[2]*x2)*y2+8192>>14];//+(rand()>>12);
/*    if (x&1||y&1) */
    //if (r>255) r=255; if (g>255) g=255; if (b>255) b=255;
    //dst(x, y)=(r>>3<<11)+(g>>2<<5)+(b>>3);
    t->bits16l[y*(1<<ds)+x]=(r>>3<<11)+(g>>2<<5)+(b>>3);
  }
  #undef dst
  //t->mipnext=_new_txt_shrink(t);
  return t;
}
Texture *new_txt_load_tga(char *fname, float gamma) {
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
  char *tmp;
  Ctx *cx=new_ctx();
  Texture *t;
  FILE *fp=fopen(fname, "rb");
  fread(&hdr, 1, 18, fp);
  fseek(fp, hdr.idlen, SEEK_CUR);
  pitch=(hdr.bpp+7>>3)*hdr.width;
  size=pitch*hdr.height;
  tmp=ctx_mem(cx, size);
  fread(tmp, 1, size, fp);
  fclose(fp);
  if (hdr.bpp==24) {
    for (i=0; i<size; i+=3) j=tmp[i], tmp[i]=tmp[i+2], tmp[i+2]=j;
  } else if (hdr.bpp==16) {
    for (i=0; i+i<size; i++) j=((short*)tmp)[i], ((short*)tmp)[i]=(j<<11&0xf800)+(j&0x07e0)+(j>>11&0x001f);
  }
  if (~hdr.flags&0x20) {
    for (y=0; y*2<hdr.height; y++) for (x=0; x<pitch; x++)
      j=tmp[y*pitch+x], tmp[y*pitch+x]=tmp[(hdr.height-1-y)*pitch+x], tmp[(hdr.height-1-y)*pitch+x]=j;
  }
  t=txt_load(new_txt(), tmp, hdr.width, hdr.height, hdr.bpp, 7, 16, gamma);
  ctx_free(cx); return t;
}







Material *new_mat() { Material *x=newobj(Material); x->refl=0; /*x->fill=filltri;*/ return x; }


static void mat_stuph(Material *m) { }
Material *mat_set_txt1(Material *m, Texture *t) { m->txt1=*t; mat_stuph(m); return m; }
//Material *mat_something(Material *m) { m->fill=filltri_cthru; return m; }
Material *mat_set_space(Material *m, Space *s) { m->space=s; return m; }
Material *mat_set_refl(Material *m) { m->refl=1; return m; }
Material *mat_set_filler(Material *m, void *filler) { m->fill=filler; return m; }






/*

static char obuf[376840];
static int obit=0;

int getbit() {
  int i=obuf[obit>>3]>>(obit&7)&1; obit++; return i;
}
int getbits(int cnt) {
  //int i=0; while (cnt-->0) putbit(i&1), i>>=1;
  int i=0, k=0; while (cnt-->0) i+=getbit()<<k, k++; return i;
}



void glotx(char *v, char *a, char *b) {
  int a=getbits(2), b, c;
  float f, ag, bg;
  if (a==0) a=11, b=-3;
    else if (a==1) a=5, b=3;
    else if (a==2) a=3, b=5;
    else a=-3, b=11;
  ag=_a[0]*0.30+_a[1]*0.59+_a[2]*0.11+0.001;
  bg=_b[0]*0.30+_b[1]*0.59+_b[2]*0.11+0.001;
  c=(_a[0]/ag+_b[0]/bg)*(ag*a+bg*b)*0.0625; _v[0]=c>255?255:c<0?0:c;
  c=(_a[1]/ag+_b[1]/bg)*(ag*a+bg*b)*0.0625; _v[1]=c>255?255:c<0?0:c;
  c=(_a[2]/ag+_b[2]/bg)*(ag*a+bg*b)*0.0625; _v[2]=c>255?255:c<0?0:c;
}

void xfine(char *dat, int xstep, int ystep) {
  int x, y;
  for (y=0; y<1024; y+=ystep) for (x=0; x<1024; x+=xstep+xstep)
    glotx(dat+(y*1024+x+xstep)*3, dat+(y*1024+x)*3, dat+(y*1024+(x+2*xstep&1023))*3);
}
void yfine(char *dat, int xstep, int ystep) {
  int x, y;
  for (y=0; y<1024; y+=ystep+ystep) for (x=0; x<1024; x+=xstep)
    glotx(dat+((y+ystep)*1024+x)*3, dat+(y*1024+x)*3, dat+((y+2*ystep&1023)*1024+x)*3);
}

Texture *new_txt_load_mfb(char *fname, float gamma) {
  int size, x, y, i, j, k, pitch;
  char *tmp;
  Texture *t;
  FILE *fp=fopen(fname, "rb"); fread(obuf, 1, 376840, fp); fclose(fp);
  obit=0;
  getbits(32); getbits(32);
  tmp=malloc(3*1024*1024);
  for (y=0; y<256; y++) for (x=0; x<256; x++) {
    i=getbits(16);
    tmp[(y*1024+x)*12+2]=i>>8&0xf8;
    tmp[(y*1024+x)*12+1]=i>>3&0xfc;
    tmp[(y*1024+x)*12  ]=i<<3&0xf8;
    //getbits(8);
  }
  //yfine(tmp, 8, 4); xfine(tmp, 4, 4);
  yfine(tmp, 4, 2); xfine(tmp, 2, 2);
  yfine(tmp, 2, 1); xfine(tmp, 1, 1);
  t=txt_load(new_txt(), tmp, 1024, 1024, 24, 10, 16, gamma);
  free(tmp); return t;
}


*/



