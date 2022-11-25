#include "functs.h"
#include "core.h"
#include "matter.pri"
#include "space.pri"
#include "mesh.h"

static void *dbuf;
static Lfb lfb;
static Space *curf;
static int pitch, pitchp2, pitchp4;
static int raja;
float texel_size=0.0;

int rand();

extern int hajotbl[4096];
static int //exppal[65536+256], epalr[65536+256], epalg[65536+256], epalb[65536+256],
  *fpal, epal[256*64], atmo1[256*64], atmo2[256*64];

void setatmo(int a) { fpal=a?atmo2:atmo1; }
/*#define epal2 (exppal+32768)
#define epal2r (epalr+32768)
#define epal2g (epalg+32768)
#define epal2b (epalb+32768)*/
int redpaskatbl[64], greenpaskatbl[128], bluepaskatbl[64];
void triprep(Space *f) {
  int i, j;
  curf=f; lfb=f->dbuf; dbuf=(short*)lfb.buf;
  pitchp4=(pitchp2=(pitch=f->dbuf.pitch)>>1)>>1;
  if (texel_size==0.0) {
    raja=5*16777216.0/fsqrt(lfb.width*(1/512.0));
    if (raja>8*16777216) raja=8*16777216;
  } else raja=5*16777216.0/texel_size;
  if (!greenpaskatbl[127]) {
    for (i=0; i<256*64; i++) {
      int r, g, b;
      r=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001)+(1-(i>>6)*(1/63.0))*0.3, 1.0/monitor_gamma);
      g=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001)+(1-(i>>6)*(1/63.0))*0.4, 1.0/monitor_gamma);
      b=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001)+(1-(i>>6)*(1/63.0))*0.5, 1.0/monitor_gamma);
      if (r>1023) r=1023; if (g>1023) g=1023; if (b>1023) b=1023;
      atmo1[i]=r>>5<<11|g>>4<<5|b>>5;
      r=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001)+(1-(i>>6)*(1/63.0))*0.5, 1.0/monitor_gamma);
      g=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001)+(1-(i>>6)*(1/63.0))*0.6, 1.0/monitor_gamma);
      b=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001)+(1-(i>>6)*(1/63.0))*0.6, 1.0/monitor_gamma);
      if (r>1023) r=1023; if (g>1023) g=1023; if (b>1023) b=1023;
      atmo2[i]=r>>5<<11|g>>4<<5|b>>5;
      r=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001), 1.0/monitor_gamma);
      g=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001), 1.0/monitor_gamma);
      b=1023*fpow(fpow((i&63)*(0.9998/63.1)+0.0001, monitor_gamma)*((i>>6)*(0.9998/63.0)+0.0001), 1.0/monitor_gamma);
      if (r>1023) r=1023; if (g>1023) g=1023; if (b>1023) b=1023;
      epal[i]=r>>5<<11|g>>4<<5|b>>5;
    }
    for (i=0; i<32; i++) redpaskatbl[i]=i<<11, bluepaskatbl[i]=i;
    for (i=0; i<64; i++) greenpaskatbl[i]=i<<5;
    for (i=32; i<64; i++) redpaskatbl[i]=31<<11, bluepaskatbl[i]=31;
    for (i=64; i<128; i++) greenpaskatbl[i]=63<<5;
    fpal=atmo1;
  }
}

//#define sumeioverflov(a, b) (redpaskatbl[((a)>>11&31)+((b)>>11&31)]+greenpaskatbl[((a)>>5&63)+((b)>>5&63)]+bluepaskatbl[((a)&31)+((b)&31)])
#define sumeioverflov(a, b) (((a)&0xf01f)+((b)&0xf01f)>>1|((a)&0x7c0)+((b)&0x7c0)>>1)

void filllst(Tri2d *x) { for (; x; x=x->next) x->m->fill(x); }

/*
void filltri(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  //static Tiledata *td;
  static int uamask, vamask, uomask, vomask, sh1, sh2, sh3, zsh, fbi;
  static struct {
    union { struct { unsigned xf; union { short *p; int x; }; }; __int64 x64; };
    union { struct { unsigned af; int a; }; __int64 a64; };
    int a2;
  } b, e, *e1, *e2;
  static struct {
    union { int v; unsigned w; };
    union { int v2; unsigned w2; };
    int a, b, xi, ei;
  } u, v, cr, cg, cb;
  //3256 3188 3148 3120 3104
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=(short*)dbuf+(top->y>>8)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>8)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>8)*pitchp2;

  //if ((int)t&60) return;
  ala=256.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  cr.a=inear(((mid->r-top->r)*bty-(bot->r-top->r)*mty)*ala);
  cr.b=inear(((bot->r-top->r)*mtx-(mid->r-top->r)*btx)*ala);
  cg.a=inear(((mid->g-top->g)*bty-(bot->g-top->g)*mty)*ala);
  cg.b=inear(((bot->g-top->g)*mtx-(mid->g-top->g)*btx)*ala);
  cb.a=inear(((mid->b-top->b)*bty-(bot->b-top->b)*mty)*ala);
  cb.b=inear(((bot->b-top->b)*mtx-(mid->b-top->b)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31);
  c=(v.a^v.a>>31)+(v.b^v.b>>31); if (c<tmp) tmp=c; tmp<<=1;
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>(raja>>txt->size)) txt=txt->mipnext;
  c=top->u^mid->u|mid->u^bot->u|top->v^mid->v|mid->v^bot->v|31;
  zsh=0; bits=(unsigned short*)txt->bits16l;
  sh1=32-2*txt->size; sh2=24-txt->size; sh3=1+txt->size;
  //while (c<0x800000 && sh1<28 && sh3>3) {
    //bits+=(unsigned)(top->v<<8+zsh&0x80000000|top->u<<7+zsh&0x40000000)>>sh1;
    //c<<=2; zsh++; sh1+=2; sh3-=2;
  //}
  uomask=vamask=0xaaaaaaaa<<sh1;
  uamask=vomask=0x55555555<<sh1;
  fbi=0x7fffffff>>32-sh1;
  uamask|=fbi  ; vamask|=fbi  ;
  uomask|=fbi+1; vomask|=fbi+1;

  if (sh3<8) u.a<<=8-sh3, v.a<<=8-sh3, u.b<<=8-sh3, v.b<<=8-sh3;
    else u.a>>=sh3-8, v.a>>=sh3-8, u.b>>=sh3-8, v.b>>=sh3-8;
  sh2+=8-sh3;
  u.xi=(hajotbl[u.a>>sh2&0xfff]<<sh1  )+(u.a&fbi)|uomask;
  v.xi=(hajotbl[v.a>>sh2&0xfff]<<sh1+1)+(v.a&fbi)|vomask;
  cr.xi=cr.a; cg.xi=cg.a; cb.xi=cb.a;

  yfix=256-(top->y&0xff);
  fistq(&e1->x64, (top->x+yfix*f1)*16777216.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*16777216.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*256-top->x;
  u.v2=(xfix*u.a+yfix*u.b+128>>8)+(top->u<<8>>sh3); u.ei=u.a*b.a+u.b;
  v.v2=(xfix*v.a+yfix*v.b+128>>8)+(top->v<<8>>sh3); v.ei=v.a*b.a+v.b;
  cr.v2=(xfix*(cr.a>>4)+yfix*(cr.b>>4)>>4)+top->r; cr.ei=cr.a*b.a+cr.b;
  cg.v2=(xfix*(cg.a>>4)+yfix*(cg.b>>4)>>4)+top->g; cg.ei=cg.a*b.a+cg.b;
  cb.v2=(xfix*(cb.a>>4)+yfix*(cb.b>>4)>>4)+top->b; cb.ei=cb.a*b.a+cb.b;
  u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
  u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
  v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
  v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>8)-(mid->y>>8);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; cr.v=cr.v2; cg.v=cg.v2; cb.v=cb.v2;
    x=b.p-e.p;
    if (x<0 && (b.x&2)) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2r[((c&0xf800)>>8)+(cr.v>>16)]+
              epal2g[((c&0x07e0)>>3)+(cg.v>>16)]+
              epal2b[((c&0x001f)<<3)+(cb.v>>16)])>>16;
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      x++;
    }
    while (x<-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      d=epal2r[((c&0xf800)>>8)+(cr.v>>16)]+
        epal2g[((c&0x07e0)>>3)+(cg.v>>16)]+
        epal2b[((c&0x001f)<<3)+(cb.v>>16)]&0xffff;
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      *(int*)(e.p+x)=d|epal2r[((c&0xf800)>>8)+(cr.v>>16)]
                      +epal2g[((c&0x07e0)>>3)+(cg.v>>16)]
                      +epal2b[((c&0x001f)<<3)+(cb.v>>16)]&0xffff0000;
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      x+=2;
    }
    if (x==-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2r[((c&0xf800)>>8)+(cr.v>>16)]+
              epal2g[((c&0x07e0)>>3)+(cg.v>>16)]+
              epal2b[((c&0x001f)<<3)+(cb.v>>16)])>>16;
      //u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    cr.v2+=(cr.xi&mask)+cr.ei;
    cg.v2+=(cg.xi&mask)+cg.ei;
    cb.v2+=(cb.xi&mask)+cb.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=256-(mid->y&0xff);
  fistq(&e1->x64, (mid->x+yfix*f3)*16777216.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*256-mid->x;
    u.v2=(xfix*u.a+yfix*u.b+128>>8)+(mid->u<<8>>sh3); u.ei=u.a*b.a+u.b;
    v.v2=(xfix*v.a+yfix*v.b+128>>8)+(mid->v<<8>>sh3); v.ei=v.a*b.a+v.b;
    cr.v2=(xfix*(cr.a>>4)+yfix*(cr.b>>4)>>4)+mid->r; cr.ei=cr.a*b.a+cr.b;
    cg.v2=(xfix*(cg.a>>4)+yfix*(cg.b>>4)>>4)+mid->g; cg.ei=cg.a*b.a+cg.b;
    cb.v2=(xfix*(cb.a>>4)+yfix*(cb.b>>4)>>4)+mid->b; cb.ei=cb.a*b.a+cb.b;
    u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
    u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
    v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
    v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>8)-(bot->y>>8);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; cr.v=cr.v2; cg.v=cg.v2; cb.v=cb.v2;
    x=b.p-e.p;
    if (x<0 && (b.x&2)) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2r[((c&0xf800)>>8)+(cr.v>>16)]+
              epal2g[((c&0x07e0)>>3)+(cg.v>>16)]+
              epal2b[((c&0x001f)<<3)+(cb.v>>16)])>>16;
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      x++;
    }
    while (x<-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      d=epal2r[((c&0xf800)>>8)+(cr.v>>16)]+
        epal2g[((c&0x07e0)>>3)+(cg.v>>16)]+
        epal2b[((c&0x001f)<<3)+(cb.v>>16)]&0xffff;
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      *(int*)(e.p+x)=d|epal2r[((c&0xf800)>>8)+(cr.v>>16)]
                      +epal2g[((c&0x07e0)>>3)+(cg.v>>16)]
                      +epal2b[((c&0x001f)<<3)+(cb.v>>16)]&0xffff0000;
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      x+=2;
    }
    if (x==-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2r[((c&0xf800)>>8)+(cr.v>>16)]+
              epal2g[((c&0x07e0)>>3)+(cg.v>>16)]+
              epal2b[((c&0x001f)<<3)+(cb.v>>16)])>>16;
      //u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    cr.v2+=(cr.xi&mask)+cr.ei;
    cg.v2+=(cg.xi&mask)+cg.ei;
    cb.v2+=(cb.xi&mask)+cb.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}
*/

void filltri(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  //static Tiledata *td;
  static int uamask, vamask, uomask, vomask, sh1, sh2, sh3, zsh, fbi;
  static struct {
    union { struct { unsigned xf; union { short *p; int x; }; }; __int64 x64; };
    union { struct { unsigned af; int a; }; __int64 a64; };
    int a2;
  } b, e, *e1, *e2;
  static struct {
    union { int v; unsigned w; };
    union { int v2; unsigned w2; };
    int a, b, xi, ei;
  } u, v, cr, cg, cb;
  //3256 3188 3148 3120 3104
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=(short*)dbuf+(top->y>>8)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>8)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>8)*pitchp2;

  //if ((int)t&60) return;
  ala=256.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  cr.a=inear(((mid->r-top->r)*bty-(bot->r-top->r)*mty)*ala);
  cr.b=inear(((bot->r-top->r)*mtx-(mid->r-top->r)*btx)*ala);
  cg.a=inear(((mid->g-top->g)*bty-(bot->g-top->g)*mty)*ala);
  cg.b=inear(((bot->g-top->g)*mtx-(mid->g-top->g)*btx)*ala);
  cb.a=inear(((mid->b-top->b)*bty-(bot->b-top->b)*mty)*ala);
  cb.b=inear(((bot->b-top->b)*mtx-(mid->b-top->b)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31);
  c=(v.a^v.a>>31)+(v.b^v.b>>31); if (c<tmp) tmp=c; tmp<<=1;
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>(raja>>txt->size)) txt=txt->mipnext;
  c=top->u^mid->u|mid->u^bot->u|top->v^mid->v|mid->v^bot->v|31;
  zsh=0; bits=(unsigned short*)txt->bits16l;
  sh1=32-2*txt->size; sh2=24-txt->size; sh3=1+txt->size;
  //while (c<0x800000 && sh1<28 && sh3>3) {
    //bits+=(unsigned)(top->v<<8+zsh&0x80000000|top->u<<7+zsh&0x40000000)>>sh1;
    //c<<=2; zsh++; sh1+=2; sh3-=2;
  //}
  uomask=vamask=0xaaaaaaaa<<sh1;
  uamask=vomask=0x55555555<<sh1;
  fbi=0x7fffffff>>32-sh1;
  uamask|=fbi  ; vamask|=fbi  ;
  uomask|=fbi+1; vomask|=fbi+1;

  if (sh3<8) u.a<<=8-sh3, v.a<<=8-sh3, u.b<<=8-sh3, v.b<<=8-sh3;
    else u.a>>=sh3-8, v.a>>=sh3-8, u.b>>=sh3-8, v.b>>=sh3-8;
  sh2+=8-sh3;
  u.xi=(hajotbl[u.a>>sh2&0xfff]<<sh1  )+(u.a&fbi)|uomask;
  v.xi=(hajotbl[v.a>>sh2&0xfff]<<sh1+1)+(v.a&fbi)|vomask;
  cr.xi=cr.a; cg.xi=cg.a; cb.xi=cb.a;

  yfix=256-(top->y&0xff);
  fistq(&e1->x64, (top->x+yfix*f1)*16777216.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*16777216.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*256-top->x;
  u.v2=(xfix*u.a+yfix*u.b+128>>8)+(top->u<<8>>sh3); u.ei=u.a*b.a+u.b;
  v.v2=(xfix*v.a+yfix*v.b+128>>8)+(top->v<<8>>sh3); v.ei=v.a*b.a+v.b;
  cr.v2=(xfix*(cr.a>>4)+yfix*(cr.b>>4)>>4)+top->r; cr.ei=cr.a*b.a+cr.b;
  cg.v2=(xfix*(cg.a>>4)+yfix*(cg.b>>4)>>4)+top->g; cg.ei=cg.a*b.a+cg.b;
  cb.v2=(xfix*(cb.a>>4)+yfix*(cb.b>>4)>>4)+top->b; cb.ei=cb.a*b.a+cb.b;
  u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
  u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
  v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
  v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>8)-(mid->y>>8);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; cr.v=cr.v2; cg.v=cg.v2; cb.v=cb.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal[((c&0xf800)>>10)+(cr.v>>12&16320)]&0xf800|
              epal[((c&0x07e0)>>5)+(cg.v>>12&16320)]&0x07e0|
              epal[((c&0x001f)<<1)+(cb.v>>12&16320)]&0x001f);
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      x++;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    cr.v2+=(cr.xi&mask)+cr.ei;
    cg.v2+=(cg.xi&mask)+cg.ei;
    cb.v2+=(cb.xi&mask)+cb.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=256-(mid->y&0xff);
  fistq(&e1->x64, (mid->x+yfix*f3)*16777216.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*256-mid->x;
    u.v2=(xfix*u.a+yfix*u.b+128>>8)+(mid->u<<8>>sh3); u.ei=u.a*b.a+u.b;
    v.v2=(xfix*v.a+yfix*v.b+128>>8)+(mid->v<<8>>sh3); v.ei=v.a*b.a+v.b;
    cr.v2=(xfix*(cr.a>>4)+yfix*(cr.b>>4)>>4)+mid->r; cr.ei=cr.a*b.a+cr.b;
    cg.v2=(xfix*(cg.a>>4)+yfix*(cg.b>>4)>>4)+mid->g; cg.ei=cg.a*b.a+cg.b;
    cb.v2=(xfix*(cb.a>>4)+yfix*(cb.b>>4)>>4)+mid->b; cb.ei=cb.a*b.a+cb.b;
    u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
    u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
    v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
    v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>8)-(bot->y>>8);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; cr.v=cr.v2; cg.v=cg.v2; cb.v=cb.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal[((c&0xf800)>>10)+(cr.v>>12&16320)]&0xf800|
              epal[((c&0x07e0)>>5)+(cg.v>>12&16320)]&0x07e0|
              epal[((c&0x001f)<<1)+(cb.v>>12&16320)]&0x001f);
      u.v+=u.xi; v.v+=v.xi; cr.v+=cr.xi; cg.v+=cg.xi; cb.v+=cb.xi;
      x++;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    cr.v2+=(cr.xi&mask)+cr.ei;
    cg.v2+=(cg.xi&mask)+cg.ei;
    cb.v2+=(cb.xi&mask)+cb.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}


void filltri_noshade(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  //static Tiledata *td;
  static int uamask, vamask, uomask, vomask, sh1, sh2, sh3, zsh, fbi;
  static struct {
    union { struct { unsigned xf; union { short *p; int x; }; }; __int64 x64; };
    union { struct { unsigned af; int a; }; __int64 a64; };
    int a2;
  } b, e, *e1, *e2;
  static struct {
    union { int v; unsigned w; };
    union { int v2; unsigned w2; };
    int a, b, xi, ei;
  } u, v;
  //3256 3188 3148 3120 3104
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=(short*)dbuf+(top->y>>8)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>8)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>8)*pitchp2;

  //if ((int)t&60) return;
  ala=256.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31);
  c=(v.a^v.a>>31)+(v.b^v.b>>31); if (c<tmp) tmp=c; tmp<<=1;
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>(raja>>txt->size)) txt=txt->mipnext;
  c=top->u^mid->u|mid->u^bot->u|top->v^mid->v|mid->v^bot->v|31;
  zsh=0; bits=(unsigned short*)txt->bits16l;
  sh1=32-2*txt->size; sh2=24-txt->size; sh3=1+txt->size;
  //while (c<0x800000 && sh1<28 && sh3>3) {
    //bits+=(unsigned)(top->v<<8+zsh&0x80000000|top->u<<7+zsh&0x40000000)>>sh1;
    //c<<=2; zsh++; sh1+=2; sh3-=2;
  //}
  uomask=vamask=0xaaaaaaaa<<sh1;
  uamask=vomask=0x55555555<<sh1;
  fbi=0x7fffffff>>32-sh1;
  uamask|=fbi  ; vamask|=fbi  ;
  uomask|=fbi+1; vomask|=fbi+1;

  if (sh3<8) u.a<<=8-sh3, v.a<<=8-sh3, u.b<<=8-sh3, v.b<<=8-sh3;
    else u.a>>=sh3-8, v.a>>=sh3-8, u.b>>=sh3-8, v.b>>=sh3-8;
  sh2+=8-sh3;
  u.xi=(hajotbl[u.a>>sh2&0xfff]<<sh1  )+(u.a&fbi)|uomask;
  v.xi=(hajotbl[v.a>>sh2&0xfff]<<sh1+1)+(v.a&fbi)|vomask;

  yfix=256-(top->y&0xff);
  fistq(&e1->x64, (top->x+yfix*f1)*16777216.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*16777216.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*256-top->x;
  u.v2=(xfix*u.a+yfix*u.b+128>>8)+(top->u<<8>>sh3); u.ei=u.a*b.a+u.b;
  v.v2=(xfix*v.a+yfix*v.b+128>>8)+(top->v<<8>>sh3); v.ei=v.a*b.a+v.b;
  u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
  u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
  v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
  v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>8)-(mid->y>>8);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      e.p[x]=bits[(unsigned)(u.v+v.v)>>sh1];
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=256-(mid->y&0xff);
  fistq(&e1->x64, (mid->x+yfix*f3)*16777216.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*256-mid->x;
    u.v2=(xfix*u.a+yfix*u.b+128>>8)+(mid->u<<8>>sh3); u.ei=u.a*b.a+u.b;
    v.v2=(xfix*v.a+yfix*v.b+128>>8)+(mid->v<<8>>sh3); v.ei=v.a*b.a+v.b;
    u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
    u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
    v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
    v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>8)-(bot->y>>8);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      e.p[x]=bits[(unsigned)(u.v+v.v)>>sh1];
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}




void filltri_tv(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  //static Tiledata *td;
  static int uamask, vamask, uomask, vomask, sh1, sh2, sh3, zsh, fbi;
  static struct {
    union { struct { unsigned xf; union { short *p; int x; }; }; __int64 x64; };
    union { struct { unsigned af; int a; }; __int64 a64; };
    int a2;
  } b, e, *e1, *e2;
  static struct {
    union { int v; unsigned w; };
    union { int v2; unsigned w2; };
    int a, b, xi, ei;
  } u, v, cf;
  //3256 3188 3148 3120 3104
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=(short*)dbuf+(top->y>>8)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>8)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>8)*pitchp2;

  //if ((int)t&60) return;
  ala=256.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  cf.a=inear(((mid->r-top->r)*bty-(bot->r-top->r)*mty)*ala);
  cf.b=inear(((bot->r-top->r)*mtx-(mid->r-top->r)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31);
  c=(v.a^v.a>>31)+(v.b^v.b>>31); if (c<tmp) tmp=c; tmp<<=1;
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>(raja>>txt->size)) txt=txt->mipnext;
  c=top->u^mid->u|mid->u^bot->u|top->v^mid->v|mid->v^bot->v|31;
  zsh=0; bits=(unsigned short*)txt->bits16l;
  sh1=32-2*txt->size; sh2=24-txt->size; sh3=1+txt->size;
  //while (c<0x800000 && sh1<28 && sh3>3) {
    //bits+=(unsigned)(top->v<<8+zsh&0x80000000|top->u<<7+zsh&0x40000000)>>sh1;
    //c<<=2; zsh++; sh1+=2; sh3-=2;
  //}
  uomask=vamask=0xaaaaaaaa<<sh1;
  uamask=vomask=0x55555555<<sh1;
  fbi=0x7fffffff>>32-sh1;
  uamask|=fbi  ; vamask|=fbi  ;
  uomask|=fbi+1; vomask|=fbi+1;

  if (sh3<8) u.a<<=8-sh3, v.a<<=8-sh3, u.b<<=8-sh3, v.b<<=8-sh3;
    else u.a>>=sh3-8, v.a>>=sh3-8, u.b>>=sh3-8, v.b>>=sh3-8;
  sh2+=8-sh3;
  u.xi=(hajotbl[u.a>>sh2&0xfff]<<sh1  )+(u.a&fbi)|uomask;
  v.xi=(hajotbl[v.a>>sh2&0xfff]<<sh1+1)+(v.a&fbi)|vomask;
  cf.xi=cf.a;

  yfix=256-(top->y&0xff);
  fistq(&e1->x64, (top->x+yfix*f1)*16777216.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*16777216.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*256-top->x;
  u.v2=(xfix*u.a+yfix*u.b+128>>8)+(top->u<<8>>sh3); u.ei=u.a*b.a+u.b;
  v.v2=(xfix*v.a+yfix*v.b+128>>8)+(top->v<<8>>sh3); v.ei=v.a*b.a+v.b;
  cf.v2=(xfix*(cf.a>>4)+yfix*(cf.b>>4)>>4)+top->r; cf.ei=cf.a*b.a+cf.b;
  u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
  u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
  v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
  v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>8)-(mid->y>>8);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; cf.v=cf.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(fpal[((c&0xf800)>>10)+(cf.v>>12&16320)]&0xf800|
              fpal[((c&0x07e0)>>5)+(cf.v>>12&16320)]&0x07e0|
              fpal[((c&0x001f)<<1)+(cf.v>>12&16320)]&0x001f);
      u.v+=u.xi; v.v+=v.xi; cf.v+=cf.xi;
      x++;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    cf.v2+=(cf.xi&mask)+cf.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=256-(mid->y&0xff);
  fistq(&e1->x64, (mid->x+yfix*f3)*16777216.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*256-mid->x;
    u.v2=(xfix*u.a+yfix*u.b+128>>8)+(mid->u<<8>>sh3); u.ei=u.a*b.a+u.b;
    v.v2=(xfix*v.a+yfix*v.b+128>>8)+(mid->v<<8>>sh3); v.ei=v.a*b.a+v.b;
    cf.v2=(xfix*(cf.a>>4)+yfix*(cf.b>>4)>>4)+mid->r; cf.ei=cf.a*b.a+cf.b;
    u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
    u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
    v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
    v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>8)-(bot->y>>8);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; cf.v=cf.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(fpal[((c&0xf800)>>10)+(cf.v>>12&16320)]&0xf800|
              fpal[((c&0x07e0)>>5)+(cf.v>>12&16320)]&0x07e0|
              fpal[((c&0x001f)<<1)+(cf.v>>12&16320)]&0x001f);
      u.v+=u.xi; v.v+=v.xi; cf.v+=cf.xi;
      x++;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    cf.v2+=(cf.xi&mask)+cf.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}





void filltri_phlat(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static struct {
    union { struct { unsigned xf; union { short *p; int x; }; }; __int64 x64; };
    union { struct { unsigned af; int a; }; __int64 a64; };
    int a2;
  } b, e, *e1, *e2;
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  return;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=(short*)dbuf+(top->y>>8)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>8)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>8)*pitchp2;

  yfix=256-(top->y&0xff);
  fistq(&e1->x64, (top->x+yfix*f1)*16777216.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*16777216.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*256-top->x;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>8)-(mid->y>>8);

  for (; y<0; y++) {
    x=b.p-e.p;
    while (x<0) e.p[x++]=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=256-(mid->y&0xff);
  fistq(&e1->x64, (mid->x+yfix*f3)*16777216.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*256-mid->x;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>8)-(bot->y>>8);
  for (; y<0; y++) {
    x=b.p-e.p;
    while (x<0) e.p[x++]=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}






void filltri_ouverlei(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  //static Tiledata *td;
  static int uamask, vamask, uomask, vomask, sh1, sh2, sh3, zsh, fbi;
  static struct {
    union { struct { unsigned xf; union { short *p; int x; }; }; __int64 x64; };
    union { struct { unsigned af; int a; }; __int64 a64; };
    int a2;
  } b, e, *e1, *e2;
  static struct {
    union { int v; unsigned w; };
    union { int v2; unsigned w2; };
    int a, b, xi, ei;
  } u, v, ci;
  //3256 3188 3148 3120 3104
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=(short*)dbuf+(top->y>>8)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>8)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>8)*pitchp2;

  //if ((int)t&60) return;
  ala=256.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  ci.a=inear(((mid->r-top->r)*bty-(bot->r-top->r)*mty)*ala);
  ci.b=inear(((bot->r-top->r)*mtx-(mid->r-top->r)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31);
  c=(v.a^v.a>>31)+(v.b^v.b>>31); if (c<tmp) tmp=c; tmp<<=1;
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>(raja>>txt->size)) txt=txt->mipnext;
  c=top->u^mid->u|mid->u^bot->u|top->v^mid->v|mid->v^bot->v|31;
  zsh=0; bits=(unsigned short*)txt->bits16l;
  sh1=32-2*txt->size; sh2=24-txt->size; sh3=1+txt->size;
  //while (c<0x800000 && sh1<28 && sh3>3) {
    //bits+=(unsigned)(top->v<<8+zsh&0x80000000|top->u<<7+zsh&0x40000000)>>sh1;
    //c<<=2; zsh++; sh1+=2; sh3-=2;
  //}
  uomask=vamask=0xaaaaaaaa<<sh1;
  uamask=vomask=0x55555555<<sh1;
  fbi=0x7fffffff>>32-sh1;
  uamask|=fbi  ; vamask|=fbi  ;
  uomask|=fbi+1; vomask|=fbi+1;

  if (sh3<8) u.a<<=8-sh3, v.a<<=8-sh3, u.b<<=8-sh3, v.b<<=8-sh3;
    else u.a>>=sh3-8, v.a>>=sh3-8, u.b>>=sh3-8, v.b>>=sh3-8;
  sh2+=8-sh3;
  u.xi=(hajotbl[u.a>>sh2&0xfff]<<sh1  )+(u.a&fbi)|uomask;
  v.xi=(hajotbl[v.a>>sh2&0xfff]<<sh1+1)+(v.a&fbi)|vomask;
  ci.xi=ci.a;

  yfix=256-(top->y&0xff);
  fistq(&e1->x64, (top->x+yfix*f1)*16777216.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*16777216.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*256-top->x;
  u.v2=(xfix*u.a+yfix*u.b+128>>8)+(top->u<<8>>sh3); u.ei=u.a*b.a+u.b;
  v.v2=(xfix*v.a+yfix*v.b+128>>8)+(top->v<<8>>sh3); v.ei=v.a*b.a+v.b;
  ci.v2=(xfix*(ci.a>>4)+yfix*(ci.b>>4)>>4)+top->r; ci.ei=ci.a*b.a+ci.b;
  u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
  u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
  v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
  v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>8)-(mid->y>>8);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; ci.v=ci.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      c=(epal[((c&0xf800)>>10)+(ci.v>>12&16320)]&0xf800|
         epal[((c&0x07e0)>>5)+(ci.v>>12&16320)]&0x07e0|
         epal[((c&0x001f)<<1)+(ci.v>>12&16320)]&0x001f);
      e.p[x]=sumeioverflov(c, e.p[x]);
      u.v+=u.xi; v.v+=v.xi; ci.v+=ci.xi;
      x++;
    }
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    ci.v2+=(ci.xi&mask)+ci.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=256-(mid->y&0xff);
  fistq(&e1->x64, (mid->x+yfix*f3)*16777216.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*256-mid->x;
    u.v2=(xfix*u.a+yfix*u.b+128>>8)+(mid->u<<8>>sh3); u.ei=u.a*b.a+u.b;
    v.v2=(xfix*v.a+yfix*v.b+128>>8)+(mid->v<<8>>sh3); v.ei=v.a*b.a+v.b;
    ci.v2=(xfix*(ci.a>>4)+yfix*(ci.b>>4)>>4)+mid->r; ci.ei=ci.a*b.a+ci.b;
    u.v2=(hajotbl[u.v2>>sh2&0xfff]<<sh1  )+(u.v2&fbi);
    u.ei=(hajotbl[u.ei>>sh2&0xfff]<<sh1  )+(u.ei&fbi)|uomask;
    v.v2=(hajotbl[v.v2>>sh2&0xfff]<<sh1+1)+(v.v2&fbi);
    v.ei=(hajotbl[v.ei>>sh2&0xfff]<<sh1+1)+(v.ei&fbi)|vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>8)-(bot->y>>8);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2; ci.v=ci.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      c=(epal[((c&0xf800)>>10)+(ci.v>>12&16320)]&0xf800|
         epal[((c&0x07e0)>>5)+(ci.v>>12&16320)]&0x07e0|
         epal[((c&0x001f)<<1)+(ci.v>>12&16320)]&0x001f);
      e.p[x]=sumeioverflov(c, e.p[x]);
      u.v+=u.xi; v.v+=v.xi; ci.v+=ci.xi;
      x++;
    }
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    ci.v2+=(ci.xi&mask)+ci.ei;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}



static float getk(float k, float z1, float z2) {
  float z=k*z1/(z2+(z1-z2)*k);
  return z<0?0:z>1?1:z;
}
void filltri_matrix(Tri2d *t) {
  static Tri2d kewl;
  static Vertex2d a, b, *v1, *v2, *v3, *vx;
  int i;
  float k;
  kewl.top=&a;
  kewl.mid=&b;
  filltri_phlat(t);
  v1=t->top; v2=t->mid; v3=t->bot;
  if (v1->u>v2->u) vx=v2, v2=v1, v1=vx;
  if (v2->u>v3->u) vx=v3, v3=v2, v2=vx;
  if (v1->u>v2->u) vx=v2, v2=v1, v1=vx;
  for (i=(v1->u+65535&~65535); i<v3->u; i+=65536) {
    if (i<v2->u)
      k=getk((i-v1->u)/(float)(v2->u-v1->u), v1->z, v2->z),
      a.x=v1->x+(v2->x-v1->x)*k, a.y=v1->y+(v2->y-v1->y)*k, a.z=v1->z+(v2->z-v1->z)*k;
    else
      k=getk((i-v2->u)/(float)(v3->u-v2->u), v2->z, v3->z),
      a.x=v2->x+(v3->x-v2->x)*k, a.y=v2->y+(v3->y-v2->y)*k, a.z=v2->z+(v3->z-v2->z)*k;
    k=getk((i-v1->u)/(float)(v3->u-v1->u), v1->z, v3->z),
    b.x=v1->x+(v3->x-v1->x)*k, b.y=v1->y+(v3->y-v1->y)*k, b.z=v1->z+(v3->z-v1->z)*k;
    if (a.z>1024 || b.z>1024) matrixline(&kewl);
  }
  if (v1->v>v2->v) vx=v2, v2=v1, v1=vx;
  if (v2->v>v3->v) vx=v3, v3=v2, v2=vx;
  if (v1->v>v2->v) vx=v2, v2=v1, v1=vx;
  for (i=(v1->v+65535&~65535); i<v3->v; i+=65536) {
    if (i<v2->v)
      k=getk((i-v1->v)/(float)(v2->v-v1->v), v1->z, v2->z),
      a.x=v1->x+(v2->x-v1->x)*k, a.y=v1->y+(v2->y-v1->y)*k, a.z=(v1->z+(v2->z-v1->z)*k);//*((i^i+1)>>16);
    else
      k=getk((i-v2->v)/(float)(v3->v-v2->v), v2->z, v3->z),
      a.x=v2->x+(v3->x-v2->x)*k, a.y=v2->y+(v3->y-v2->y)*k, a.z=(v2->z+(v3->z-v2->z)*k);//*((i^i+1)>>16);
    k=getk((i-v1->v)/(float)(v3->v-v1->v), v1->z, v3->z),
    b.x=v1->x+(v3->x-v1->x)*k, b.y=v1->y+(v3->y-v1->y)*k, b.z=(v1->z+(v3->z-v1->z)*k);//*((i^i+1)>>16);
    if (a.z>1024 || b.z>1024) matrixline(&kewl);
  }
}





#define ip16(x, y, f) ((((x)&0xf81f)*(f)+((y)&0xf81f)*(64-(f))+0x18030>>6&0xf81f)+(((x)&0x07e0)*(f)+((y)&0x07e0)*(64-(f))+0x0600>>6&0x07e0))
#define ip32(x, y, f) ((((x)&0xff00ff)*(f)+((y)&0xff00ff)*(128-(f))+0x600060>>7&0xff00ff)+(((x)&0xff00)*(f)+((y)&0xff00)*(128-(f))+0x6000>>7&0xff00))

void aadrawline(Tri2d *t) {
  int x, y, dx, dy, a, b, k, kk;
  Vertex2d *v1, *v2;
  short *p;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*256-65536+(256-(v1->x&255))*(1/256.0)*dy;
    for (x=v1->x>>8; x<v2->x>>8; x++) {
      if ((unsigned)(y>>16)<lfb.height-1) {
        p=(short*)dbuf+(y>>16)*pitchp2+x; k=(y+32768&65535)>>10;
        a=p[0]; b=p[pitchp2];
        ((short*)dbuf)[(y+32768>>16)*pitchp2+x]=ip16(a, b, k);
      }
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*256-65536+(256-(v1->y&255))*(1/256.0)*dx;
    for (y=v1->y>>8; y<v2->y>>8; y++) {
      if ((unsigned)(x>>16)<lfb.width-1) {
        p=(short*)dbuf+y*pitchp2+(x>>16); k=(x+32768&65535)>>10;
        a=p[0]; b=p[1];
        ((short*)dbuf)[y*pitchp2+(x+32768>>16)]=ip16(a, b, k);
      }
      x+=dx;
    }
  }
}

void matrixline(Tri2d *t) {
  int x, y, dx, dy, a, b, k, kk, z;
  Vertex2d *v1, *v2;
  short *p;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    z=v1->z+v2->z>>2; if (z>1048575) z=1048575;
    dy=dy*65536.0/dx; y=v1->y*256-65536+(256-(v1->x&255))*(1/256.0)*dy;
    for (x=v1->x>>8; x<v2->x>>8; x++) {
      if ((unsigned)(y>>16)<lfb.height-1) {
        p=(short*)dbuf+(y>>16)*pitchp2+x; k=(y+32768&65535)>>10;
        p[0]+=(65535-y>>10&63)*z>>12;
        p[pitchp2]+=(y>>10&63)*z>>12;
      }
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    z=v1->z+v2->z>>2; if (z>1048575) z=1048575;
    dx=dx*65536.0/dy; x=v1->x*256-65536+(256-(v1->y&255))*(1/256.0)*dx;
    for (y=v1->y>>8; y<v2->y>>8; y++) {
      if ((unsigned)(x>>16)<lfb.width-1) {
        p=(short*)dbuf+y*pitchp2+(x>>16); k=(x+32768&65535)>>10;
        p[0]+=(65535-x>>10&63)*z>>12;
        p[1]+=(x>>10&63)*z>>12;
      }
      x+=dx;
    }
  }
}

void greenline(Tri2d *t) {
  int x, y, dx, dy, a, b, k, kk, z;
  Vertex2d *v1, *v2;
  short *p;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    z=v1->z+v2->z>>2; if (z>1048575) z=1048575;
    dy=dy*65536.0/dx; y=v1->y*256-65536+(256-(v1->x&255))*(1/256.0)*dy;
    for (x=v1->x>>8; x<v2->x>>8; x++) {
      if ((unsigned)(y>>16)<lfb.height-1) {
        p=(short*)dbuf+(y>>16)*pitchp2+x; k=(y+32768&65535)>>10;
        p[0]+=(65535-y>>10&63)>>3<<5;
        p[pitchp2]+=(y>>10&63)>>3<<5;
      }
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    z=v1->z+v2->z>>2; if (z>1048575) z=1048575;
    dx=dx*65536.0/dy; x=v1->x*256-65536+(256-(v1->y&255))*(1/256.0)*dx;
    for (y=v1->y>>8; y<v2->y>>8; y++) {
      if ((unsigned)(x>>16)<lfb.width-1) {
        p=(short*)dbuf+y*pitchp2+(x>>16); k=(x+32768&65535)>>10;
        p[0]+=(65535-x>>10&63)>>3<<5;
        p[1]+=(x>>10&63)>>3<<5;
      }
      x+=dx;
    }
  }
}


void portaltri(Tri2d *t) {
  //Space *tmp=curf;
  //space_draw((Space*)t->top, (Space*)t->mid);
  //space_close((Space*)t->top);
  //triprep(tmp);
}




