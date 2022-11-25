#include "core.h"
#include "frame.pri"


Frame *_new_frame() {
  Frame *x=getmem(sizeof(*x));
  x->lst1=getmem(sizeof(Tri2d*)*1024);
  x->lst2=getmem(sizeof(Tri2d*)*1024);
  return x;
}
Frame *_save_frame(Frame *x) { x=save(x, sizeof(*x)); return x; }
Frame *_destroy_frame(Frame *x) { return destroy(x); }



Frame *_frame_addplane(Frame *f, Plane *p) { f->viewcone[f->pl_cnt++]=*p; return f; }
Frame *_frame_open(Frame *f, void *dbuf, int pitch, int x0, int y0, int x1, int y1, Matrix *camera, float zoom) {
  Vector k, l, v1, v2, v3, v4;
  int i;
  f->cam=*camera;

  stosd(dbuf, 0, pitch*y1>>1);
  f->dbuf=dbuf; f->pitch=pitch;
  f->x0=x0<<12; f->x1=x1+1<<12; f->xc=x0+x1+1<<11;
  f->y0=y0<<12; f->y1=y1+1<<12; f->yc=y0+y1+1<<11;
  f->viewcone=getmem(sizeof(Plane)*32); f->pl_cnt=0;

  v_create(&v4, f->x0+1024, f->y1-1024, 1);
  v_create(&v1, f->x0+1024, f->y0+1024, 1);
  v_create(&v2, f->x1-1024, f->y0+1024, 1);
  v_create(&v3, f->x1-1024, f->y1-1024, 1);
  teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v1, &v4);
  teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v2, &v1);
  teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v3, &v2);
  teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v4, &v3);
  teeplane(f->viewcone+f->pl_cnt++, &v_c0, &v_cj, &v_ci);

  m_inv(&f->cam_inv, &f->cam);
  v_create(&k, (f->x1-f->x0)*3>>2, f->y0-f->y1, f->zoom=1.0/zoom); v_mul(&k, &k, 1/65536.0);
  v_vmul(&f->cam_f_inv.i, &f->cam_inv.i, &k); v_vmul(&f->cam_f_inv.j, &f->cam_inv.j, &k);
  v_vmul(&f->cam_f_inv.k, &f->cam_inv.k, &k); v_vmul(&f->cam_f_inv.l, &f->cam_inv.l, &k);
  v_create(&k, f->xc, f->yc, 0);
  v_add(&f->cam_f_inv.i, &f->cam_f_inv.i, v_mul(&l, &k, f->cam_f_inv.i.z));
  v_add(&f->cam_f_inv.j, &f->cam_f_inv.j, v_mul(&l, &k, f->cam_f_inv.j.z));
  v_add(&f->cam_f_inv.k, &f->cam_f_inv.k, v_mul(&l, &k, f->cam_f_inv.k.z));
  v_add(&f->cam_f_inv.l, &f->cam_f_inv.l, v_mul(&l, &k, f->cam_f_inv.l.z));
  m_inv(&f->cam_f, &f->cam_f_inv);

  for (i=0; i<1024; i++) f->lst1[i]=0;
  for (i=0; i<1024; i++) f->lst2[i]=0;
  return f;
}
Frame *_frame_new_subf(Frame *a) {
  Frame *f=new_frame();
  int i;
  f->dbuf=a->dbuf; f->zbuf=a->zbuf;
  f->pitch=a->pitch;
  f->x0=a->x0; f->x1=a->x1; f->xc=a->xc;
  f->y0=a->y0; f->y1=a->y1; f->yc=a->yc; f->zoom=a->zoom;
  f->cam=a->cam; f->cam_f=a->cam_f;
  f->cam_inv=a->cam_inv; f->cam_f_inv=a->cam_f_inv;
  for (i=0; i<1024; i++) f->lst1[i]=0;
  for (i=0; i<1024; i++) f->lst2[i]=0;
  f->viewcone=getmem(sizeof(Plane)*32); f->pl_cnt=0;
  for (i=0; i<a->pl_cnt; i++) frame_addplane(f, a->viewcone+i);
  return f;
}
Frame *_frame_close(Frame *f) {
  triprep(f);
  tri_sort(f->lst1, f->lst2);
  return f;
}
Frame *_frame_mmul(Frame *f, Matrix *m) {
  Vector k, l, v1, v2, v3, v4;
  Matrix m2;
  //m_copy(&f->cam, m_mmul(&m2, &f->cam, m));
  m_copy(&f->cam, m);

  m_inv(&f->cam_inv, &f->cam);
  v_create(&k, (f->x1-f->x0)*3>>2, f->y0-f->y1, f->zoom); v_mul(&k, &k, 1/65536.0);
  v_vmul(&f->cam_f_inv.i, &f->cam_inv.i, &k); v_vmul(&f->cam_f_inv.j, &f->cam_inv.j, &k);
  v_vmul(&f->cam_f_inv.k, &f->cam_inv.k, &k); v_vmul(&f->cam_f_inv.l, &f->cam_inv.l, &k);
  v_create(&k, f->xc, f->yc, 0);
  v_add(&f->cam_f_inv.i, &f->cam_f_inv.i, v_mul(&l, &k, f->cam_f_inv.i.z));
  v_add(&f->cam_f_inv.j, &f->cam_f_inv.j, v_mul(&l, &k, f->cam_f_inv.j.z));
  v_add(&f->cam_f_inv.k, &f->cam_f_inv.k, v_mul(&l, &k, f->cam_f_inv.k.z));
  v_add(&f->cam_f_inv.l, &f->cam_f_inv.l, v_mul(&l, &k, f->cam_f_inv.l.z));
  m_inv(&f->cam_f, &f->cam_f_inv);
  return f;
}



