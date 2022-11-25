struct _Texture {
  short *bits16d, *bits16l;
  int *bits32d;
  int size;
  //int raja;
  Texture *mipnext;
};
struct _Material {
  void (*fill)(Tri2d *t);
  Texture txt1, txt2;
  int refl;
  Space *space;
};

