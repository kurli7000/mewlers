#include <stdlib.h>
#include "core.h"
#include "obuja.h"
#include "mesh.h"


#define polyfor(p, pl, pc) for ((p##_)=(pl); (p)=*(p##_), (p##_)<(pl)+(pc); (p##_)++)
#define vertfor(v, vl, vc) for ((v##_)=(vl); (v)=*(v##_), (v##_)<(vl)+(vc); (v##_)++)

Mesh *new_mesh_alloc(int polys, int verts) {
  Mesh *o=newobjz(Mesh);
  o->pl=memz(sizeof(Poly3d*)*polys);
  o->vl=memz(sizeof(Vertex3d*)*verts);
  o->pc=polys; o->vc=verts; return o;
}
Mesh *mesh_v_set(Mesh *o, int i, float x, float y, float z, float u, float v) {
  Vertex3d *w=o->vl[i]=memz(sizeof(Vertex3d));
  v_create(&w->pos, x, y, z); w->u=u*16777216.0; w->v=v*16777216.0;
  return o;
}
Mesh *mesh_p_set3(Mesh *o, int i, Material *m, int v1, int v2, int v3) {
  Poly3d *p=o->pl[i];
  if (!p || p->vc<3) p=o->pl[i]=memz(sizeof(Poly3d)+3*sizeof(Vertex3d*));
  p->m=m; p->vc=3;
  p->vl[0]=o->vl[v1]; p->vl[1]=o->vl[v2];
  p->vl[2]=o->vl[v3];
  return o;
}
Mesh *mesh_p_set4(Mesh *o, int i, Material *m, int v1, int v2, int v3, int v4) {
  Poly3d *p=o->pl[i];
  if (!p || p->vc<4) p=o->pl[i]=memz(sizeof(Poly3d)+4*sizeof(Vertex3d*));
  p->m=m; p->vc=4;
  p->vl[0]=o->vl[v1]; p->vl[1]=o->vl[v2];
  p->vl[2]=o->vl[v3]; p->vl[3]=o->vl[v4];
  return o;
}
Mesh *mesh_p_setn(Mesh *o, int i, Material *m, int n) {
  Poly3d *p=o->pl[i];
  if (!p || p->vc<n) p=o->pl[i]=memz(sizeof(Poly3d)+n*sizeof(Vertex3d*));
  p->m=m; p->vc=n;
  return o;
}
void p_calc_normal(Poly3d *p) {
  Vector v1, v2;
  v_norm(v_cross(&p->p.n, v_sub(&v1, &p->vl[1]->pos, &p->vl[0]->pos)
                        , v_sub(&v2, &p->vl[2]->pos, &p->vl[0]->pos)));
  p->p.d=-v_dot(&p->vl[0]->pos, &p->p.n);
}
Mesh *mesh_calc_bound(Mesh *o) {
  Vertex3d *v, **v_;
  int vp;
  v_create(&o->box0, 0, 0, 0);
  v_create(&o->box1, 0, 0, 0);
  vertfor(v, o->vl, o->vc) {
    if (v->pos.x<o->box0.x) o->box0.x=v->pos.x;
    if (v->pos.y<o->box0.y) o->box0.y=v->pos.y;
    if (v->pos.z<o->box0.z) o->box0.z=v->pos.z;
    if (v->pos.x>o->box1.x) o->box1.x=v->pos.x;
    if (v->pos.y>o->box1.y) o->box1.y=v->pos.y;
    if (v->pos.z>o->box1.z) o->box1.z=v->pos.z;
  }
  return o;
}
static Edge3d *ecache[1024];

static void mesh_addedge(Mesh *o, Vertex3d *v1, Vertex3d *v2, Poly3d *p) {
  int i;
  Vertex3d *v;
  Edge3d *e;
  if (v1<v2) v=v1, v1=v2, v2=v;
  i=(int)v1*0x6f46cb73+(int)v2*0xc7d10a77>>16&0x3ff;
  for (e=ecache[i]; e; e=e->next) if (e->v1==v1 && e->v2==v2) {
    if (e->p2) err("calcedge: edge shared with more than 2 polys");
    e->p2=p; break;
  }
  if (!e) {
    //e=o->el[o->ec++]=memz(sizeof(Edge3d));
    o->ec++;
    e=memz(sizeof(Edge3d));
    e->p1=p; e->v1=v1; e->v2=v2; e->next=ecache[i]; ecache[i]=e;
  }
}
Mesh *mesh_calc_edges(Mesh *o) {
  int i, j, pp, ep;
  Vertex3d *v1, *v2, **v1_, **v2_;
  Edge3d *e;
  Poly3d *p, **p_;
  Vector v;

  //o->el=memz(sizeof(Edge3d*)*3*o->vc);
  o->ec=0;
  stosd(ecache, 0, sizeof(ecache)>>2);
  polyfor(p, o->pl, o->pc) {
    for (i=0, j=p->vc-1; i<p->vc; j=i++) mesh_addedge(o, p->vl[i], p->vl[j], p);
  }
  o->el=memz(sizeof(Edge3d*)*o->ec);
  for (i=0, j=0; i<1024; i++) for (e=ecache[i]; e; e=e->next) o->el[j++]=e;
  o->aec=0;
  for (i=0; i<o->ec; i++) { e=o->el[i];
    v_mul(&v, v_add3(&v, &e->p1->vl[0]->pos, &e->p1->vl[1]->pos, &e->p1->vl[2]->pos), 1/3.0);
    o->aec+=e->flag=!e->p2 || v_dot(&e->p1->p.n, &e->p2->p.n)<=0.997
      && v_dot(&v, &e->p2->p.n)+e->p2->p.d<-0.000001;
  }
  o->ael=memz(sizeof(Edge3d*)*o->aec);
  for (i=0, j=0; i<o->ec; i++) if (o->el[i]->flag) o->ael[j++]=o->el[i];
  return o;
}
Mesh *mesh_calc_normals(Mesh *o) {
  int pp, i, j;
  Poly3d *p, **p_;
  Vertex3d *v, **v_;
  Vector v1, v2;
  j=0;
  polyfor(p, o->pl, o->pc) vertfor(v, p->vl, p->vc) v->flag=0;
  polyfor(p, o->pl, o->pc) vertfor(v, p->vl, p->vc) if (!v->flag) v->flag=1, j++;
  if (j>o->vc || 1) o->vl=memz(j*sizeof(Vertex3d*));
  o->vc=j;
  j=0;
  polyfor(p, o->pl, o->pc) vertfor(v, p->vl, p->vc)
    if (v->flag==1) v->flag=0, o->vl[j++]=v, v_create(&v->n, 0.0, 0.0, 0.0);
  polyfor(p, o->pl, o->pc) {
    p_calc_normal(p);
    vertfor(v, p->vl, p->vc) v_add(&v->n, &v->n, &p->p.n);
  }
  vertfor(v, o->vl, o->vc) v_norm(&v->n);
  return o;
}
Mesh *mesh_killempty(Mesh *o) {
  Vertex3d *v, **v_;
  Poly3d *p, **p_;
  int i, j;
  j=0; vertfor(v, o->vl, o->vc) if (v) o->vl[j++]=v; o->vc=j;
  j=0; polyfor(p, o->pl, o->pc) if (p) o->pl[j++]=p; o->pc=j;
  return o;
}
static int hilbert2d(float x, float y, int depth) {
  if (depth<0) return 0;
  y*=2; x*=2;
  if (y<1) {
    if (x<1) return hilbert2d(y  , x  , depth-2);
        else return hilbert2d(x-1, y  , depth-2)+(1<<depth);
  } else {
    if (x<1) return hilbert2d(2-y, 1-x, depth-2)+(3<<depth);
        else return hilbert2d(x-1, y-1, depth-2)+(2<<depth);
  }
}
float mesh_p_dist(Mesh *o, int i, int j) {
  Poly3d *p=o->pl[i], *q=o->pl[j];
  Vertex3d *v, **v_;
  Vector v1, v2;
  if (i<0 || j<0 || i>=o->pc || j>=o->pc) return 0;
  v_create(&v1, 0, 0, 0); v_create(&v2, 0, 0, 0);
  vertfor(v, p->vl, p->vc) v_add(&v1, &v1, &v->pos);
  vertfor(v, q->vl, q->vc) v_add(&v2, &v2, &v->pos);
  v_sub(&v1, v_mul(&v1, &v1, 1.0/p->vc), v_mul(&v2, &v2, 1.0/q->vc));
  return v_dot(&v1, &v1);
}
Mesh *mesh_p_swap(Mesh *o, int i, int j) {
  if ((o->pl[i]<o->pl[j] ^ i>j) && o->pl[i]->vc==o->pl[j]->vc && o->pl[i]->vc<20) {
    static char tmp[512];
    movsb(tmp     , o->pl[i], sizeof(Poly3d)+o->pl[i]->vc*sizeof(Vertex3d*));
    movsb(o->pl[i], o->pl[j], sizeof(Poly3d)+o->pl[i]->vc*sizeof(Vertex3d*));
    movsb(o->pl[j], tmp     , sizeof(Poly3d)+o->pl[i]->vc*sizeof(Vertex3d*));
  } else {
    Poly3d *p=o->pl[i]; o->pl[i]=o->pl[j]; o->pl[j]=p;
  }
  return o;
}
Mesh *mesh_v_swap(Mesh *o, int i, int j) {
  Vertex3d *v=o->vl[i]; o->vl[i]=o->vl[j]; o->vl[j]=v;
  return o;
}
Mesh *mesh_v_reorder(Mesh *o, int i, int j) {
  if (o->vl[i]<o->vl[j] ^ i>j) {
    static Vertex3d tmp, *v;
    movsb(&tmp    , o->vl[i], sizeof(Vertex3d));
    movsb(o->vl[i], o->vl[j], sizeof(Vertex3d));
    movsb(o->vl[j], &tmp    , sizeof(Vertex3d));
    v=o->vl[i]; o->vl[i]=o->vl[j]; o->vl[j]=v;
  }
  return o;
}
/*Mesh *mesh_reorder(Mesh *o) {
  int i, j, k;
  float x, y;
  Poly3d *p;
  mesh_calc_normals(mesh_calc_bound(mesh_killempty(o)));
  for (i=0; i<o->pc; i++) {
    p=o->pl[i];
    for (x=0, y=0, j=0; j<p->vc; j++)
      x+=p->vl[j]->pos.x-o->box0.x, y+=p->vl[j]->pos.z-o->box0.z;
    p->flag=hilbert2d(x/((o->box1.x-o->box0.x)*p->vc),
                      y/((o->box1.z-o->box0.z)*p->vc), 28);
  }
  do {
    j=0;
    for (i=1; i<o->pc; i++)
      if (o->pl[i-1]->flag>o->pl[i]->flag) mesh_p_swap(o, i-1, i), j++;
  } while (j);
  for (k=0; k<3; k++) {
    for (i=0; i<o->pc-1; i++) {
      if (mesh_p_dist(o, i-1, i)+mesh_p_dist(o, i+1, i+2)>
          mesh_p_dist(o, i-1, i+1)+mesh_p_dist(o, i, i+2))
          mesh_p_swap(o, i, i+1);
    }
    for (i=o->pc-2; i>=0; i--) {
      if (mesh_p_dist(o, i-1, i)+mesh_p_dist(o, i+1, i+2)>
          mesh_p_dist(o, i-1, i+1)+mesh_p_dist(o, i, i+2))
          mesh_p_swap(o, i, i+1);
    }
  }
  for (i=0; i<o->vc; i++) o->vl[i]->flag=-1;
  for (i=0; i<o->pc; i++) {
    p=o->pl[i];
    for (j=0; j<p->vc; j++) if ((unsigned)p<(unsigned)p->vl[j]->flag) p->vl[j]->flag=(unsigned)p;
  }
  do {
    j=0;
    for (i=1; i<o->vc; i++)
      if ((unsigned)o->vl[i-1]->flag>(unsigned)o->vl[i]->flag) mesh_v_swap(o, i-1, i), j++;
  } while (j);
  for (i=0; i<o->vc; i++) o->vl[i]->flag=i;
  for (i=0; i<o->pc; i++) {
    p=o->pl[i];
    for (j=0; j<p->vc; j++) p->vl[j]=(void*)p->vl[j]->flag;
  }
  for (i=1; i<o->vc; i++) mesh_v_reorder(o, i-1, i);
  for (i=0; i<o->pc; i++) {
    p=o->pl[i];
    for (j=0; j<p->vc; j++) p->vl[j]=o->vl[(int)p->vl[j]];
  }
  return mesh_calc_edges(o);
}*/
Mesh *mesh_recalc(Mesh *o) {
  return /*mesh_calc_edges*/(mesh_calc_normals(mesh_calc_bound(mesh_killempty(o))));
}
static void phile_writeint(Phile *ph, int x, int len) {
  phile_write(ph, &x, len);
}
static int phile_readint(Phile *ph, int len) {
  int x; phile_read(ph, &x, len);
  return x<<32-len*8>>32-len*8;
}
static void phile_writedelta(Phile *ph, int x) {
  if (-0x20<=x && x<0x20) phile_writeint(ph, x<<2, 1);
    else if (-0x2000<=x && x<0x2000) phile_writeint(ph, x<<2|1, 2);
    else if (-0x200000<=x && x<0x200000) phile_writeint(ph, x<<2|2, 3);
    else phile_writeint(ph, 3, 1), phile_writeint(ph, x, 4);
}
static int phile_readdelta(Phile *ph) {
  int x;
  x=phile_readint(ph, 1)&3;
  if (x==3) return phile_readint(ph, 4); else {
    phile_dseek(ph, -1);
    if (x==0) return phile_readint(ph, 1)>>2;
    if (x==1) return phile_readint(ph, 2)>>2;
    return phile_readint(ph, 3)>>2;
  }
}

Mesh *mesh_write(Mesh *o, Phile *ph) {
  int x, y, z, i, j, k;
  mesh_recalc(o);
  phile_write(ph, &o->vc, 4);
  phile_write(ph, &o->pc, 4);
  phile_write(ph, &o->box0, sizeof(Vector));
  phile_write(ph, &o->box1, sizeof(Vector));
  for (i=0; i<o->vc; i++) {
    o->vl[i]->flag=i;
    x=(o->vl[i]->pos.x-o->box0.x)*65535.4/(o->box1.x-o->box0.x);
    y=(o->vl[i]->pos.y-o->box0.y)*65535.4/(o->box1.y-o->box0.y);
    z=(o->vl[i]->pos.z-o->box0.z)*65535.4/(o->box1.z-o->box0.z);
    phile_write(ph, &x, 2);
    phile_write(ph, &y, 2);
    phile_write(ph, &z, 2);
    phile_write(ph, &o->vl[i]->u, 4);
    phile_write(ph, &o->vl[i]->v, 4);
    //x=(o->vl[i]->u)*65535.4/(o->box1.x-o->box0.x);
    //y=(o->vl[i]->v)*65535.4/(o->box1.y-o->box0.y);
  }
  k=0;
  for (i=0; i<o->pc; i++) {
    phile_write(ph, &o->pl[i]->vc, 2);
    if (!o->pl[i]) err("fgdfgdf");
    for (j=0; j<o->pl[i]->vc; j++) {
      //phile_write(ph, &o->pl[i]->vl[j]->flag, 2);
      phile_writedelta(ph, o->pl[i]->vl[j]->flag-k);
      k=o->pl[i]->vl[j]->flag;
    }
  }
  return o;
}
//90438(60633) -> 75302(55379)
Mesh *new_mesh_read(Phile *ph, Material *m) {
  int x, y, z, u, v, i, j, k;
  int vc, pc;
  Mesh *o;
  phile_read(ph, &vc, 4);
  phile_read(ph, &pc, 4);
  o=new_mesh_alloc(pc, vc); o->vc=vc; o->pc=pc;
  phile_read(ph, &o->box0, sizeof(Vector));
  phile_read(ph, &o->box1, sizeof(Vector));
  for (i=0; i<o->vc; i++) {
    x=y=z=u=v=0;
    phile_read(ph, &x, 2);
    phile_read(ph, &y, 2);
    phile_read(ph, &z, 2);
    phile_read(ph, &u, 4);
    phile_read(ph, &v, 4);
    mesh_v_set(o, i, o->box0.x+x*(o->box1.x-o->box0.x)*(1/65536.0),
                     o->box0.y+y*(o->box1.y-o->box0.y)*(1/65536.0),
                     o->box0.z+z*(o->box1.z-o->box0.z)*(1/65536.0), u*(1/16777216.0), v*(1/16777216.0));
    o->vl[i]->flag=i;
  }
  k=0;
  for (i=0; i<o->pc; i++) {
    vc=0; phile_read(ph, &vc, 2);
    //if (vc!=3) err("new_mesh_read: only triangles accepted at the moment");
    mesh_p_setn(o, i, m, vc);
    for (j=0; j<vc; j++) {
      k+=phile_readdelta(ph);
      if (k>=o->vc) err("new_mesh_read: error in vertex data");
      o->pl[i]->vl[j]=o->vl[k];
    }
  }
  return mesh_recalc(o);
}
Mesh *mesh_wraparound(Mesh *o) {
  int i, j;
  Vertex3d *v;
  for (i=0; i<o->pc; i++) {
    for (j=1; j<o->pl[i]->vc; j++) {
      if (o->pl[i]->vl[j]->u>o->pl[i]->vl[0]->u+8388608) {
        v=mem(sizeof(Vertex3d));
        movsd(v, o->pl[i]->vl[j], sizeof(Vertex3d));
        v->u-=16777216; o->pl[i]->vl[j]=v;
      }
    }
  }
  return o;
}




