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
  int xp, yp, zp;
  int color;
  int tx, ty;
  float nx, ny, nz;
  float rnx, rny, rnz;
} vertex;

typedef struct {
  int v1, v2, v3;
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

void teematriisi(float m[3][3], float a, float b, float c);
void teematriisi2(float m[3][3], float dx, float dy, float dz);
void viiva(object *opu, int v1, int v2);
void klenzviiva(object *opu, int v1, int v2);
//void textureviiva(object *opu, int v1, int v2);
void taeyttaejae(ushort *dest, object *opu);
void klenzviivat(char *dest, object *opu);
void klenztaeyttaejae(ushort *dest, char *sourke, object *opu);
//void texturetaeyttaejae(object *opu);
void rotateobject(object *opu, float xrot, float yrot, float zrot,
                               float xpos, float ypos, float zpos);
void latex(object *opu, float xpos, float ypos, float zpos,
                        float xrot, float yrot, float zrot);
void latex2(object *opu, float xpos, float ypos, float zpos,
                         float xtgt, float ytgt, float ztgt);
void laskevektorinormaalit(object *opu);
void teemunkkirinkila(object *opu);



