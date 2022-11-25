#include "functs.h"
#include "core.h"
#include "user.h"
typedef unsigned short ushort;


#define cam_z 100
#define luku 666
#define pi 3.1415936536

typedef struct {
  float x, y, z;
} vector;

typedef struct {
  float x, y, z;
  float rx, ry, rz;
  float crx, cry, crz;
//  int xp, yp, zp;
  int color;
  //int tx, ty;
  float nx, ny, nz;
  float rnx, rny, rnz;
  Vertex2d p;
} vertex;

typedef struct {
  int v1, v2, v3;
  Material *m;
} face;

typedef struct {
  int koord;
  float color, zp;
  int tx, ty;
} alkup;

typedef struct {
  int koord;
  float color, zp;
  int tx, ty;
} loppup;

typedef struct {
  vertex *vlist;
  face *flist;
  alkup xbgn[200];
  loppup xend[200];
  int vcount;
  int fcount;
} object;

void risti(vector *d, vector *a, vector *b);
void normalisoi(vector *d);
void teematriisi(float m[3][3], float a, float b, float c);
void teematriisi2(float m[3][3], float dx, float dy, float dz);
void taeyttaejae(char *dest, object *opu);
void rotateobject(object *opu, float xrot, float yrot, float zrot,
                               float xpos, float ypos, float zpos);
void latex2(object *opu, float xpos, float ypos, float zpos,
                         float xtgt, float ytgt, float ztgt, float per);
void laskevektorinormaalit(object *opu);
void teedotit(char *dest, object *opu);
void teemunkkirinkila(object *opu);
void create_pallo(object *opu, Material *m);
void teepallo(object *opu, float t, float size);
void create_pallo2(object *opu, Material *m);
void teepallo2(object *opu, float t);
void teepiikkipallo(object *opu, float t);


