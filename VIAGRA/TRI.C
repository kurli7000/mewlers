#include "functs.h"
#include "core.h"

#undef uaddi
#undef vaddi
#define uaddi(u) (ugonvi(u)|0xaaaaa800)
#define vaddi(v) (vgonvi(v)|0x55555800)
static int ugonvi(int u) {
  return (u>>3&0x000007ff)|(u>>2&0x00001000)|(u>>1&0x00004000)|(u   &0x00010000)
        |(u<<1&0x00040000)|(u<<2&0x00100000)|(u<<3&0x00400000)|(u<<4&0x01000000)
        |(u<<5&0x04000000)|(u<<6&0x10000000)|(u<<7&0x40000000);
}
static int vgonvi(int v) {
  return (v>>3&0x000007ff)|(v>>1&0x00002000)|(v   &0x00008000)|(v<<1&0x00020000)
        |(v<<2&0x00080000)|(v<<3&0x00200000)|(v<<4&0x00800000)|(v<<5&0x02000000)
        |(v<<6&0x08000000)|(v<<7&0x20000000)|(v<<8&0x80000000);
}




static char *dbuf;
static unsigned short *zbuf;
void triprep(char *buf, unsigned short *_zbuf) { dbuf=buf; zbuf=_zbuf; }
/*
void filltri(Vertex2d *v1, Vertex2d *v2, Vertex2d *v3) {
  int flag;
  float f1, f2, f3, tmp;
  char *d, *dp1, *dp2, *dp3;
  struct {
    char *x; unsigned xf; int a; unsigned af;
  } b, e, *e1, *e2;
  f1=(v2->x-v1->x)/(float)(v2->y-v1->y);
  f2=(v3->x-v1->x)/(float)(v3->y-v1->y);
  f3=(v3->x-v2->x)/(float)(v3->y-v2->y);
  if (f1<f2) e1=&b, e2=&e; else e1=&e, e2=&b;
  dp1=dbuf+(v1->y>>12)*320;
  dp2=dbuf+(v2->y>>12)*320;
  dp3=dbuf+(v3->y>>12)*320;
  tmp=v1->x+(4096-(v1->y&0xfff))*f1; e1->x=dp1+ifloor(tmp*(1/4096.0)); e1->xf=tmp*16.0*65536.0; e1->a=320+ifloor(f1); e1->af=f1*4294967296.0;
  tmp=v1->x+(4096-(v1->y&0xfff))*f2; e2->x=dp1+ifloor(tmp*(1/4096.0)); e2->xf=tmp*16.0*65536.0; e2->a=320+ifloor(f2); e2->af=f2*4294967296.0;
  while (e.x<dp2) {
    for (d=b.x; d<e.x; d++) *d=15;
    b.x+=b.a; if (-b.af<b.xf) { b.x++; } b.xf+=b.af;
    e.x+=e.a; if (-e.af<e.xf) { e.x++; } e.xf+=e.af;
  }
  tmp=v2->x+(4096-(v2->y&0xfff))*f3; e1->x=dp2+ifloor(tmp*(1/4096.0)); e1->xf=tmp*16.0*65536.0; e1->a=320+ifloor(f3); e1->af=f3*4294967296.0;
  while (e.x<dp3) {
    for (d=b.x; d<e.x; d++) *d=15;
    b.x+=b.a; if (-b.af<b.xf) { b.x++; } b.xf+=b.af;
    e.x+=e.a; if (-e.af<e.xf) { e.x++; } e.xf+=e.af;
  }
}
*/


// 3224 3131 3132 3081 3233 2940 3129 2842 2789 2740 2363 2822 2813 2763
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
  u.v2=ugonvi((xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+top->u); u.ei=uaddi(u.a*b.a+u.b);
  v.v2=vgonvi((xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+top->v); v.ei=vaddi(v.a*b.a+v.b);
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

/*    u.v=u.v2; v.v=v.v2; z.w=z.w2;
    for (d=b.p, zp=d-dbuf+zbuf; d<e.p; d++, zp++) {
      u.v&=0x555557ff; v.v&=0xaaaaa7ff;
      if (*zp<(z.w>>12&0xffff)) {
        *zp=z.w>>12&0xffff;
        *d=txt[(unsigned)(u.v+v.v)>>12];
      }
      u.v+=u.xi; v.v+=v.xi; z.w+=z.xi;
    } */

    mask=mask_adc(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=0x555557ff;
    v.v2+=v.xi&mask; v.v2&=0xaaaaa7ff;
    z.w2+=(z.xi&mask)+z.ei;
    u.v2+=u.ei; u.v2&=0x555557ff;
    v.v2+=v.ei; v.v2&=0xaaaaa7ff;
    mask=mask_adc(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=4096-(mid->y&0xfff);
  fistq(&e1->xf, (mid->x+(yfix&mask_ne((int)dp2, (int)dp3))*f3)*1048576.0); fistq(&e1->af, f3*4294967296.0); e1->a2=(e1->a+256&511)+64;
  if (flag) {
    xfix=(b.x+1)*4096-mid->x;
    u.v2=ugonvi((xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+mid->u); u.ei=uaddi(u.a*b.a+u.b);
    v.v2=vgonvi((xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+mid->v); v.ei=vaddi(v.a*b.a+v.b);
    z.w2=      ((xfix*(z.a>>8)+yfix*(z.b>>8)>>4)+mid->z); z.ei=     (z.a*b.a+z.b);
  }
  e1->p=dp2+e1->x;
  y=(mid->y>>12)-(bot->y>>12);
  for (; y<0; y++) {

    if (b.p<e.p) inneri(u.v2, v.v2, z.w2, b.p-dbuf+zbuf, b.p);
/*
    u.v=u.v2; v.v=v.v2; z.w=z.w2;
    for (d=b.p, zp=d-dbuf+zbuf; d<e.p; d++, zp++) {
      u.v&=0x555557ff; v.v&=0xaaaaa7ff;
      if (*zp<(z.w>>12&0xffff)) {
        *zp=z.w>>12&0xffff;
        *d=txt[(unsigned)(u.v+v.v)>>12];
      }
      u.v+=u.xi; v.v+=v.xi; z.w+=z.xi;
    }*/

    mask=mask_adc(&b.xf, b.af); b.p+=b.a2-mask;
    u.v2+=u.xi&mask; u.v2&=0x555557ff;
    v.v2+=v.xi&mask; v.v2&=0xaaaaa7ff;
    z.w2+=(z.xi&mask)+z.ei;
    u.v2+=u.ei; u.v2&=0x555557ff;
    v.v2+=v.ei; v.v2&=0xaaaaa7ff;
    mask=mask_adc(&e.xf, e.af); e.p+=e.a2-mask;
  }
}


