#include "scan.h"
#include <libya\rawkb.h>

Scan scanbuf[102400], *scanp;
#define swap(a, b, tmp) ((tmp)=(b), (b)=(a), (a)=(tmp))
extern __int64 cnter2;

/*#define parent(i) ((i)-1>>1)
#define left(i) ((i)*2+1)
#define right(i) (left(i)+1)
#define value(i) (sb[(i)]->x0)
#define vaihda(i, j) (swap(sb[(i)], sb[(j)], tmp))
#define fixtop() { \
  int i, j; \
  for (i=0;;) { \
    j=left(i); \
    if (j>=heapsize) break; \
    j+=j+1<heapsize && value(j)>value(j+1); \
    if (value(j)>=value(i)) break; \
    vaihda(i, j); i=j; \
  } \
}
#define fixlast() { \
  int i; \
  for (i=heapsize-1; ; i=parent(i)) { \
    if (i==0 || value(parent(i))<=value(i)) break; \
    vaihda(i, parent(i)); \
  } \
}*/

static int xmin, xmax;
static int tmpb[4096];
void doscan(Scan *sp) {
  int u, v, ud, vd;
  float zk;
  short *t=sp->txt, *b, *bm;
  int x, xm=sp->x1;
  x=sp->x0;
  //vd=sp->vd; v=sp->v0+x*vd; vd<<=11; v<<=11;
  //ud=sp->ud; u=sp->u0+x*ud; ud=(ud<<4)-vd; u=(u<<4)-v;
  vd=sp->vd; v=sp->v0;
  ud=sp->ud; u=sp->u0;
  vd<<=8; v<<=8;
  ud=(ud<<1)-vd; u=(u<<1)-v;
  u<<=7; ud<<=7;
  //xm=x+1;
  //x++;
  if (x<xmin) x=xmin; if (xm>xmax) xm=xmax;
  if (x<xm) _asm {
    mov ebx, t
    mov esi, u
    mov edi, v
    mov ecx, x
    l1:
      mov eax, esi
      add esi, ud
      shr eax, 7
      add eax, edi
      add edi, vd
      shr eax, 18
      mov ax, [eax*2+ebx]
      mov [tmpb+ecx*4], eax
      add ecx, 1
      cmp ecx, xm
    jb l1
    test byte[kcnt+2],1
    jz l2
      xor eax, eax
      mov [tmpb+ecx*4-4], eax
    l2:
  };
  //for (; x<xm; x+=2) {
    //tmpb[x  ]=(au+2)*64|0x00&t[(u&0x003fffff)+v>>12]; u+=ud; v+=vd;
    //tmpb[x+1]=(au+2)*64|0x00&t[(u&0x003fffff)+v>>12]; u+=ud; v+=vd;
    //tmpb[x  ]=t[(u&0x01ffffff)+v>>18]; u+=ud; v+=vd;
    //tmpb[x+1]=t[(u&0x01ffffff)+v>>18]; u+=ud; v+=vd;
  //}
  //for (x++; x<xm; x++) tmpb[x]=(int)sp->txt;
}

void clearscans(Lfb *lfb) { scanp=scanbuf; }
void (*postfx)(int *buf, int x, int y, int w)=0;
#define stripew 128
void rasterify(Lfb *lfb) {
  int x, y, yy, i, str;
  Scan *sp, *tmp;
  float z;
  static int lkms[8100];
  static Scan **linep[8100];
  Scan **linebuf;
  static volatile int zero=0;
  memset(lkms, 0, lfb->height*4*5);
  cnter2-=rdtsc();
  for (sp=scanbuf; sp<scanp; sp++) {
    int b1=(unsigned)sp->x0/stripew;
    int b2=((unsigned)sp->x1-1)/stripew;
    float u1, v1;
    if (b1<0) b1=0; if (b2<0) b2=0;
    if (b1>4) b1=4; if (b2>4) b2=4;
    if (b2>b1) {
      *scanp=*sp;
      scanp->x0=(b1+1)*stripew;
      scanp++;
    }
    sp->y+=b1*lfb->height;
    lkms[sp->y]++;
    if (sp->x0<b1*stripew) sp->x0=b1*stripew;
    if (sp->x1>(b1+1)*stripew) sp->x1=(b1+1)*stripew;
    z=1./(sp->z0+sp->x1*sp->zd);
    u1=(sp->u0+sp->x1*sp->ud)*z;
    v1=(sp->v0+sp->x1*sp->vd)*z;
    z=sp->z0+sp->x0*sp->zd; sp->z0=z; z=1./z;
    sp->u0=(sp->u0+sp->x0*sp->ud)*z;
    sp->v0=(sp->v0+sp->x0*sp->vd)*z;
    z=1./(sp->x1-sp->x0);
    sp->ud=(u1-sp->u0)*z;
    sp->vd=(v1-sp->v0)*z;
  }
  linebuf=mem(4*(scanp-scanbuf));
  linep[0]=linebuf; for (y=1; y<lfb->height*5; y++) linep[y]=linep[y-1]+lkms[y-1];
  for (sp=scanbuf; sp<scanp; sp++) {
    *linep[sp->y]++=sp;
    //if (isnan(sp->z0)) err("z0 nan");
    //if (isnan(sp->zd)) err("zd nan");
    //if (isnan(sp->u0)) err("u0 nan");
    //if (isnan(sp->ud)) err("ud nan");
    //if (isnan(sp->v0)) err("v0 nan");
    //if (isnan(sp->vd)) err("vd nan");
  }
  for (y=0; y<lfb->height*5; y++) linep[y]-=lkms[y];
  cnter2+=rdtsc();
  for (y=0; y<lfb->height*5; y++) {
    int flips;
    do {
      flips=0;
      for (i=0; i+1<lkms[y]; i++) if (linep[y][i]->z0>linep[y][i+1]->z0) {
        swap(linep[y][i], linep[y][i+1], tmp);
        flips++;
      }
    } while (flips);
  }
  yy=0;
  for (str=0; str+stripew<=lfb->width; str+=stripew) {
    xmin=0; xmax=lfb->width;
    for (y=0; y<lfb->height; y++, yy++) {
      memset(tmpb+str, 0, stripew*4);
      for (i=0; i<lkms[yy]; i++) doscan(linep[yy][i]);
      if (postfx) postfx(tmpb+str, str, y, stripew);
      {
        int *d=(int*)(lfb->buf+y*lfb->pitch+str*2), *s=tmpb+str, *sm=s+stripew;
        for (; s<sm; d+=2, s+=4) {
          d[0]=s[0]&0xffff|s[1]<<16;
          d[1]=s[2]&0xffff|s[3]<<16;
        }
      }
      //memcpy(lfb->buf+y*lfb->pitch+str*2, tmpb+str, stripew*2);
    }
  }


}


