#include "user.h"

#define poly_next(p) ((Poly3d*)((p)->vl+(p)->vc))

void filltri(Tri2d *t);
void filltri_cthru(Tri2d *t);
void portaltri(Tri2d *t);
void drawline(Tri2d *t);
void triprep(Space *f);
void filllst(Tri2d *l);
void tri_sort(Tri2d **lst1, Tri2d **lst2);



