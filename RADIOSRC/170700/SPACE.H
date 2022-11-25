typedef struct _Mlist Mlist;
struct _Mlist {
  Mesh *o;
  Matrix *m;
  Mlist *next;
};

typedef struct _Llist Llist;
struct _Llist {
  Vector pos, xpos, col;
  Llist *next;
};


struct _Space {
//  void *zbuf;
  int x0, y0, x1, y1, xc, yc;
  Lfb dbuf;
  Matrix *cam, *cam_f, *cam_inv, *cam_f_inv;
  float zoom;
  Tri2d **lst1, **lst2;
  Plane *viewcone;
  int pl_cnt;
  int clrflag;
  Vector clrcol, amb;
  Mlist *mlist;
  Llist *llist;
};

