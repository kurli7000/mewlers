#include "phile.h"

Mesh *new_mesh_alloc(int polys, int verts);
Mesh *mesh_v_set(Mesh *o, int i, float x, float y, float z, float u, float v);
Mesh *mesh_v_swap(Mesh *o, int i, int j);
Mesh *mesh_v_reorder(Mesh *o, int i, int j);
float mesh_p_dist(Mesh *o, int i, int j);
Mesh *mesh_p_calc_normal(Mesh *o, int i);
Mesh *mesh_p_swap(Mesh *o, int i, int j);
Mesh *mesh_p_set3(Mesh *o, int i, Material *m, int v1, int v2, int v3);
Mesh *mesh_p_set4(Mesh *o, int i, Material *m, int v1, int v2, int v3, int v4);
Mesh *mesh_calc_bound(Mesh *o);
Mesh *mesh_calc_edges(Mesh *o);
Mesh *mesh_calc_normals(Mesh *o);
Mesh *mesh_reorder(Mesh *o);
Mesh *mesh_recalc(Mesh *o);
Mesh *mesh_wraparound(Mesh *o);
Mesh *mesh_killempty(Mesh *o);
Mesh *mesh_check(Mesh *o);

Mesh *new_mesh_read(Phile *ph, Material *m);
Mesh *mesh_write(Mesh *o, Phile *ph);

struct _Vertex2d {
  int x, y, r, g, b;
  //union { float uz; int u; };
  //union { float vz; int v; };
  float u, v, z;
};
struct _Tri2d {
  Vertex2d *top, *mid, *bot;
  Material *m;
  int c;
  int z;
  Tri2d *next;
};
struct _Vertex3d {
  Vector pos, n;
  int u, v, clip, flag;
  Vertex2d *v2d;
};
struct _Poly3d {
  Material *m;
  Plane p;
  int flag;
  int vc; Vertex3d *vl[];
};
struct _Edge3d {
  Vertex3d *v1, *v2;
  Poly3d *p1, *p2;
  Edge3d *next;
  int flag;
};
struct _Mesh {
  int vc, pc, ec, aec, flgs;
  Poly3d **pl;//, *pl_end;
  Vertex3d **vl;//, *vl_end;
  Edge3d **el;//, *el_end;
  Edge3d **ael;//, *el_end;
  //Array *polys, *vertices, *edges;
  Vector box0, box1;
};



