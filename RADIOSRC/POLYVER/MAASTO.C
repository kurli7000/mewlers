#include <stdlib.h>
#include "core.h"
#include "obuja.h"
#include "mesh.pri"
#include "mesh.h"

static Lfb buf;
static Mesh *o;

typedef struct _Plink Plink;
struct _Plink {
  Poly3d *p;
  Plink *next;
};

#define wid 512.0
#define hgt 512.0
float kerroin, inkki;

int xrand() {
  static int a=0x93a242c3, b=0x46f56327, c=0x24034d3b;
  a=a*0x3372f6c1+1;
  b=b*0x76a8632f+1^(a>>16);
  c=c*0x14d23839+1+(b>>16);
  return c>>16&0x7fff;
}
#define res 10
#define geth(x, z) (buf.buf[(int)(((z)*(1/hgt)+0.5)*buf.height)*buf.pitch+(int)(((x)*(1/wid)+0.5)*buf.width)]*kerroin+inkki)
static float plypts(Vector *v1, Vector *v2, Vector *v3) {
  Vector a, b, c, d;
  float x, y, z, e, f, g;
  int u, v;
  v_sub(&a, v2, v1);
  v_sub(&b, v3, v1);
  v_sub(&c, v3, v2);
  e=0.1;
  for (u=0; u<10; u++) for (v=0; v<u; v++) {
    f=(u+xrand()*(1/65536.0))*(1/10.0);
    g=(v+xrand()*(1/65536.0))*(1/10.0);
    x=v1->x+f*a.x+g*b.x;
    y=v1->y+f*a.y+g*b.y;
    z=v1->z+f*a.z+g*b.z;
    f=fsqr(y-geth(x, z)); if (f>1) e+=f;
  }
  v_norm(&a); v_norm(&b); v_norm(&c);
  x=ffabs(v_dot(&a, &b));
  y=ffabs(v_dot(&b, &c));
  z=ffabs(v_dot(&c, &a));
  if (y>x) x=y; if (z>x) x=z;
  if (x>0.95) return 1.0e20;
  return e/fsqrt(1-x);
}
static float vpts(Vertex3d *v) {
  Plink *p;
  float x=0.0;
  for (p=(Plink*)v->v2d; p; p=p->next) x+=plypts(&p->p->vl[0]->pos, &p->p->vl[1]->pos, &p->p->vl[2]->pos);
  return x;
}
void vopt1(Vertex3d *v) {
  float p=vpts(v), q;
  int i;
  q=p*0.999;
  for (i=0; i<10; i++) {
    Vector a=v->pos;
    v->pos.x+=(xrand()-xrand())*0.00002;
    v->pos.y+=(xrand()-xrand())*0.00001;
    v->pos.z+=(xrand()-xrand())*0.00002; //v->pos.y=geth(v->pos.x, v->pos.z);
    p=vpts(v);
    if (p>q) v->pos=a; else q=p*0.999;
  }
}
/*void vopt2(int x, int y) {
  Vertex3d *v=o->vl[y*res+x];
  float p=vpts(x, y), q;
  int i;
  q=p*0.999-0.01; if (q<0.0) return;
  for (i=0; i<10; i++) {
    Vector a=v->pos;
    v->pos.x+=(xrand()-xrand())*0.00001;
    v->pos.y+=(xrand()-xrand())*0.00001;
    v->pos.z+=(xrand()-xrand())*0.00001; //v->pos.y=geth(v->pos.x, v->pos.z);
    p=vpts(x, y);
    if (p>q) v->pos=a;
      else { q=p*0.999-0.01; if (q<0.0) return; }
  }
}*/
static Vector *normal(Vector *d, Vector *a, Vector *b, Vector *c) {
  Vector v1, v2;
  return v_norm(v_cross(d, v_sub(&v1, a, b), v_sub(&v2, a, c)));
}
static float polysize(Vector *a, Vector *b, Vector *c) {
  Vector v1, v2, v3;
  return fsqrt(v_dot(v_cross(&v3, v_sub(&v1, a, b), v_sub(&v2, a, c)), &v3));
}
static float divpts(Edge3d *e, float r) {
  int i;
  Vector v;
  Vertex3d *v1, *v2;
  if (!e->p1||!e->p2||e->p1->flag||e->p2->flag) return -1;
  v.x=e->v1->pos.x*r+e->v2->pos.x*(1-r);
  v.z=e->v1->pos.z*r+e->v2->pos.z*(1-r);
  v.y=/*(e->v1->pos.y+e->v2->pos.y)*0.5;*/geth(v.x, v.z);
  for (i=0; i<3; i++) if (e->p1->vl[i]!=e->v1 && e->p1->vl[i]!=e->v2) v1=e->p1->vl[i];
  for (i=0; i<3; i++) if (e->p2->vl[i]!=e->v1 && e->p2->vl[i]!=e->v2) v2=e->p2->vl[i];
  return (
     (plypts(&e->v1->pos, &e->v2->pos, &v1->pos)+
      plypts(&e->v1->pos, &e->v2->pos, &v2->pos))*3
     -plypts(&v, &e->v1->pos, &v1->pos)-plypts(&v, &e->v1->pos, &v2->pos)
     -plypts(&v, &e->v2->pos, &v1->pos)-plypts(&v, &e->v2->pos, &v2->pos)
   )*(
     polysize(&e->v1->pos, &e->v2->pos, &v1->pos)+
     polysize(&e->v1->pos, &e->v2->pos, &v2->pos)
   );
}
static void divide(Edge3d *e, float r) {
  int i;
  float x, z;
  Poly3d *p;
  if (!e->p1||!e->p2||e->p1->flag||e->p2->flag) return;
  x=e->v1->pos.x*r+e->v2->pos.x*(1-r);
  z=e->v1->pos.z*r+e->v2->pos.z*(1-r);
  mesh_v_set(o, o->vc, x, geth(x, z), z, 0, 0);
  e->p1->flag=e->p2->flag=1;
  p=o->pl[o->pc++]=getmem(sizeof(Poly3d)+sizeof(Vertex3d*)*3);
  movsb(p, e->p1, sizeof(Poly3d)+sizeof(Vertex3d*)*3);
  for (i=0; i<3; i++) if (p->vl[i]==e->v2) p->vl[i]=o->vl[o->vc];
  p=o->pl[o->pc++]=getmem(sizeof(Poly3d)+sizeof(Vertex3d*)*3);
  movsb(p, e->p2, sizeof(Poly3d)+sizeof(Vertex3d*)*3);
  for (i=0; i<3; i++) if (p->vl[i]==e->v2) p->vl[i]=o->vl[o->vc];
  for (i=0; i<3; i++) if (e->p1->vl[i]==e->v1) e->p1->vl[i]=o->vl[o->vc];
  for (i=0; i<3; i++) if (e->p2->vl[i]==e->v1) e->p2->vl[i]=o->vl[o->vc];
  o->vc++;
}
static void subdiv1() {
  float pts, bpts=0;
  float r, br;
  int i, j;
  Edge3d *e, *be=0;
  for (i=0; i<100; i++) {
    e=o->el[xrand()%o->ec];
    pts=divpts(e, 0.5); if (pts>bpts) bpts=pts, be=e;
  }
  if (be) {
    bpts=0; br=0;
    for (r=0.2; r<0.8; r+=0.03) {
      pts=divpts(be, r); if (pts>bpts) bpts=pts, br=r;
    }
    if (br) divide(be, br);
  }
}

static void turn1(Edge3d *e) {
  int i;
  Vector w1, w2;
  Vertex3d *v1=0, *v2=0;
  if (!e->p1||!e->p2||e->p1->flag||e->p2->flag) return;
  for (i=0; i<3; i++) if (e->p1->vl[i]!=e->v1 && e->p1->vl[i]!=e->v2) v1=e->p1->vl[i];
  for (i=0; i<3; i++) if (e->p2->vl[i]!=e->v1 && e->p2->vl[i]!=e->v2) v2=e->p2->vl[i];
  if (plypts(&e->v1->pos, &e->v2->pos, &v1->pos)
     +plypts(&e->v1->pos, &e->v2->pos, &v2->pos)>
      plypts(&v1->pos, &v2->pos, &e->v1->pos)
     +plypts(&v1->pos, &v2->pos, &e->v2->pos)) {
    for (i=0; i<o->ec; i++) if (o->el[i]->v1==v1 && o->el[i]->v2==v2) return;
    for (i=0; i<o->ec; i++) if (o->el[i]->v1==v2 && o->el[i]->v2==v1) return;
    if (v_dot(normal(&w1, &e->v1->pos, &v1->pos, &v2->pos),
              normal(&w2, &e->v2->pos, &v2->pos, &v1->pos))<0.5) return;
    e->p1->flag=e->p2->flag=1;
    for (i=0; i<3; i++) if (e->p1->vl[i]==e->v1) e->p1->vl[i]=v2;
    for (i=0; i<3; i++) if (e->p2->vl[i]==e->v2) e->p2->vl[i]=v1;
    e->v1=v2; e->v2=v1;
  }
}
static void turnall() {
  int i;
  for (i=0; i<o->ec>>1; i++) turn1(o->el[xrand()%o->ec]);
  for (i=0; i<o->ec; i++) turn1(o->el[i]);
}

static void vertopt() {
  int i, j;
  Plink *l;
  Poly3d *p;
  return;
  mark();
  for (i=0; i<o->vc; i++) o->vl[i]->v2d=0;
  for (i=0; i<o->pc; i++) {
    p=o->pl[i];
    for (j=0; j<3; j++) {
      l=zgetmem(sizeof(Plink));
      l->p=p;
      l->next=(Plink*)p->vl[j]->v2d;
      p->vl[j]->v2d=(Vertex2d*)l;
    }
  }
  for (i=0; i<o->vc; i++) vopt1(o->vl[i]);
  release();
}

Mesh *new_mesh_maasto(Material *m, Lfb *field, int polys, float kork, float taso0) {
  int x, y, xx, zz, i, j;
  Vertex3d *w1;
  Poly3d *f;
  float a1, a2, r=15.95;

  o=new_mesh_alloc(polys+100, res*res+polys);
  o->vc=res*res; o->pc=2*(res-1)*(res-1);
  buf=*field;
  kerroin=1.0/256*kork;
  inkki=taso0;

  for (y=0; y<res; y++) for (x=0; x<res; x++) {
    float u, v;
    //int z=0;
    u=(x+0.01)/(res-1+0.02);
    v=(y+0.01)/(res-1+0.02);
    xx=(2*u-1)*wid/2; zz=(2*v-1)*hgt/2;
    mesh_v_set(o, y*res+x, xx, geth(xx, zz), zz, u, v);
    //if (z) {
      //float r=3000.0/fsqrt(fsqr(o->vl[y*res+x]->pos.x)+fsqr(o->vl[y*res+x]->pos.z));
      //o->vl[y*res+x]->pos.x*=r; o->vl[y*res+x]->pos.z*=r;
    //}
  }
  for (y=0; y<res-1; y++) {
    for (x=0; x<res-1; x++) {
      float h;
      i=y*res+x;
      mesh_p_set3(o, (y*(res-1)+x)*2  , m, i, i+res, i+res+1);
      mesh_p_set3(o, (y*(res-1)+x)*2+1, m, i, i+res+1, i+1);
    }
  }
  while (o->pc<polys) {
    for (i=0; i<o->pc; i++) o->pl[i]->flag=0; mesh_calc_edges(o);
    j=o->pc+(o->pc>>3); if (j>polys) j=polys;
    while (o->pc<j) subdiv1();
    for (i=0; i<o->pc; i++) o->pl[i]->flag=0; mesh_calc_edges(o); vertopt(); turnall();
  }
  for (i=0; i<o->pc; i++) o->pl[i]->flag=0; mesh_calc_edges(o); vertopt(); turnall();
  for (i=0; i<o->pc; i++) o->pl[i]->flag=0; mesh_calc_edges(o); vertopt(); turnall();
  for (i=0; i<o->pc; i++) o->pl[i]->flag=0; mesh_calc_edges(o); vertopt(); turnall();
  for (i=0; i<o->pc; i++) o->pl[i]->flag=0; mesh_calc_edges(o); vertopt(); turnall();
  for (i=0; i<o->vc; i++) {
    o->vl[i]->u=(o->vl[i]->pos.x*(1/wid)+0.5)*16777216.0;
    o->vl[i]->v=(o->vl[i]->pos.z*(1/hgt)+0.5)*16777216.0;
  }
  mesh_recalc(o);
  return o;
}



