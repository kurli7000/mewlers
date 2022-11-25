struct _Frame {
  void *dbuf, *zbuf;
  int pitch, x0, y0, x1, y1, xc, yc;
  Matrix cam, cam_f, cam_inv, cam_f_inv;
  float zoom;
  Tri2d **lst1, **lst2;
  Plane *viewcone;
  int pl_cnt;
};

