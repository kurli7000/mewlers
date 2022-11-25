#include "functs.h"
#include "core.h"
#include "matter.pri"
#include "frame.pri"
#include "mesh.pri"
// 3224 3131 3132 3081 3233 2940 3129 2842 2789 2740 2363 2822 2813 2763
// 2243

static short *dbuf;
static Frame *curf;
int pitch;


static int exppal[65536+256];
#define epal2 (exppal+32768)
void triprep(Frame *f) {
  int i;
  curf=f; dbuf=f->dbuf; pitch=f->pitch;
  if (!exppal[65536]) {
    for (i=0; i<65536+256; i++) {
      int v;
      if (i<32768+256) v=255*fexp2((i-32768-256)*(6/256.0)); else v=255;
      exppal[i]=(v>>3<<11)+(v>>2<<5)+(v>>3); exppal[i]|=exppal[i]<<16;
      //exppal[i]=(v<<16)+(v<<8)+(v);
    }
  }
}



void filllst(Tri2d *x) { for (; x; x=x->next) x->m->fill(x); }

/*
void filltri(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y;
  static int mask, flag;
  char *d, *dp1, *dp2, *dp3;
  static char *txt;
  static struct {
    unsigned xf; union { char *p; int x; }; unsigned af; int a, a2;
  } b, e, *e1, *e2;
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  txt=t->m->txt1.data;
  f1=(mid->x-top->x)/(float)(mid->y-top->y);
  f2=(bot->x-top->x)/(float)(bot->y-top->y);
  f3=(bot->x-mid->x)/(float)(bot->y-mid->y);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=dbuf+(top->y>>12)*320;
  dp2=dbuf+(mid->y>>12)*320;
  dp3=dbuf+(bot->y>>12)*320;

  ala=4096.0/((mid->x-top->x)*(float)(bot->y-top->y)-(bot->x-top->x)*(float)(mid->y-top->y));

  yfix=4096-(top->y&0xfff);
  fistq(&e1->xf, (top->x+yfix*f1)*1048576.0); fistq(&e1->af, f1*4294967296.0); e1->a2=(e1->a+256&511)+64;
  fistq(&e2->xf, (top->x+yfix*f2)*1048576.0); fistq(&e2->af, f2*4294967296.0); e2->a2=(e2->a+256&511)+64;
  xfix=(b.x+1)*4096-top->x;
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>12)-(mid->y>>12);

  for (; y<0; y++) {
    for (d=b.p; d<e.p; d++) *d=t->c;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->xf, (mid->x+yfix*f3)*1048576.0); fistq(&e1->af, f3*4294967296.0); e1->a2=(e1->a+256&511)+64;
  e1->p=dp2+e1->x;
  y=(mid->y>>12)-(bot->y>>12);
  for (; y<0; y++) {
    for (d=b.p; d<e.p; d++) *d=t->c;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}
*/


/*
void zfilltri(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y;
  static int mask, flag;
  char *d, *dp1, *dp2, *dp3;
  unsigned short *zp;
  static char *txt;
  static struct {
    unsigned xf; union { char *p; int x; }; unsigned af; int a, a2;
  } b, e, *e1, *e2;
  static struct {
    union { int v; unsigned w; };
    union { int v2; unsigned w2; };
    int a, b, xi, ei;
  } u, v, z;
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  void inneri(int u, int v, unsigned z, unsigned short *zp, char *dp);

  txt=t->m->txt1.data;
  f1=(mid->x-top->x)/(float)(mid->y-top->y);
  f2=(bot->x-top->x)/(float)(bot->y-top->y);
  f3=(bot->x-mid->x)/(float)(bot->y-mid->y);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=dbuf+(top->y>>12)*320;
  dp2=dbuf+(mid->y>>12)*320;
  dp3=dbuf+(bot->y>>12)*320;

  ala=4096.0/((mid->x-top->x)*(float)(bot->y-top->y)-(bot->x-top->x)*(float)(mid->y-top->y));
  u.a=inear(((mid->u-top->u)*(float)(bot->y-top->y)-(bot->u-top->u)*(float)(mid->y-top->y))*ala);
  u.b=inear(((bot->u-top->u)*(float)(mid->x-top->x)-(mid->u-top->u)*(float)(bot->x-top->x))*ala);
  u.xi=uaddi(u.a);
  v.a=inear(((mid->v-top->v)*(float)(bot->y-top->y)-(bot->v-top->v)*(float)(mid->y-top->y))*ala);
  v.b=inear(((bot->v-top->v)*(float)(mid->x-top->x)-(mid->v-top->v)*(float)(bot->x-top->x))*ala);
  v.xi=vaddi(v.a);
  z.a=inear(((mid->z-top->z)*(float)(bot->y-top->y)-(bot->z-top->z)*(float)(mid->y-top->y))*ala);
  z.b=inear(((bot->z-top->z)*(float)(mid->x-top->x)-(mid->z-top->z)*(float)(bot->x-top->x))*ala);
  z.xi=z.a;

  yfix=4096-(top->y&0xfff);
  fistq(&e1->xf, (top->x+(yfix&mask_ne((int)dp1, (int)dp2))*f1)*1048576.0); fistq(&e1->af, f1*4294967296.0); e1->a2=(e1->a+256&511)+64;
  fistq(&e2->xf, (top->x+(yfix&mask_ne((int)dp1, (int)dp3))*f2)*1048576.0); fistq(&e2->af, f2*4294967296.0); e2->a2=(e2->a+256&511)+64;
  xfix=(b.x+1)*4096-top->x;
  u.v2=ukonvi((xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+top->u); u.ei=uaddi(u.a*b.a+u.b);
  v.v2=vkonvi((xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+top->v); v.ei=vaddi(v.a*b.a+v.b);
  z.w2=      ((xfix*(z.a>>8)+yfix*(z.b>>8)>>4)+top->z); z.ei=     (z.a*b.a+z.b);
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  y=(top->y>>12)-(mid->y>>12);

  #pragma aux inneri=\
    "cmp edi, dword ptr[e+4]"\
    "jnb l3"\
    "l1:"\
      "and ebx, 0555557ffh"\
      "and ecx, 0aaaaa7ffh"\
      "mov ebp, ebx"\
      "mov eax, edx"\
      "shr eax, 12"\
      "add ebp, ecx"\
      "shr ebp, 12"\
      "add ebx, dword ptr[u+16]"\
      "add ecx, dword ptr[v+16]"\
      "add ebp, [txt]"\
      "cmp ax, word ptr[esi]"\
      "jb l2"\
        "mov word ptr[esi], ax"\
        "mov al, byte ptr[ebp]"\
        "mov [edi], al"\
      "l2:"\
      "add edx, dword ptr[z+16]"\
      "inc edi"\
      "add esi, 2"\
      "cmp edi, dword ptr[e+4]"\
    "jb l1"\
    "l3:"\
    parm [ebx][ecx][edx][esi][edi] modify exact [eax ebx ecx edx esi edi ebp];

  for (; y<0; y++) {
    if (b.p<e.p) inneri(u.v2, v.v2, z.w2, b.p-dbuf+zbuf, b.p);
    //u.v=u.v2; v.v=v.v2; z.w=z.w2;
    //for (d=b.p, zp=d-dbuf+zbuf; d<e.p; d++, zp++) {
      //u.v&=td->uamask; v.v&=td->vamask;
      //if (*zp<(z.w>>12&0xffff)) {
        //*zp=z.w>>12&0xffff;
        //*d=txt[(unsigned)(u.v+v.v)>>12];
      //}
      //u.v+=u.xi; v.v+=v.xi; z.w+=z.xi;
    //}
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=td->uamask;
    v.v2+=v.xi&mask; v.v2&=td->vamask;
    z.w2+=(z.xi&mask)+z.ei;
    u.v2+=u.ei; u.v2&=td->uamask;
    v.v2+=v.ei; v.v2&=td->vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->xf, (mid->x+(yfix&mask_ne((int)dp2, (int)dp3))*f3)*1048576.0); fistq(&e1->af, f3*4294967296.0); e1->a2=(e1->a+256&511)+64;
  if (flag) {
    xfix=(b.x+1)*4096-mid->x;
    u.v2=ukonvi((xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+mid->u); u.ei=uaddi(u.a*b.a+u.b);
    v.v2=vkonvi((xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+mid->v); v.ei=vaddi(v.a*b.a+v.b);
    z.w2=      ((xfix*(z.a>>8)+yfix*(z.b>>8)>>4)+mid->z); z.ei=     (z.a*b.a+z.b);
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>12)-(bot->y>>12);
  for (; y<0; y++) {
    if (b.p<e.p) inneri(u.v2, v.v2, z.w2, b.p-dbuf+zbuf, b.p);
    //u.v=u.v2; v.v=v.v2; z.w=z.w2;
    //for (d=b.p, zp=d-dbuf+zbuf; d<e.p; d++, zp++) {
      //u.v&=td->uamask; v.v&=td->vamask;
      //if (*zp<(z.w>>12&0xffff)) {
        //*zp=z.w>>12&0xffff;
        //*d=txt[(unsigned)(u.v+v.v)>>12];
      //}
      //u.v+=u.xi; v.v+=v.xi; z.w+=z.xi;
    //}
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=td->uamask;
    v.v2+=v.xi&mask; v.v2&=td->vamask;
    z.w2+=(z.xi&mask)+z.ei;
    u.v2+=u.ei; u.v2&=td->uamask;
    v.v2+=v.ei; v.v2&=td->vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}
*/





void filltri(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c;
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
  dp1=dbuf+(top->y>>12)*pitch;
  dp2=dbuf+(mid->y>>12)*pitch;
  dp3=dbuf+(bot->y>>12)*pitch;

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
  fistq(&e1->x64, (top->x+yfix*f1)*1048576.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitch-256;
  fistq(&e2->x64, (top->x+yfix*f2)*1048576.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitch-256;
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
    if ((x=b.p-e.p)&1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x]=(epal2[(c&0xf800)>>8]&0x00ff0000|
              //epal2[(c&0x07e0)>>3]&0x0000ff00|
              //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      //e.p[x  ]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x  ]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x  ]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      //e.p[x+1]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x+1]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x+1]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->x64, (mid->x+yfix*f3)*1048576.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitch-256;
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
    if ((x=b.p-e.p)&1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x]=(epal2[(c&0xf800)>>8]&0x00ff0000|
              //epal2[(c&0x07e0)>>3]&0x0000ff00|
              //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      //e.p[x  ]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x  ]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x  ]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      //e.p[x+1]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x+1]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x+1]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}



void filltri_cthru(Tri2d *t) {
/*  static float f1, f2, f3, ala;
  int xfix, yfix, y, x;
  static int mask, flag, tmp;
  char *dp1, *dp2, *dp3;
  static char *bits;
  static Texture *txt;
  static Tiledata *td;
  static int uamask, vamask, sh1;
  static struct {
    union { struct { unsigned xf; union { char *p; int x; }; }; __int64 x64; };
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
  dp1=dbuf+(top->y>>12)*320;
  dp2=dbuf+(mid->y>>12)*320;
  dp3=dbuf+(bot->y>>12)*320;


  ala=4096.0/(mtx*bty-btx*mty);
  u.a=inear(((mid->u-top->u)*bty-(bot->u-top->u)*mty)*ala);
  u.b=inear(((bot->u-top->u)*mtx-(mid->u-top->u)*btx)*ala);
  v.a=inear(((mid->v-top->v)*bty-(bot->v-top->v)*mty)*ala);
  v.b=inear(((bot->v-top->v)*mtx-(mid->v-top->v)*btx)*ala);
  tmp=(u.a^u.a>>31)+(u.b^u.b>>31)+(v.a^v.a>>31)+(v.b^v.b>>31);
  txt=&t->m->txt1;
  while (txt->mipnext && tmp>txt->td->raja) txt=txt->mipnext;
  td=txt->td; bits=txt->bits8p; uamask=td->uamask; vamask=td->vamask; sh1=td->sh1;
  u.xi=td->utbl[u.a>>td->sh2&0xfff]+(u.a<<8>>td->sh3&td->fbits)|td->uomask;
  v.xi=td->vtbl[v.a>>td->sh2&0xfff]+(v.a<<8>>td->sh3&td->fbits)|td->vomask;

  yfix=4096-(top->y&0xfff);
  fistq(&e1->x64, (top->x+yfix*f1)*1048576.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+64;
  fistq(&e2->x64, (top->x+yfix*f2)*1048576.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+64;
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
    if ((x=b.p-e.p)&1) {
      u.v&=uamask; v.v&=vamask;
      e.p[x]=e.p[x]+bits[(unsigned)(u.v+v.v)>>sh1]>>2;
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      e.p[x  ]=e.p[x  ]+bits[(unsigned)(u.v+v.v)>>sh1]>>2;
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      e.p[x+1]=e.p[x+1]+bits[(unsigned)(u.v+v.v)>>sh1]>>2;
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->x64, (mid->x+yfix*f3)*1048576.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+64;
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
    if ((x=b.p-e.p)&1) {
      u.v&=uamask; v.v&=vamask;
      e.p[x]=e.p[x]+bits[(unsigned)(u.v+v.v)>>sh1]>>2;
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      e.p[x  ]=e.p[x  ]+bits[(unsigned)(u.v+v.v)>>sh1]>>2;
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      e.p[x+1]=e.p[x+1]+bits[(unsigned)(u.v+v.v)>>sh1]>>2;
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }*/
}


void filltri_phlat(Tri2d *t) {
  static float f1, f2, f3, ala;
  int xfix, yfix, y, x, c;
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
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
  float mtx=mid->x-top->x, btx=bot->x-top->x,
        mty=mid->y-top->y, bty=bot->y-top->y;
  f1=mtx/mty;
  f2=btx/bty;
  f3=(btx-mtx)/(bty-mty);
  if (flag=(f1<f2)) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=dbuf+(top->y>>12)*pitch;
  dp2=dbuf+(mid->y>>12)*pitch;
  dp3=dbuf+(bot->y>>12)*pitch;


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
  fistq(&e1->x64, (top->x+yfix*f1)*1048576.0); fistq(&e1->a64, f1*4294967296.0); e1->a2=(e1->a+256&511)+pitch-256;
  fistq(&e2->x64, (top->x+yfix*f2)*1048576.0); fistq(&e2->a64, f2*4294967296.0); e2->a2=(e2->a+256&511)+pitch-256;
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
    if ((x=b.p-e.p)&1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x]=(epal2[(c&0xf800)>>8]&0x00ff0000|
              //epal2[(c&0x07e0)>>3]&0x0000ff00|
              //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      //e.p[x  ]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x  ]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x  ]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      //e.p[x+1]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x+1]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x+1]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->x64, (mid->x+yfix*f3)*1048576.0); fistq(&e1->a64, f3*4294967296.0); e1->a2=(e1->a+256&511)+pitch-256;
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
    if ((x=b.p-e.p)&1) {
      u.v&=uamask; v.v&=vamask;
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x]=(epal2[(c&0xf800)>>8]&0xf800f800|
              epal2[(c&0x07e0)>>3]&0x07e007e0|
              epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x]=(epal2[(c&0xf800)>>8]&0x00ff0000|
              //epal2[(c&0x07e0)>>3]&0x0000ff00|
              //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x++;
    }
    while (x<0) {
      u.v&=uamask; v.v&=vamask;
      //e.p[x  ]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x  ]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x  ]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      u.v&=uamask; v.v&=vamask;
      //e.p[x+1]=bits[(unsigned)(u.v+v.v)>>sh1];
      c=bits[(unsigned)(u.v+v.v)>>sh1];
      e.p[x+1]=(epal2[(c&0xf800)>>8]&0xf800f800|
                epal2[(c&0x07e0)>>3]&0x07e007e0|
                epal2[(c&0x001f)<<3]&0x001f001f)>>16;
      //e.p[x+1]=(epal2[(c&0xf800)>>8]&0x00ff0000|
                //epal2[(c&0x07e0)>>3]&0x0000ff00|
                //epal2[(c&0x001f)<<3]&0x000000ff);
      u.v+=u.xi; v.v+=v.xi;
      x+=2;
    }
    if (*(volatile char*)1047&16) *b.p=0;
    mask=add_mask(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=uamask;
    v.v2+=v.xi&mask; v.v2&=vamask;
    u.v2+=u.ei; u.v2&=uamask;
    v.v2+=v.ei; v.v2&=vamask;
    mask=add_mask(&e.xf, e.af); e.p+=e.a2-mask;
  }
}



#define ip16(x, y, f) ((((x)&0xf81f)*(f)+((y)&0xf81f)*(64-(f))+0x18030>>6&0xf81f)+(((x)&0x07e0)*(f)+((y)&0x07e0)*(64-(f))+0x0600>>6&0x07e0))

void drawline(Tri2d *t) {
  int x, y, dx, dy, a, b, k, kk;
  Vertex2d *v1, *v2;
  short *p;
  if (nb_mask(t->top->x, 4096*321)|nb_mask(t->top->y, 4096*201)) return;
  if (nb_mask(t->mid->x, 4096*321)|nb_mask(t->mid->y, 4096*201)) return;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*16-65536+(4096-(v1->x&0xfff))*(1/4096.0)*dy;
    for (x=v1->x>>12; x<v2->x>>12; x++) {
      p=dbuf+(y>>16)*pitch+x; k=(y+32768&65535)>>10;
      dbuf[(y+32768>>16)*pitch+x]=ip16(p[0], p[pitch], k);
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*16-65536+(4096-(v1->y&0xfff))*(1/4096.0)*dx;
    for (y=v1->y>>12; y<v2->y>>12; y++) {
      p=dbuf+y*pitch+(x>>16); k=(x+32768&65535)>>10;
      dbuf[y*pitch+(x+32768>>16)]=ip16(p[0], p[1], k);
      x+=dx;
    }
  }
}
/*
void drawline(Tri2d *t) {
  int x, y, dx, dy, a, b, k;
  Vertex2d *v1, *v2;
  short *p;
  if (nb_mask(t->top->x, 4096*321)|nb_mask(t->top->y, 4096*201)) return;
  if (nb_mask(t->mid->x, 4096*321)|nb_mask(t->mid->y, 4096*201)) return;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*16-65536+(4096-(v1->x&0xfff))*(1/4096.0)*dy;
    for (x=v1->x>>12; x<v2->x>>12; x++) {
      p=dbuf+(y>>16)*pitch+x; k=(y&65535)>>13;
      a=p[0]; b=p[pitch];
      p[-960]=ip16(b, p[-960],  8-k);
      p[-640]=ip16(b, p[-640], 16-k);
      p[-320]=ip16(b, p[-320], 24-k);
      p[   0]=ip16(b, p[   0], 32-k);
      p[ 320]=ip16(a, p[ 320], 24+k);
      p[ 640]=ip16(a, p[ 640], 16+k);
      p[ 960]=ip16(a, p[ 960], 8+k);
      p[1280]=ip16(a, p[1280], k);
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*16-65536+(4096-(v1->y&0xfff))*(1/4096.0)*dx;
    for (y=v1->y>>12; y<v2->y>>12; y++) {
      p=dbuf+y*320+(x>>16); k=(x&65535)>>13;
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
  Frame *tmp=curf;
  space_draw((Frame*)t->top, (Space*)t->mid);
  frame_close((Frame*)t->top);
  triprep(tmp);
}

/*
void drawline(Tri2d *t) {
  int x, y, xe, ye, dx, dy, a, b;
  Vertex2d *v1, *v2;
  char *p;
  if (nb_mask(t->top->x, 4096*321)|nb_mask(t->top->y, 4096*201)) return;
  if (nb_mask(t->mid->x, 4096*321)|nb_mask(t->mid->y, 4096*201)) return;
  dx=t->mid->x-t->top->x; dy=t->mid->y-t->top->y;
  if ((dx>0?dx:-dx)>(dy>0?dy:-dy)) {
    if (dx<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dy=dy*65536.0/dx; y=v1->y*16-65536+(4096-(v1->x&0xfff))*(1/4096.0)*dy;
    for (x=v1->x>>12; x<v2->x>>12; x++) {
      p=dbuf+(y>>16)*320+x; a=p[0]&127; b=p[320]&127;
      p[0]=128+b; p[320]=128+a;
      y+=dy;
    }
  } else {
    if (dy<0) v1=t->mid, v2=t->top; else v1=t->top, v2=t->mid;
    dx=dx*65536.0/dy; x=v1->x*16-65536+(4096-(v1->y&0xfff))*(1/4096.0)*dx;
    for (y=v1->y>>12; y<v2->y>>12; y++) {
      p=dbuf+y*320+(x>>16); a=p[0]&127; b=p[1]&127;
      p[0]=128+b; p[1]=128+a;
      x+=dx;
    }
  }
}
*/



