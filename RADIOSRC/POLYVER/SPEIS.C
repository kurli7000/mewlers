#include "core.h"
#include "space.pri"

Space *_new_space() { Space *x=getmem(sizeof(*x)); x->o=0; x->next=0; return x; }
Space *_save_space(Space *x) { x=save(x, sizeof(*x)); return x; }
Space *_destroy_space(Space *x) { return destroy(x); }


Space *_space_addobj(Space *s, Mesh *o, Matrix *m) {
  Space *s2;
  if (s->o) {
    s2=new_space(); s2->next=s->next; s->next=s2;
  } else s2=s;
  s2->o=o; s2->m=m;
  return s;
}
Space *_space_addsimplecut(Space *s, Mesh *o1, Matrix *m1, Material *mat1, Mesh *o2, Matrix *m2) {
  static Matrix tmp;
  static Plane lst[32];
  static int lc;

  lc=mesh_to_planes(lst, o1, m_mdiv(&tmp, m2, m1));
  space_addobj(s, mesh_flipnormals(mesh_clip_in(o2, lst, lc)), m2);
  space_addobj(s, mesh_set_mat(mesh_clip_out(o2, lst, lc), mat1), m2);
  lc=mesh_to_planes(lst, o2, m_mdiv(&tmp, m1, m2));
  space_addobj(s, mesh_clip_out(o1, lst, lc), m1);
  return s;
}
void _space_draw(Frame *f, Space *s) {
  for (; s && s->o; s=s->next) mesh_draw(f, s->o, s->m);
}









