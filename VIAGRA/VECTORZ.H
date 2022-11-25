typedef struct {
  float x,y,z;
} Vector;

Vector *vcreate(Vector *d, float x, float y, float z);
Vector *vcopy(Vector *d, Vector *a);
Vector *vadd(Vector *d, Vector *a, Vector *b);
Vector *vsub(Vector *d, Vector *a, Vector *b);
Vector *vmul(Vector *d, Vector *a, float k);
Vector *vvmul(Vector *d, Vector *a, Vector *b);
float vdot(Vector *a, Vector *b);
Vector *vcross(Vector *d, Vector *a, Vector *b);
Vector *vnorm(Vector *d);

