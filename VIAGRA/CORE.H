#include "user.h"

struct _Vertex2d {
  int x, y, z, u, v, clip;
};
struct _Tri2d {
  Vertex2d *top, *mid, *bot;
  Material *m;
  Tri2d *next;
};
struct _Vertex3d {
  float x, y, z;
  int u, v;
};
struct _Poly3d {
  Material *m;
  int vc;
  Vertex3d *vl[];
};
struct _Texture {
  union { char *data; short *wdata; long *ddata; };
};
struct _Material {
  Texture txt1, txt2;
  void (*fill)(Tri2d *t);
};

void zfilltri(Tri2d *t); //Vertex2d *v1, Vertex2d *v2, Vertex2d *v3);
void triprep(char *buf, unsigned short *zbuf);
void sel_mat(Material *m);


