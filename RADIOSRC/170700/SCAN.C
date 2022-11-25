#include "scan.h"

Scan scanbuf[102400], *scanp;
#define swap(a, b, tmp) ((tmp)=(b), (b)=(a), (a)=(tmp))

#define parent(i) ((i)-1>>1)
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
}


void clearscans(Lfb *lfb) { scanp=scanbuf; }
void rasterify(Lfb *lfb) {
  int y, col, lkm, i, j, au, a, b, yy;
  static short tmpb[4096];
  Scan *sp, *tmp;
  static Scan **sb, *sb2[1024];
  static int lkms[2048];
  static Scan **linep[2048];
  Scan **linebuf;
  int x, heapsize, lkm2;
  stosd(lkms, 0, lfb->height);
  for (sp=scanbuf; sp<scanp; sp++) lkms[sp->y]++;
  linebuf=mem(4*(scanp-scanbuf));
  linep[0]=linebuf; for (y=1; y<lfb->height; y++) linep[y]=linep[y-1]+lkms[y-1];
  for (sp=scanbuf; sp<scanp; sp++) *linep[sp->y]++=sp;
  for (y=0; y<lfb->height; y++) linep[y]-=lkms[y];
  for (yy=0; yy<lfb->height; yy++) {
    y=yy; while (y>=lfb->height) y-=lfb->height;
    stosd(tmpb, 0, lfb->width>>1);
    lkm=lkms[y]; sb=linep[y];
    for (heapsize=0; heapsize<lkm; ) { heapsize++; fixlast(); }
    lkm2=0;
    au=0;
    if (heapsize) {
      sp=sb[0]; sb[0]=sb[--heapsize];
      if (heapsize) for (;;) {
        fixtop();
        if (sb[0]->x0<sp->x1) {
          if (sb[0]->z0<sp->z0) {
            sb[0]->x0=sp->x1;
            if (sb[0]->x1<=sb[0]->x0) { sb[0]=sb[--heapsize]; if (!heapsize) break; }
          } else {
            if (sp->x1>sb[0]->x1) {
              *scanp=*sp; scanp->x0=sb[0]->x1; /*scanp->x0d=sb[0]->x1d;*/ sb[heapsize++]=scanp++; fixlast();
              au++;
            }
            sp->x1=sb[0]->x0;
            if (sp->x1>sp->x0) sb2[lkm2++]=sp;
            sp=sb[0]; sb[0]=sb[--heapsize]; if (!heapsize) break;
          }
        } else {
          sb2[lkm2++]=sp; sp=sb[0]; sb[0]=sb[--heapsize]; if (!heapsize) break;
        }
      }
      sb2[lkm2++]=sp;
    }
    for (i=j=0; i<lkm2; i++) if (sb2[i]->x0>>12<sb2[i]->x1>>12)
      sb2[j++]=sb2[i];
    lkm2=j;
    for (i=0; i<lkm2; i++) if (sp=sb2[i], 1) {
      unsigned u, v, ud, vd;
      short *t=sp->txt, *b, *bm;
      int xm=sp->x1>>12;
      x=sp->x0>>12;
      //vd=sp->vd; v=sp->v0+x*vd; vd<<=11; v<<=11;
      //ud=sp->ud; u=sp->u0+x*ud; ud=(ud<<4)-vd; u=(u<<4)-v;
      vd=sp->vd; v=sp->v0+(x+1)*vd; vd<<=8; v<<=8;
      ud=sp->ud; u=sp->u0+(x+1)*ud; ud=(ud>>2)-vd; u=(u>>2)-v;
      u<<=10; ud<<=10;
      //xm=x+1;
      //x++;
      if (x<0) x=0; if (xm>1023) xm=1023;
      if (x<xm) _asm {
        mov ebx, t
        mov esi, u
        mov edi, v
        mov ecx, x
        l1:
          mov eax, esi
          add esi, ud
          shr eax, 10
          add eax, edi
          add edi, vd
          shr eax, 12
          mov ax, [eax*2+ebx]
          mov [tmpb+ecx*2], ax
          add ecx, 1
          cmp ecx, xm
        jb l1
      };
      //for (; x<xm; x+=2) {
        //tmpb[x  ]=(au+2)*64|0x00&t[(u&0x003fffff)+v>>12]; u+=ud; v+=vd;
        //tmpb[x+1]=(au+2)*64|0x00&t[(u&0x003fffff)+v>>12]; u+=ud; v+=vd;
        //tmpb[x  ]=t[(u&0x01ffffff)+v>>18]; u+=ud; v+=vd;
        //tmpb[x+1]=t[(u&0x01ffffff)+v>>18]; u+=ud; v+=vd;
      //}
      //for (x++; x<xm; x++) tmpb[x]=(int)sp->txt;
    }
    for (i=0; i<lkm2-1; i++) {
      if (sb2[i]->txt!=sb2[i+1]->txt) {
        //int k1=sb2[i+1]->x0d;
        //if (k1<0) k1=-k1;
        //k1+=5000;
        //if (k1>24000) k1=24000;
        //k1=k1*7>>3;
        int k1=7000;
        a=tmpb[(sb2[i]->x1>>12)-1]; b=tmpb[sb2[i]->x1>>12];
        for (x=sb2[i]->x1-k1>>12; x<sb2[i]->x1>>12; x++) {
          j=(sb2[i]->x1+k1-x*4096-4096)*16/k1;
          if (x>=0 && x<1024) tmpb[x]=(tmpb[x]&0xf81f)*j+(b&0xf81f)*(32-j)>>5&0xf81f
                                     |(tmpb[x]&0x07e0)*j+(b&0x07e0)*(32-j)>>5&0x07e0;
        }
        for (; x<sb2[i]->x1+k1>>12; x++) {
          j=(sb2[i]->x1+k1-x*4096-4096)*16/k1;
          if (x>=0 && x<1024) tmpb[x]=(a&0xf81f)*j+(tmpb[x]&0xf81f)*(32-j)>>5&0xf81f
                                     |(a&0x07e0)*j+(tmpb[x]&0x07e0)*(32-j)>>5&0x07e0;
        }
      }
    }
    movsd(lfb->buf+y*lfb->pitch, tmpb, lfb->width>>1);
  }
}


