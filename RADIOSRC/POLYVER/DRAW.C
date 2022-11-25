#include "core.h"
#include "functs.h"
#include <stdlib.h>
#include <stdio.h>
#include "mesh.pri"
#include "mesh.h"
#include "space.pri"
#include "matter.pri"


static int frame_number=0;
static Matrix om, om_inv, mf, mf_inv;
static Llist *lights=0;
static Vector amb;

extern Material linemat;
extern Material portalmat;
static Plane clip_pl[32], *clip_plm;
static int bits;
//static Tri2d *tplink;
static Space cur_f;

extern void erreur(char *);

extern char *heapp;

void shit() { erreur("feature disabled due to a lazy coder."); }

Vertex2d *_new_v2d()  { Vertex2d *x=getmem(sizeof(*x)); return x; }
Tri2d    *_new_tri()  { Tri2d    *x=getmem(sizeof(*x)); return x; }
Vertex3d *_new_v3d()  { Vertex3d *x=getmem(sizeof(*x)); return x; }
Edge3d   *_new_edge() { Edge3d   *x=zgetmem(sizeof(*x)); return x; }
Mesh     *_new_mesh() { Mesh     *x=zgetmem(sizeof(*x)); x->flgs=65535; return x; }

#undef new_v3d
#undef new_v2d
#undef new_tri
#define new_v3d() ((*(Vertex3d**)&heapp)++)
#define new_v2d() ((*(Vertex2d**)&heapp)++)
#define new_tri() ((*(Tri2d**)&heapp)++)

Vertex2d *_save_v2d  (Vertex2d *x) { x=save(x, sizeof(*x)); return x; }
Tri2d    *_save_tri  (Tri2d    *x) { x=save(x, sizeof(*x)); return x; }
Vertex3d *_save_v3d  (Vertex3d *x) { x=save(x, sizeof(*x)); return x; }
Edge3d   *_save_edge (Edge3d   *x) { x=save(x, sizeof(*x)); return x; }
Mesh     *_save_mesh (Mesh     *x) { int s, tmp, v, tmp2; Poly3d *p; Edge3d *e; x=save(x, sizeof(*x));
  shit();
  /*  tmp=(int)x->vl; tmp2=(int)x->pl;
  x->pl=save(x->pl, s=(int)x->pl_end-(int)x->pl); x->pl_end=(Poly3d   *)(((int)x->pl)+s);
  x->vl=save(x->vl, s=(int)x->vl_end-(int)x->vl); x->vl_end=(Vertex3d *)(((int)x->vl)+s);
  tmp=(int)x->vl-tmp; tmp2=(int)x->pl-tmp2;
  x->el=save(x->el, s=(int)x->el_end-(int)x->el); x->el_end=(Edge3d   *)(((int)x->el)+s);
  for (p=x->pl; p<x->pl_end; p=poly_next(p)) for (v=0; v<p->vc; v++)
    p->vl[v]=(Vertex3d*)((int)(p->vl[v])+tmp);
  for (e=x->el; e<x->el_end; e++) {
    e->v1=(Vertex3d*)((int)(e->v1)+tmp);
    e->v2=(Vertex3d*)((int)(e->v2)+tmp);
    e->p1=(Poly3d*)((int)(e->p1)+tmp2);
    e->p2=(Poly3d*)((int)(e->p2)+tmp2);
  }*/
  return x;
}

Vertex2d *_destroy_v2d  (Vertex2d *x) { return destroy(x); }
Tri2d    *_destroy_tri  (Tri2d    *x) { return destroy(x); }
Vertex3d *_destroy_v3d  (Vertex3d *x) { return destroy(x); }
Edge3d   *_destroy_edge (Edge3d   *x) { return destroy(x); }
Mesh     *_destroy_mesh (Mesh     *x) { return destroy(x); }

/*
static Poly3d *poly_cpy(Poly3d *d, Poly3d *tmp) {
  movsd(d, tmp, (sizeof(Poly3d)>>2)+(sizeof(Vertex3d*)>>2)*tmp->vc);
  return d;
}
static Tmppoly *poly_to_tmp(Tmppoly *d, Poly3d *s) {
  int v;
  for (v=0, d->vc=s->vc, d->orm=0, d->andm=-1; v<d->vc; v++)
    d->vl[v]=s->vl[v], d->orm|=d->vl[v]->clip, d->andm&=d->vl[v]->clip;
  return d;
}
static Poly3d *new_poly_tmp(Tmppoly *s, Poly3d *s2) {
  int v;
  Poly3d *d;
  d=getmem(sizeof(Poly3d)+sizeof(Vertex3d*)*s->vc);
  *d=*s2; if ((d->vc=s->vc)==0) return 0;
  for (v=0; v<d->vc; v++) d->vl[v]=s->vl[v];
  return d;
}
static Tmppoly *tmp_cpy(Tmppoly *d, Tmppoly *s) { movsd(d, s, 3+s->vc); return d; }
*/




float jytol_kludge=0.0;
int koamk_kludge=0;
/*static void xform(Vertex3d *v) {
  float x, y, z, cr, cg, cb, r;
  Plane *p;
  Llist *lp;
  int m;
  v->v2d=new_v2d();
  if (jytol_kludge==0.0) {
    cr=amb.x; cg=amb.y; cb=amb.z;
    for (lp=lights; lp; lp=lp->next) {
      x=lp->xpos.x-v->pos.x; y=lp->xpos.y-v->pos.y; z=lp->xpos.z-v->pos.z;
      r=x*x+y*y+z*z; r=16777216*(x*v->n.x+y*v->n.y+z*v->n.z)/(r*fsqrt(r));
      if (r>0) cr+=lp->col.x*r, cg+=lp->col.y*r, cb+=lp->col.z*r;
    }
    v->v2d->r=cr; v->v2d->g=cg; v->v2d->b=cb;
  }

  //v->v2d->r=v->v2d->g=v->v2d->b=0.0;
  v->v2d->z=z=1.0/(v->pos.x*mf.i.z+v->pos.y*mf.j.z+v->pos.z*mf.k.z+mf.l.z);
  //v->v2d->r=z<1?16777215.0*z:16777215.0;
  if (jytol_kludge!=0.0) v->v2d->r=16777215*fexp2(-jytol_kludge/z);
  v->v2d->x=x=(v->pos.x*mf.i.x+v->pos.y*mf.j.x+v->pos.z*mf.k.x+mf.l.x)*z;
  v->v2d->y=y=(v->pos.x*mf.i.y+v->pos.y*mf.j.y+v->pos.z*mf.k.y+mf.l.y)*z;
  v->v2d->u=v->u; v->v2d->v=v->v;
  v->clip=(l_mask (v->v2d->x, cur_f.x0)&1|l_mask (v->v2d->y, cur_f.y0)&2
          |nl_mask(v->v2d->x, cur_f.x1)&4|nl_mask(v->v2d->y, cur_f.y1)&8)
          ^*(int*)&z>>31&31;
  if (v->clip) {
    if (v->clip&16) v->v2d->x=cur_f.x0, v->v2d->y=cur_f.y0;
    if (v->clip&1) v->v2d->x=cur_f.x0;
    if (v->clip&2) v->v2d->y=cur_f.y0;
    if (v->clip&4) v->v2d->x=cur_f.x1;
    if (v->clip&8) v->v2d->y=cur_f.y1;
  }
  //for (m=32, p=clip_pl+5; p<clip_plm; p++, m+=m) v->clip|=~pl_v_inside(p, &v->pos)&m;
  v->clip=0;
}
*/


static void teefogi(Vertex3d **wp, int cnt) {
  Vertex3d *v, **wm;
  Vertex2d *v2;
  for (wm=wp+cnt; wp<wm; wp++) {
    v2=(*wp)->v2d; v2->r=16777215*fexp2(-jytol_kludge/v2->z);
  }
}
static void teevalo(Vertex3d **wp, int cnt, Vector *lp, Vector *lc) {
  float x, y, z, cr, cg, cb, r;
  Plane *p;
  Llist *ll;
  int cl, m;
  Vertex3d *v, **wm;
  Vertex2d *v2;
  for (wm=wp+cnt; wp<wm; wp++) {
    v2=(v=*wp)->v2d;
    //cr=amb.x; cg=amb.y; cb=amb.z;
    x=lp->x-v->pos.x; y=lp->y-v->pos.y; z=lp->z-v->pos.z;
    r=x*x+y*y+z*z; r=16777216.0*(x*v->n.x+y*v->n.y+z*v->n.z)/(r*fsqrt(r));
    if (r<0) r=-r;
    v2->r=lc->x*r, v2->g=lc->y*r, v2->b=lc->z*r;
  }
}
static void xforml(Vertex3d **wp, int cnt) {
  float x, y, z;
  int cl;
  Vertex3d *v, **wm;
  Vertex2d *v2;
  for (wm=wp+cnt; wp<wm; wp++) {
    v=*wp;
    v2=v->v2d=new_v2d();
    v2->z=z=1.0/(v->pos.x*mf.i.z+v->pos.y*mf.j.z+v->pos.z*mf.k.z+mf.l.z);
    v2->x=x=(v->pos.x*mf.i.x+v->pos.y*mf.j.x+v->pos.z*mf.k.x+mf.l.x)*z;
    v2->y=y=(v->pos.x*mf.i.y+v->pos.y*mf.j.y+v->pos.z*mf.k.y+mf.l.y)*z;
    v2->u=v->u; v2->v=v->v;
    cl=(l_mask (v2->x, cur_f.x0)&1|l_mask (v2->y, cur_f.y0)&2
       |nl_mask(v2->x, cur_f.x1)&4|nl_mask(v2->y, cur_f.y1)&8)^*(int*)&z>>31&31;
    if (cl) {
      if (cl&16) v2->x=cur_f.x0, v2->y=cur_f.y0;
      if (cl&1) v2->x=cur_f.x0;
      if (cl&2) v2->y=cur_f.y0;
      if (cl&4) v2->x=cur_f.x1;
      if (cl&8) v2->y=cur_f.y1;
    }
  }
}


static Vertex3d *clipline(Plane *pl, Vertex3d *v1, Vertex3d *v2) {
  Vector d; float t;
  Vertex3d *v=new_v3d();
  v_sub(&d, &v1->pos, &v2->pos);
  t=(v_dot(&pl->n, &v1->pos)+pl->d)/v_dot(&pl->n, &d);
  if (t<0) t=0; if (t>1) t=1;
  *v=*v1;
  v_add(&v->pos, v_mul(&v->pos, v_sub(&v->pos, &v2->pos, &v1->pos), t), &v1->pos);
  v->u=v1->u+(v2->u-v1->u)*t;
  v->v=v1->v+(v2->v-v1->v)*t;
  v->n.x=v1->n.x+(v2->n.x-v1->n.x)*t;
  v->n.y=v1->n.y+(v2->n.y-v1->n.y)*t;
  v->n.z=v1->n.z+(v2->n.z-v1->n.z)*t;
  v->v2d=0;
  return v;
}


void portaali(Poly3d *p) {
  Vector v1, v2, v3, v4;
  Tri2d *tp, *tp2, **lst;
  int i, j, jab, jac, jbc;
  Matrix m, m2, m3, m4;
  Plane pl, pl2;
  Space *f2;
  /*
  f2=space_new_subf(&cur_f);
  m_mmul(&m, new_m_mmul(new_m_mmul(&cur_f.cam, &om_inv), new_m_mirror(&p->p)), &om);

  tp=new_tri(); tp->m=&portalmat;
  tp->top=(Vertex2d*)f2; tp->mid=(Vertex2d*)p->m->space;
  space_mmul(f2, &m);
  tp->z=0x7fffffff;
  for (j=p->vc-1, i=0; i<p->vc; j=i++) {
    teeplane(&pl2, &mf_inv.l, &p->vl[j]->pos, &p->vl[i]->pos);
    pl_norm(pl_mmul(&pl, &pl2, &mf_inv));
    space_addplane(f2, &pl);
    if ((unsigned)(p->vl[i]->v2d->z)<(unsigned)(tp->z)) tp->z=p->vl[i]->v2d->z;
  }
  teeplane(&pl2, &p->vl[0]->pos, &p->vl[1]->pos, &p->vl[2]->pos);
  pl_norm(pl_mmul(&pl, &pl2, &mf_inv));
  space_addplane(f2, &pl);

  tp->z*=3;
  lst=cur_f.lst1+(tp->z&1023); tp2=tp->next; tp->next=*lst; *lst=tp; tp=tp2;
  */
}

void drawl(Poly3d **pp, int pc) {
  Vertex2d *aa, *bb, *cc;
  Tri2d *tp, *tp2, **lst;
  Poly3d *p, **pm;
  int i, j, jab, jac, jbc;
  for (pm=pp+pc; pp<pm; pp++) {
    p=*pp;
    aa=p->vl[0]->v2d;
    for (i=1; i<p->vc-1; i++) {
      bb=p->vl[i]->v2d; cc=p->vl[i+1]->v2d;
      tp=new_tri(); tp->m=p->m; //tp->c=(int)(p+frame_number)>>10&65535;
      jab=b_mask(aa->y, bb->y)&sizeof(Vertex2d*);
      jac=b_mask(aa->y, cc->y)&sizeof(Vertex2d*);
      jbc=b_mask(bb->y, cc->y)&sizeof(Vertex2d*);
      *(Vertex2d**)((int)&tp->bot-jab-jac)=aa;
      *(Vertex2d**)((int)&tp->mid+jab-jbc)=bb;
      *(Vertex2d**)((int)&tp->top+jac+jbc)=cc;
      tp->z=tp->top->z+tp->mid->z+tp->bot->z;
      lst=cur_f.lst1+(tp->z&1023); tp2=tp->next; tp->next=*lst; *lst=tp; tp=tp2;
    }
  }
}

static Poly3d *outpoly, *inpoly;
void poly_split(Poly3d *p, int pno) {
  Plane *pl=clip_pl+pno;
  int bit=1<<pno;
  Vertex3d *c, **l1, **l2;
  int j, icnt, ocnt;

  for (j=0, icnt=ocnt=0; j<p->vc; j++) if (p->vl[j]->clip&bit) ocnt++; else icnt++;
  if (!ocnt) { inpoly=p; outpoly=0; return; }
  if (!icnt) { outpoly=p; inpoly=0; return; }
  inpoly=getmem(sizeof(Poly3d)+(icnt+2)*sizeof(Vertex3d*)); *inpoly=*p;
  outpoly=getmem(sizeof(Poly3d)+(ocnt+2)*sizeof(Vertex3d*)); *outpoly=*p;
  inpoly->vc=outpoly->vc=0;
  l2=p->vl+p->vc-1;
  for (l1=p->vl; l1<p->vl+p->vc; l2=l1++) {
    if (l2[0]->clip&bit) {
      outpoly->vl[outpoly->vc++]=l2[0];
      if (~l1[0]->clip&bit) {
        c=clipline(pl, l1[0], l2[0]);
        for (j=0, c->clip=0; j<clip_plm-clip_pl; j++) c->clip|=~pl_v_inside(clip_pl+j, &c->pos)&1<<j;
        outpoly->vl[outpoly->vc++]=c; inpoly->vl[ inpoly->vc++]=c;
      }
    } else {
      inpoly->vl[inpoly->vc++]=l2[0];
      if (l1[0]->clip&bit) {
        c=clipline(pl, l2[0], l1[0]);
        for (j=0, c->clip=0; j<clip_plm-clip_pl; j++) c->clip|=~pl_v_inside(clip_pl+j, &c->pos)&1<<j;
        outpoly->vl[outpoly->vc++]=c; inpoly->vl[ inpoly->vc++]=c;
      }
    }
  }
  if (inpoly->vc!=icnt+2 || outpoly->vc!=ocnt+2) erreur("vittu");
}


//4576 4351
static void clippoly2(Mesh *o, Poly3d *s, Poly3d ***d) {
  int bit, i;
  if (!s) return;
  for (bit=1, i=0; bit<=bits; bit+=bit, i++) {
    poly_split(s, i);
    if (outpoly) *(*d)++=outpoly;
    if (inpoly) s=inpoly; else return;
  }
}
static void clippoly3(Mesh *o, Poly3d *s, Poly3d ***d) {
  int bit, i;
  //if (s) *(*d)++=s;
  for (bit=1, i=0; bit<=bits; bit+=bit, i++) {
    if (!s) return;
    poly_split(s, i); s=inpoly;
  }
  if (s) *(*d)++=s;
}



// 1, jos kokonaan ulkona
static int chkbox(Mesh *o, Plane *p) {
  float f=p->d+
    p->n.x*((Vector*)((int)&o->box0+((int)&o->box1-(int)&o->box0&*(int*)&p->n.x>>31)))->x+
    p->n.y*((Vector*)((int)&o->box0+((int)&o->box1-(int)&o->box0&*(int*)&p->n.y>>31)))->y+
    p->n.z*((Vector*)((int)&o->box0+((int)&o->box1-(int)&o->box0&*(int*)&p->n.z>>31)))->z;
  return ~(*(int*)&f>>31);
}
// 1, jos edes osittain ulkona
static int chkbox2(Mesh *o, Plane *p) {
  float f=p->d+
    p->n.x*((Vector*)((int)&o->box0+((int)&o->box1-(int)&o->box0&~*(int*)&p->n.x>>31)))->x+
    p->n.y*((Vector*)((int)&o->box0+((int)&o->box1-(int)&o->box0&~*(int*)&p->n.y>>31)))->y+
    p->n.z*((Vector*)((int)&o->box0+((int)&o->box1-(int)&o->box0&~*(int*)&p->n.z>>31)))->z;
  return ~(*(int*)&f>>31);
}

static void subdiv(Tri2d *tp) {
  Tri2d **lst, *tp2, *tp3;
  int x, y;
  float ap, bp, cp;
  float av, bv, cv;
  Vertex2d *aa, *bb, *cc;
  Vertex2d *top, *mid, *bot;
  while (tp) {
    top=tp->top; mid=tp->mid; bot=tp->bot;
    x=mid->x-bot->x; y=mid->y-bot->y;
    av=(ffabs(ap=mid->z/(float)(mid->z+bot->z)-0.5)*((x^x>>31)*2+(y^y>>31)))*0.0002-4;
    x=bot->x-top->x; y=bot->y-top->y;
    bv=(ffabs(bp=bot->z/(float)(bot->z+top->z)-0.5)*((x^x>>31)*2+(y^y>>31)))*0.0002-4;
    x=top->x-mid->x; y=top->y-mid->y;
    cv=(ffabs(cp=top->z/(float)(top->z+mid->z)-0.5)*((x^x>>31)*2+(y^y>>31)))*0.0002-4;
    if (av<0&&bv<0&&cv<0||1) {
      tp->z=top->z+mid->z+bot->z;
      lst=cur_f.lst1+(tp->z&1023); tp2=tp->next; tp->next=*lst; *lst=tp; tp=tp2;
      continue;
    }
    if (av<0) av=0; if (av>1) av=1; ap=0.5+ap*av;
    if (bv<0) bv=0; if (bv>1) bv=1; bp=0.5+bp*bv;
    if (cv<0) cv=0; if (cv>1) cv=1; cp=0.5+cp*cv;
    aa=new_v2d(); aa->x=mid->x+bot->x>>1; aa->y=mid->y+bot->y>>1; aa->z=mid->z+bot->z>>1; aa->u=bot->u+(mid->u-bot->u)*ap; aa->v=bot->v+(mid->v-bot->v)*ap;
    bb=new_v2d(); bb->x=bot->x+top->x>>1; bb->y=bot->y+top->y>>1; bb->z=bot->z+top->z>>1; bb->u=top->u+(bot->u-top->u)*bp; bb->v=top->v+(bot->v-top->v)*bp;
    cc=new_v2d(); cc->x=top->x+mid->x>>1; cc->y=top->y+mid->y>>1; cc->z=top->z+mid->z>>1; cc->u=mid->u+(top->u-mid->u)*cp; cc->v=mid->v+(top->v-mid->v)*cp;
    tp3=tp2=new_tri(); tp2->top=cc; tp2->mid=tp->mid; tp2->bot=aa; tp2->m=tp->m; tp2->next=new_tri();
    tp2=tp2->next;     tp2->top=cc; tp2->mid=bb;      tp2->bot=aa; tp2->m=tp->m; tp2->next=new_tri();
    tp2=tp2->next;     tp2->top=bb; tp2->mid=aa; tp2->bot=tp->bot; tp2->m=tp->m; tp2->next=tp;
    tp->top=tp->top; tp->mid=cc; tp->bot=bb;
    tp=tp3;
  }
}

static void antialias(Mesh *o) {
  Edge3d *e;
  Vertex3d *v, *vv, *c; Poly3d *p;
  int i, c_or, bit, ep;
  Tri2d **lst, *tp;
  Plane *pl;
  extern volatile char kcnt[];
  extern void aadrawline(Tri2d*), greenline(Tri2d*);
  if (kcnt[2]&1) return;

  linemat.fill=koamk_kludge?greenline:aadrawline;

  for (ep=0; ep<o->ec; ep++) { e=o->el[ep];
    v=e->v1; vv=e->v2;
    if (((e->p1&&e->p1->flag)^(e->p2&&e->p2->flag)) && !(v->clip&vv->clip)) {
      //exit(1);
      if ((c_or=v->clip|vv->clip)!=0) {
        for (bit=1, pl=clip_pl; bit<=c_or; bit+=bit, pl++) if (bit&(v->clip|vv->clip)) {
          if (v->clip&bit) c=v=clipline(pl, vv, v);
            else c=vv=clipline(pl, v, vv);
          for (i=0, c->clip=0; i<clip_plm-clip_pl; i++)
            c->clip|=~pl_v_inside(clip_pl+i, &c->pos)&1<<i;
        }
        if (!v->v2d) xforml(&v, 1);
        if (!vv->v2d) xforml(&vv, 1);
      }
      tp=new_tri(); tp->m=&linemat;
      tp->top=v->v2d; tp->mid=tp->bot=vv->v2d; tp->z=0;
      if ((p=e->p1)!=0&&p->flag) { for (i=0; i<p->vc; i++) if (p->vl[i]->v2d->z>tp->z) tp->z=p->vl[i]->v2d->z+1; }
      if ((p=e->p2)!=0&&p->flag) { for (i=0; i<p->vc; i++) if (p->vl[i]->v2d->z>tp->z) tp->z=p->vl[i]->v2d->z+1; }
      if (tp->z==0) return;
      tp->z=tp->z*3+(tp->z>>2);
      lst=cur_f.lst1+(tp->z&1023);
      tp->next=*lst; *lst=tp;
    }
  }
}

Mesh *_mesh_backface(Mesh *o, Vector *c) {
  int pp, i;
  Poly3d *p;
  static int val=0x1231231;
  Mesh *d=getmem(sizeof(Mesh)); *d=*o;
  d->vl=getmem(sizeof(Vertex3d*)*2*o->vc); d->vc=0;
  d->pl=getmem(sizeof(Poly3d*)*2*o->pc); d->pc=0;
  for (pp=0; pp<o->pc; pp++) {
    p=o->pl[pp];
    if (!pl_v_inside(&p->p, c)) {
      d->pl[d->pc++]=p;
      for (i=0; i<p->vc; i++)
        if (p->vl[i]->flag!=val) (d->vl[d->vc++]=p->vl[i])->flag=val;
    }
  }
  val++;
  return d;
}

Mesh *_mesh_clip_in(Mesh *o, Plane *pl, int pc) {
  Poly3d *p, **dpp;
  int pp, i, j, c_or, c_and;
  static int val=0x12112332;
  Vertex3d *v;
  Mesh *d;
  for (i=0; i<pc; i++) if (chkbox (o, pl+i)) return 0;
  for (i=0; i<pc; i++) if (chkbox2(o, pl+i)) break;
  if (i==pc) return o;
  d=getmem(sizeof(Mesh)); *d=*o;
  for (i=0; i<pc; i++) clip_pl[i]=pl[i];
  dpp=d->pl=getmem(sizeof(Poly3d*)*o->pc);
  clip_plm=clip_pl+pc; bits=(1<<pc)-1;
  val++;
  for (pp=0; pp<o->pc; pp++) {
    p=o->pl[pp];
    c_or=0; c_and=-1;
    for (i=0; i<p->vc; i++) {
      v=p->vl[i];
      if (v->flag!=val) {
        for (j=0, v->clip=0; j<pc; j++) v->clip|=-(~pl_v_inside(clip_pl+j, &v->pos))<<j;
        v->flag=val;
      }
      c_or|=v->clip, c_and&=v->clip;
    }
    if (c_and) ; else if (c_or) clippoly3(o, o->pl[pp], &dpp); else *dpp++=p;
  }
  d->pc=dpp-d->pl;
  return d;
}

Mesh *_mesh_clip_out(Mesh *o, Plane *pl, int pc) {
  Poly3d *p, **dpp;
  int pp, i, j, c_or, c_and;
  static int val=0x48762232;
  Vertex3d *v;
  Mesh *d=getmem(sizeof(Mesh)); *d=*o;
  for (i=0; i<pc; i++) clip_pl[i]=pl[i];
  dpp=d->pl=getmem(sizeof(Poly3d*)*(o->pc+500));
  clip_plm=clip_pl+pc; bits=(1<<pc)-1;
  val++;
  for (pp=0; pp<o->pc; pp++) {
    p=o->pl[pp];
    c_or=0; c_and=-1;
    for (i=0; i<p->vc; i++) {
      v=p->vl[i];
      if (v->flag!=val) {
        for (j=0, v->clip=0; j<pc; j++) v->clip|=~pl_v_inside(clip_pl+j, &v->pos)&1<<j;
        v->flag=val;
      }
      c_or|=v->clip, c_and&=v->clip;
    }
    if (c_and) *dpp++=p; else if (c_or) clippoly2(o, p, &dpp); else ;
  }
  d->pc=dpp-d->pl;
  return d;
}

int _mesh_to_planes(Plane *pl, Mesh *o, Matrix *m) {
  Poly3d *p;
  int i, pc=0;
  for (i=0; i<o->pc; i++) pl_mmul(pl++, &o->pl[i]->p, m), pc++;
  return pc;
}

Mesh *_mesh_flipnormals(Mesh *o) {
  int i; Poly3d *p;
  for (i=0; i<o->pc; i++) { p=o->pl[i];
    p->p.n.x=-p->p.n.x; p->p.n.y=-p->p.n.y; p->p.n.z=-p->p.n.z; p->p.d=-p->p.d;
  }
  return o;
}

Mesh *_mesh_set_mat(Mesh *o, Material *m) {
  int i;
  for (i=0; i<o->pc; i++) o->pl[i]->m=m;
  return o;
}

void tri_sort(Tri2d **lst1, Tri2d **lst2) {
  Tri2d *p, *q, **l1, **l2;
  for (l1=lst1+1023; l1>=lst1; l1--) for (p=*l1; p; p=q) {
    l2=lst2+(p->z>>10&1023); q=p->next; p->next=*l2; *l2=p;
  }
  for (l2=lst2; l2<lst2+1024; l2++) filllst(*l2);
}


void _mesh_draw(Space *f, Mesh *o2, Matrix *m, unsigned flgs) {
  Poly3d *p;
  Tri2d *tp;
  int i, c_or, c_and;
  static int val=0x98765432;
  Vector v1, v2, v3, v4, v5;
  Vertex2d *aa, *bb, *cc;
  int jab, jac, jbc;
  int pp;
  Mesh *o;

  flgs=(o2->flgs|flgs)&~flgs>>16;
  frame_number++;
  lights=f->llist; amb=f->amb;
  om=*m; om_inv=*new_m_inv(&om);
  mf=*new_m_mmul(m, f->cam_f_inv);
  mf_inv=*new_m_inv(&mf);
  if (flgs&f_lights) {
    Llist *lp;
    for (lp=lights; lp; lp=lp->next) v_mmul(&lp->xpos, &lp->pos, &om_inv);
  }
  clip_plm=clip_pl;
  for (i=0; i<f->pl_cnt; i++) pl_mmul(clip_plm++, f->viewcone+i, &mf);
  o=o2;

  cur_f=*f;
  val++;
  o=_mesh_clip_in(o2, clip_pl, clip_plm-clip_pl);
  if (!o) return;
  o=_mesh_backface(o, &mf_inv.l);
  if (!o) return;
  xforml(o->vl, o->vc);
  if (lights) teevalo(o->vl, o->vc, &lights->xpos, &lights->col);
  if (jytol_kludge!=0.0) teefogi(o->vl, o->vc);

  drawl(o->pl, o->pc);
  //for (pp=0; pp<o->pc; pp++) poly_draw(o->pl[pp]);
  if (flgs&f_antialias) antialias(o);
  *f=cur_f;
}


/*

  (t+e)ý+1=tý+2et+eý+1
  f(t)=a/(tý+pt+q)=a/((t-.5p)ý+q-.25pý)
  p=exp(-Sf(t)dt)


*/
