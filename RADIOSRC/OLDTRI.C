#include "functs.h"
#include "core.h"

static char *dbuf;
void triprep(char *buf) { dbuf=buf; }
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

int adc(unsigned int *a, unsigned int b);
#pragma aux adc="adc [ebx],eax" "sbb eax,eax" parm[ebx][eax] value[eax] modify exact[eax];

//690 736 780 737 734 652
void filltri(Tri2d *t) {
  float f1, f2, f3, ala;
  int xfix, yfix;
  int mask, flag;
  char *d, *dp1, *dp2, *dp3;
  char *txt=t->m->txt1.data;
  struct {
    unsigned xf; union { char *p; int x; }; unsigned af; int a, a2;
  } b, e, *e1, *e2;
  struct {
    int v, mid, a, b, xi, yi, ei;
  } u, v;
  Vertex2d *top=t->top, *mid=t->mid, *bot=t->bot;
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
  u.xi=uaddi(u.a); u.yi=uaddi(u.b);
  v.a=inear(((mid->v-top->v)*(float)(bot->y-top->y)-(bot->v-top->v)*(float)(mid->y-top->y))*ala);
  v.b=inear(((bot->v-top->v)*(float)(mid->x-top->x)-(mid->v-top->v)*(float)(bot->x-top->x))*ala);
  v.xi=vaddi(v.a); v.yi=vaddi(v.b);

  yfix=-top->y&0xfff&-(dp1!=dp2);
  fistq(&e1->xf, (top->x+yfix*f1)*1048576.0); fistq(&e1->af, f1*4294967296.0); e1->a2=(e1->a+32&63)+288;
  fistq(&e2->xf, (top->x+yfix*f2)*1048576.0); fistq(&e2->af, f2*4294967296.0); e2->a2=(e2->a+32&63)+288;
  xfix=b.x*4096-top->x;
  u.mid=ukonvi((xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+top->u); u.ei=uaddi(u.a*b.a+u.b);
  v.mid=vkonvi((xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+top->v); v.ei=vaddi(v.a*b.a+v.b);
  e1->p=dp1+e1->x; e2->p=dp1+e2->x;
  while (e.p<dp2) {
    u.v=u.mid; v.v=v.mid;
    for (d=b.p+1; d<e.p; d++) {
      *d=txt[(unsigned)((u.v&=0x555557ff)+(v.v&=0xaaaaa7ff))>>12]; u.v+=u.xi; v.v+=v.xi;
    }
    mask=adc(&b.xf, b.af); b.p+=b.a2-mask;
    u.mid+=u.xi&mask; u.mid&=0x555557ff;
    v.mid+=v.xi&mask; v.mid&=0xaaaaa7ff;
    u.mid+=u.ei; u.mid&=0x555557ff;
    v.mid+=v.ei; v.mid&=0xaaaaa7ff;
    mask=adc(&e.xf, e.af); e.p+=e.a2-mask;
  }
  yfix=-mid->y&0xfff&-(dp2!=dp3);
  fistq(&e1->xf, (mid->x+yfix*f3)*1048576.0); fistq(&e1->af, f3*4294967296.0); e1->a2=(e1->a+32&63)+288;
  if (flag) {
    xfix=b.x*4096-mid->x;
    u.mid=ukonvi((xfix*(u.a>>4)+yfix*(u.b>>4)>>8)+mid->u); u.ei=uaddi(u.a*b.a+u.b);
    v.mid=vkonvi((xfix*(v.a>>4)+yfix*(v.b>>4)>>8)+mid->v); v.ei=vaddi(v.a*b.a+v.b);
  }
  e1->p=dp2+e1->x;
  while (e.p<dp3) {
    u.v=u.mid; v.v=v.mid;
    for (d=b.p+1; d<e.p; d++) {
      *d=txt[(unsigned)((u.v&=0x555557ff)+(v.v&=0xaaaaa7ff))>>12]; u.v+=u.xi; v.v+=v.xi;
    }
    mask=adc(&b.xf, b.af); b.p+=b.a2-mask;
    u.mid+=u.xi&mask; u.mid&=0x555557ff;
    v.mid+=v.xi&mask; v.mid&=0xaaaaa7ff;
    u.mid+=u.ei; u.mid&=0x555557ff;
    v.mid+=v.ei; v.mid&=0xaaaaa7ff;
    mask=adc(&e.xf, e.af); e.p+=e.a2-mask;
  }
}


