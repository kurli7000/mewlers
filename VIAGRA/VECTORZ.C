#include <tinymath.h>
#include "vectorz.h"

Vector *vcreate(Vector *d, float x, float y, float z) {
  d->x=x; d->y=y; d->z=z; return d;
}
Vector *vcopy(Vector *d, Vector *a) {
  d->x=a->x; d->y=a->y; d->z=a->z; return d;
}
Vector *vadd(Vector *d, Vector *a, Vector *b) {
  d->x=a->x+b->x; d->y=a->y+b->y; d->z=a->z+b->z; return d;
}
Vector *vsub(Vector *d, Vector *a, Vector *b) {
  d->x=a->x-b->x; d->y=a->y-b->y; d->z=a->z-b->z; return d;
}
Vector *vmul(Vector *d, Vector *a, float k) {
  d->x=a->x*k; d->y=a->y*k; d->z=a->z*k; return d;
}
Vector *vvmul(Vector *d, Vector *a, Vector *b) {
  d->x=a->x*b->x; d->y=a->y*b->y; d->z=a->z*b->z; return d;
}
float vdot(Vector *a, Vector *b) { return a->x*b->x+a->y*b->y+a->z*b->z; }
Vector *vcross(Vector *d, Vector *a, Vector *b) {
  d->x=a->y*b->z-a->z*b->y; d->y=a->z*b->x-a->x*b->z; d->z=a->x*b->y-a->y*b->x;
  return d;
}
Vector *vnorm(Vector *d) {
  return vmul(d, d, 1.0/fsqrt(vdot(d, d)));
}

