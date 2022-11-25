#include "functs.h"
#include "core.h"
#include "matter.pri"
#include "space.pri"
#include "mesh.pri"
// 3224 3131 3132 3081 3233 2940 3129 2842 2789 2740 2363 2822 2813 2763
// 2243

static void *dbuf;
static Lfb lfb;
static Space *curf;
static int pitch, pitchp2, pitchp4;


static int exppal[65536+256];
#define epal2 (exppal+32768)
int biltbl[257];
void triprep(Space *f) {
  int i, j;
  curf=f; lfb=f->dbuf; dbuf=(short*)lfb.buf;
  pitchp4=(pitchp2=(pitch=f->dbuf.pitch)>>1)>>1;
  if (!exppal[65536]) {
    for (i=0; i<65536+256; i++) {
      int v;
      if (i>=32768+256) v=1023; else
        if (i<=32768-256) v=0; else
        v=1023*fexp2((i-32768-256)*(texture_contrast/256.0)*(palette_gamma/monitor_gamma));
      //exppal[i]=(v>>5<<11)+(v>>4<<5)+(v>>5); exppal[i]|=exppal[i]<<16;
      exppal[i]=(v<<22)+(v<<11)+(v);
    }
    for (i=0; i<257; i++) {
      float f;
      if (i<8) j=8; else if (i>248) j=248; else j=i;
      f=(j-128)/128.0; //if (f!=0.0) f*=fpow(ffabs(f),  0.2);
      biltbl[i]=flog2((f+1)*0.5)*(256.0/texture_contrast)*(monitor_gamma/palette_gamma);
    }
  }
}



void filllst(Tri2d *x) { for (; x; x=x->next) x->m->fill(x); }



void filltri_nobi(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, d;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  static Tiledata *td;
  static int uamask, vamask, sh1;
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
  dp1=(short*)dbuf+(top->y>>12)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>12)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>12)*pitchp2;

  //if ((int)t&60) return;
  ala=4096.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31)+(v.a^v.a>>31)+(v.b^v.b>>31);
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>txt->td->raja) txt=txt->mipnext;
  td=txt->td; bits=(unsigned short*)txt->bits16p; uamask=td->uamask; vamask=td->vamask; sh1=td->sh1;
  u.xi=td->utbl[u.a>>td->sh2&0xfff]+(u.a<<8>>td->sh3&td->fbits)|td->uomask;
  v.xi=td->vtbl[v.a>>td->sh2&0xfff]+(v.a<<8>>td->sh3&td->fbits)|td->vomask;

  yfix=4096-(top->y&0xfff);
  fistq(&e1->x64, (top->x+yfix*f1)*1048576.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*1048576.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*4096-top->x;
  u.v2=(xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+top->u; u.ei=u.a*b.a+u.b;
  v.v2=(xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+top->v; v.ei=v.a*b.a+v.b;
  u.v2=td->utbl[u.v2>>td->sh2&0xfff]+(u.v2<<8>>td->sh3&td->fbits);
  u.ei=td->utbl[u.ei>>td->sh2&0xfff]+(u.ei<<8>>td->sh3&td->fbits)|td->uomask;
  v.v2=td->vtbl[v.v2>>td->sh2&0xfff]+(v.v2<<8>>td->sh3&td->fbits);
  v.ei=td->vtbl[v.ei>>td->sh2&0xfff]+(v.ei<<8>>td->sh3&td->fbits)|td->vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>12)-(mid->y>>12);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2;
    x=b.p-e.p;
    if (x<0 && (b.x&2)) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      d=epal2[(c&0xf800)>>8]&0x0000f800|
        epal2[(c&0x07e0)>>3]&0x000007e0|
        epal2[(c&0x001f)<<3]&0x0000001f;
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      *(int*)(e.p+x)=d|epal2[(c&0xf800)>>8]&0xf8000000
                      |epal2[(c&0x07e0)>>3]&0x07e00000
                      |epal2[(c&0x001f)<<3]&0x001f0000;
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (x==-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      u.v+=u.xi; v.v+=v.xi;
    }
    //if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->x64, (mid->x+yfix*f3)*1048576.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*4096-mid->x;
    u.v2=(xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+mid->u; u.ei=u.a*b.a+u.b;
    v.v2=(xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+mid->v; v.ei=v.a*b.a+v.b;
    u.v2=td->utbl[u.v2>>td->sh2&0xfff]+(u.v2<<8>>td->sh3&td->fbits);
    u.ei=td->utbl[u.ei>>td->sh2&0xfff]+(u.ei<<8>>td->sh3&td->fbits)|td->uomask;
    v.v2=td->vtbl[v.v2>>td->sh2&0xfff]+(v.v2<<8>>td->sh3&td->fbits);
    v.ei=td->vtbl[v.ei>>td->sh2&0xfff]+(v.ei<<8>>td->sh3&td->fbits)|td->vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>12)-(bot->y>>12);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2;
    x=b.p-e.p;
    if (x<0 && (b.x&2)) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      d=epal2[(c&0xf800)>>8]&0x0000f800|
        epal2[(c&0x07e0)>>3]&0x000007e0|
        epal2[(c&0x001f)<<3]&0x0000001f;
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      *(int*)(e.p+x)=d|epal2[(c&0xf800)>>8]&0xf8000000
                      |epal2[(c&0x07e0)>>3]&0x07e00000
                      |epal2[(c&0x001f)<<3]&0x001f0000;
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (x==-1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      u.v+=u.xi; v.v+=v.xi;
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



void filltri(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c, c2, c3, c4, d;
  int u_one, v_one;
  static int mask, flag, tmp;
  short *dp1, *dp2, *dp3;
  static unsigned short *bits;
  static Texture *txt;
  static Tiledata *td;
  static int uamask, vamask, sh1;
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
  dp1=(short*)dbuf+(top->y>>12)*pitchp2;
  dp2=(short*)dbuf+(mid->y>>12)*pitchp2;
  dp3=(short*)dbuf+(bot->y>>12)*pitchp2;

  //if ((int)t&60) return;
  ala=4096.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31)+(v.a^v.a>>31)+(v.b^v.b>>31);
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>txt->td->raja) txt=txt->mipnext;
  td=txt->td; bits=(unsigned short*)txt->bits16p; uamask=td->uamask; vamask=td->vamask; sh1=td->sh1;
  u.xi=td->utbl[u.a>>td->sh2&0xfff]+(u.a<<8>>td->sh3&td->fbits)|td->uomask;
  v.xi=td->vtbl[v.a>>td->sh2&0xfff]+(v.a<<8>>td->sh3&td->fbits)|td->vomask;
  u_one=td->utbl[1]|td->uomask;
  v_one=td->vtbl[1]|td->vomask;

  yfix=4096-(top->y&0xfff);
  fistq(&e1->x64, (top->x+yfix*f1)*1048576.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  fistq(&e2->x64, (top->x+yfix*f2)*1048576.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitchp2-256;
  xfix=(b.x+1)*4096-top->x;
  u.v2=(xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+top->u; u.ei=u.a*b.a+u.b;
  v.v2=(xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+top->v; v.ei=v.a*b.a+v.b;
  u.v2=td->utbl[u.v2>>td->sh2&0xfff]+(u.v2<<8>>td->sh3&td->fbits);
  u.ei=td->utbl[u.ei>>td->sh2&0xfff]+(u.ei<<8>>td->sh3&td->fbits)|td->uomask;
  v.v2=td->vtbl[v.v2>>td->sh2&0xfff]+(v.v2<<8>>td->sh3&td->fbits);
  v.ei=td->vtbl[v.ei>>td->sh2&0xfff]+(v.ei<<8>>td->sh3&td->fbits)|td->vomask;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>12)-(mid->y>>12);

  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c =bits[(unsigned)(u.v+v.v)>>sh1];
      c2=bits[(unsigned)((u.v+u_one&uamask)+v.v)>>sh1];
      c3=bits[(unsigned)(u.v+(v.v+v_one&vamask))>>sh1];
      c4=bits[(unsigned)((u.v+u_one&uamask)+(v.v+v_one&vamask))>>sh1];
      {
      int xx=u.v>>sh1-9&255, yy=v.v>>sh1-9&255, bx0=biltbl[xx], bx1=biltbl[256-xx], by0=biltbl[yy], by1=biltbl[256-yy];
      d=epal2[((c4&0xf800)>>8)+bx0+by0]+epal2[((c3&0xf800)>>8)+bx1+by0]+epal2[((c2&0xf800)>>8)+bx0+by1]+epal2[((c &0xf800)>>8)+bx1+by1]&0x001ff800|
        epal2[((c4&0x07e0)>>3)+bx0+by0]+epal2[((c3&0x07e0)>>3)+bx1+by0]+epal2[((c2&0x07e0)>>3)+bx0+by1]+epal2[((c &0x07e0)>>3)+bx1+by1]&0xffe00000|
        epal2[((c4&0x001f)<<3)+bx0+by0]+epal2[((c3&0x001f)<<3)+bx1+by0]+epal2[((c2&0x001f)<<3)+bx0+by1]+epal2[((c &0x001f)<<3)+bx1+by1]&0x000003ff;
      e.p[x]=d>>5&0xf81f|d>>21&0x07e0;
      }
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->x64, (mid->x+yfix*f3)*1048576.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitchp2-256;
  if (flag) {
    xfix=(b.x+1)*4096-mid->x;
    u.v2=(xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+mid->u; u.ei=u.a*b.a+u.b;
    v.v2=(xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+mid->v; v.ei=v.a*b.a+v.b;
    u.v2=td->utbl[u.v2>>td->sh2&0xfff]+(u.v2<<8>>td->sh3&td->fbits);
    u.ei=td->utbl[u.ei>>td->sh2&0xfff]+(u.ei<<8>>td->sh3&td->fbits)|td->uomask;
    v.v2=td->vtbl[v.v2>>td->sh2&0xfff]+(v.v2<<8>>td->sh3&td->fbits);
    v.ei=td->vtbl[v.ei>>td->sh2&0xfff]+(v.ei<<8>>td->sh3&td->fbits)|td->vomask;
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>12)-(bot->y>>12);
  for (; y<0; y++) {
    u.v=u.v2; v.v=v.v2;
    x=b.p-e.p;
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      c =bits[(unsigned)(u.v+v.v)>>sh1];
      c2=bits[(unsigned)((u.v+u_one&uamask)+v.v)>>sh1];
      c3=bits[(unsigned)(u.v+(v.v+v_one&vamask))>>sh1];
      c4=bits[(unsigned)((u.v+u_one&uamask)+(v.v+v_one&vamask))>>sh1];
      {
      int xx=u.v>>sh1-9&255, yy=v.v>>sh1-9&255, bx0=biltbl[xx], bx1=biltbl[256-xx], by0=biltbl[yy], by1=biltbl[256-yy];
      d=epal2[((c4&0xf800)>>8)+bx0+by0]+epal2[((c3&0xf800)>>8)+bx1+by0]+epal2[((c2&0xf800)>>8)+bx0+by1]+epal2[((c &0xf800)>>8)+bx1+by1]&0x001ff800|
        epal2[((c4&0x07e0)>>3)+bx0+by0]+epal2[((c3&0x07e0)>>3)+bx1+by0]+epal2[((c2&0x07e0)>>3)+bx0+by1]+epal2[((c &0x07e0)>>3)+bx1+by1]&0xffe00000|
        epal2[((c4&0x001f)<<3)+bx0+by0]+epal2[((c3&0x001f)<<3)+bx1+by0]+epal2[((c2&0x001f)<<3)+bx0+by1]+epal2[((c &0x001f)<<3)+bx1+by1]&0x000003ff;
      e.p[x]=d>>5&0xf81f|d>>21&0x07e0;
      }
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}





void filltri_cthru(Tri2d *t) {
}



#define ip16(x, y, f) ((((x)&0xf81f)*(f)+((y)&0xf81f)*(64-(f))+0x18030>>6&0xf81f)+(((x)&0x07e0)*(f)+((y)&0x07e0)*(64-(f))+0x0600>>6&0x07e0))
#define ip32(x, y, f) ((((x)&0xff00ff)*(f)+((y)&0xff00ff)*(128-(f))+0x600060>>7&0xff00ff)+(((x)&0xff00)*(f)+((y)&0xff00)*(128-(f))+0x6000>>7&0xff00))

void drawline(Tri2d *t) {
  int x, y, dx, dy, a, b, k, kk;
  Vertex2d *v1, *v2;
  short *p;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*16-65536+(4096-(v1->x&0xfff))*(1/4096.0)*dy;
    for (x=v1->x>>12; x<v2->x>>12; x++) {
      p=(short*)dbuf+(y>>16)*pitchp2+x; k=(y+32768&65535)>>10;
      if ((unsigned)(y>>16)<lfb.height-1)
        ((short*)dbuf)[(y+32768>>16)*pitchp2+x]=ip16(p[0], p[pitchp2], k);
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*16-65536+(4096-(v1->y&0xfff))*(1/4096.0)*dx;
    for (y=v1->y>>12; y<v2->y>>12; y++) {
      p=(short*)dbuf+y*pitchp2+(x>>16); k=(x+32768&65535)>>10;
      if ((unsigned)(x>>16)<lfb.width-1)
        ((short*)dbuf)[y*pitchp2+(x+32768>>16)]=ip16(p[0], p[1], k);
      x+=dx;
    }
  }
}
void drawline_32(Tri2d *t) {
  int x, y, dx, dy, a, b, k, kk;
  Vertex2d *v1, *v2;
  int *p;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*16-65536+(4096-(v1->x&0xfff))*(1/4096.0)*dy;
    for (x=v1->x>>12; x<v2->x>>12; x++) {
      p=(int*)dbuf+(y>>16)*pitchp4+x; k=(y+32768&65535)>>9;
      if ((unsigned)(y>>16)<lfb.height-1)
        ((int*)dbuf)[(y+32768>>16)*pitchp4+x]=ip32(p[0], p[pitchp4], k);
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*16-65536+(4096-(v1->y&0xfff))*(1/4096.0)*dx;
    for (y=v1->y>>12; y<v2->y>>12; y++) {
      p=(int*)dbuf+y*pitchp4+(x>>16); k=(x+32768&65535)>>9;
      if ((unsigned)(x>>16)<lfb.width-1)
        ((int*)dbuf)[y*pitchp4+(x+32768>>16)]=ip32(p[0], p[1], k);
      x+=dx;
    }
  }
}

/*
void drawline(Tri2d *t) {
  int x, y, dx, dy, a, b, k;
  Vertex2d *v1, *v2;
  short *p;
  //if (nb_mask(t->top->x, 4096*321)|nb_mask(t->top->y, 4096*201)) return;
  //if (nb_mask(t->mid->x, 4096*321)|nb_mask(t->mid->y, 4096*201)) return;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*16-65536+(4096-(v1->x&0xfff))*(1/4096.0)*dy;
    for (x=v1->x>>12; x<v2->x>>12; x++) {
      p=dbuf+(y>>16)*pitchp2+x; k=(y&65535)>>13;
      a=p[0]; b=p[pitchp2];
      p[-3*pitchp2]=ip16(b, p[-3*pitchp2],  8-k);
      p[-2*pitchp2]=ip16(b, p[-2*pitchp2], 16-k);
      p[-1*pitchp2]=ip16(b, p[-1*pitchp2], 24-k);
      p[       0]=ip16(b, p[       0], 32-k);
      p[ 1*pitchp2]=ip16(a, p[ 1*pitchp2], 24+k);
      p[ 2*pitchp2]=ip16(a, p[ 2*pitchp2], 16+k);
      p[ 3*pitchp2]=ip16(a, p[ 3*pitchp2], 8+k);
      p[ 4*pitchp2]=ip16(a, p[ 4*pitchp2], k);
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*16-65536+(4096-(v1->y&0xfff))*(1/4096.0)*dx;
    for (y=v1->y>>12; y<v2->y>>12; y++) {
      p=dbuf+y*pitchp2+(x>>16); k=(x&65535)>>13;
      a=p[0]; b=p[1];
      p[-3]=ip16(b, p[-3],  8-k);
      p[-2]=ip16(b, p[-2], 16-k);
      p[-1]=ip16(b, p[-1], 24-k);
      p[ 0]=ip16(b, p[ 0], 32-k);
      p[ 1]=ip16(a, p[ 1], 24+k);
      p[ 2]=ip16(a, p[ 2], 16+k);
      p[ 3]=ip16(a, p[ 3], 8+k);
      p[ 4]=ip16(a, p[ 4], k);
      x+=dx;
    }
  }
}
*/
void portaltri(Tri2d *t) {
  //Space *tmp=curf;
  //space_draw((Space*)t->top, (Space*)t->mid);
  //frame_close((Space*)t->top);
  //triprep(tmp);
}




