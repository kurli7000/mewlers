#include "scan.h"
#include "space.h"
#include <libya\libya.h>


Space *new_space() {
  Space *x=newobj(Space);
  x->lst1=mem(sizeof(Tri2d*)*1024);
  x->lst2=mem(sizeof(Tri2d*)*1024);
  x->mlist=0; x->llist=0; x->clrflag=0;
  x->amb.x=x->amb.y=x->amb.z=0.01;
  return x;
}




//Space *space_addplane(Space *f, Plane *p) { f->viewcone[f->pl_cnt++]=*p; return f; }
Space *space_addmesh(Space *f, Mesh *o, Matrix *m) {
  Mlist *ml=newobj(Mlist);
  ml->next=f->mlist;
  ml->m=m; ml->o=o;
  f->mlist=ml;
  //Mlist **ml;
  //for (ml=&f->mlist; *ml; ml=&(*ml)->next) ;
  //*ml=newobj(Mlist);
  //(*ml)->next=0;
  //(*ml)->m=m; (*ml)->o=o;
  return f;
}
Space *space_setambient(Space *f, Vector *col) { f->amb=*col; return f; }
Space *space_addlight(Space *f, Vector *pos, Vector *col) {
  Llist *ll=newobj(Llist);
  ll->next=f->llist;
  ll->pos=*pos;
  ll->col=*col;
  f->llist=ll;
  return f;
}
Space *space_clear(Space *s) { s->clrflag=1; return s; }
Space *space_fill(Space *s, Vector *col) { s->clrflag=2; s->clrcol=*col; return s; }



Space *space_render(Space *f, Lfb *lfb, Matrix *camera, float zoom, int flgs) {
  Vector k, l, v1, v2, v3, v4;
  int i, j, y, lkm, col, blk;
  Mlist *ml;
  float xsc=(lfb->width+1<<4)*9, ysc=-(lfb->height+1<<8);
  #define blksz 500
  for (blk=0; blk<lfb->height; blk+=blksz) {
    Ctx *cx=ctx();
    f->cam=camera;
    f->dbuf=*lfb;
    f->dbuf.buf+=blk*f->dbuf.pitch;
    f->dbuf.height=lfb->height-blk;
    if (f->dbuf.height>blksz) f->dbuf.height=blksz;
    f->x0=0<<8; f->x1=f->dbuf.width+1<<8; f->xc=lfb->width+1<<7;
    f->y0=0<<8; f->y1=f->dbuf.height+1<<8; f->yc=lfb->height-blk*2+1<<7;
    f->viewcone=mem(sizeof(Plane)*32); f->pl_cnt=0;
    v_create(&v4, f->x0+64, f->y1-64, 1);
    v_create(&v1, f->x0+64, f->y0+64, 1);
    v_create(&v2, f->x1-64, f->y0+64, 1);
    v_create(&v3, f->x1-64, f->y1-64, 1);
    teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v1, &v4);
    teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v2, &v1);
    teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v3, &v2);
    teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v4, &v3);
    teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v_cj, &v_ci);

    f->cam_inv=new_m_inv(f->cam);
    v_create(&k, xsc, ysc, f->zoom=1.0/zoom); v_mul(&k, &k, 1/16.0/65536.0);
    f->cam_f_inv=new_m();
    v_vmul(&f->cam_f_inv->i, &f->cam_inv->i, &k); v_vmul(&f->cam_f_inv->j, &f->cam_inv->j, &k);
    v_vmul(&f->cam_f_inv->k, &f->cam_inv->k, &k); v_vmul(&f->cam_f_inv->l, &f->cam_inv->l, &k);
    v_create(&k, f->xc, f->yc, 0);
    v_add(&f->cam_f_inv->i, &f->cam_f_inv->i, v_mul(&l, &k, f->cam_f_inv->i.z));
    v_add(&f->cam_f_inv->j, &f->cam_f_inv->j, v_mul(&l, &k, f->cam_f_inv->j.z));
    v_add(&f->cam_f_inv->k, &f->cam_f_inv->k, v_mul(&l, &k, f->cam_f_inv->k.z));
    v_add(&f->cam_f_inv->l, &f->cam_f_inv->l, v_mul(&l, &k, f->cam_f_inv->l.z));
    f->cam_f=new_m_inv(f->cam_f_inv);

    //triprep(f);
    //for (y=0; y<lfb->height; y++) lastsc[y]=scans+y*100;
    clearscans(&f->dbuf);
    for (ml=f->mlist; ml; ml=ml->next) mesh_draw(f, ml->o, ml->m, flgs);
    rasterify(&f->dbuf);
  /*  col=0;
    if (f->clrflag&3) {
      if (f->clrflag==2) {
        if (f->clrcol.x<0) f->clrcol.x=0; else if (f->clrcol.x>1) f->clrcol.x=1;
        if (f->clrcol.y<0) f->clrcol.y=0; else if (f->clrcol.y>1) f->clrcol.y=1;
        if (f->clrcol.z<0) f->clrcol.z=0; else if (f->clrcol.z>1) f->clrcol.z=1;
        col=(int)(fpow(f->clrcol.x, 1.0/monitor_gamma)*31.4)*0x08000800
           +(int)(fpow(f->clrcol.y, 1.0/monitor_gamma)*63.4)*0x00200020
           +(int)(fpow(f->clrcol.z, 1.0/monitor_gamma)*31.4)*0x00010001;
      } else col=0;
    }*/
    ctx_free(cx);
  }
  return f;
}
/*Space *space_new_subf(Space *a) {
  Space *f=new_space();
  int i;
  f->dbuf=a->dbuf;
  f->x0=a->x0; f->x1=a->x1; f->xc=a->xc;
  f->y0=a->y0; f->y1=a->y1; f->yc=a->yc; f->zoom=a->zoom;
  f->cam=a->cam; f->cam_f=a->cam_f;
  f->cam_inv=a->cam_inv; f->cam_f_inv=a->cam_f_inv;
  for (i=0; i<1024; i++) f->lst1[i]=0;
  for (i=0; i<1024; i++) f->lst2[i]=0;
  f->viewcone=mem(sizeof(Plane)*32); f->pl_cnt=0;
  for (i=0; i<a->pl_cnt; i++) space_addplane(f, a->viewcone+i);
  return f;
}
Space *space_close(Space *f) {
  return f;
}
Space *space_mmul(Space *f, Matrix *m) {
  Vector k, l, v1, v2, v3, v4;
  Matrix m2;
  //m_copy(&f->cam, m_mmul(&m2, &f->cam, m));
  m_copy(&f->cam, m);

  m_inv(&f->cam_inv, &f->cam);
  v_create(&k, (f->x1-f->x0)*3>>2, f->y0-f->y1, f->zoom); v_mul(&k, &k, 1/65536.0);
  v_vmul(&f->cam_f_inv->i, &f->cam_inv.i, &k); v_vmul(&f->cam_f_inv->j, &f->cam_inv.j, &k);
  v_vmul(&f->cam_f_inv->k, &f->cam_inv.k, &k); v_vmul(&f->cam_f_inv->l, &f->cam_inv.l, &k);
  v_create(&k, f->xc, f->yc, 0);
  v_add(&f->cam_f_inv->i, &f->cam_f_inv->i, v_mul(&l, &k, f->cam_f_inv->i.z));
  v_add(&f->cam_f_inv->j, &f->cam_f_inv->j, v_mul(&l, &k, f->cam_f_inv->j.z));
  v_add(&f->cam_f_inv->k, &f->cam_f_inv->k, v_mul(&l, &k, f->cam_f_inv->k.z));
  v_add(&f->cam_f_inv->l, &f->cam_f_inv->l, v_mul(&l, &k, f->cam_f_inv->l.z));
  m_inv(&f->cam_f, &f->cam_f_inv);
  return f;
}*/



