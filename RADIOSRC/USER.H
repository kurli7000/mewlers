#include <stdio.h>
#include <libya\libya.h>
#include <libya\platform\fasm.h>
#include <libya\platform\asm.h>
#include <libya\vectorz.h>

typedef struct _Vertex2d Vertex2d;
typedef struct _Tri2d Tri2d;
typedef struct _Vertex3d Vertex3d;
typedef struct _Poly3d Poly3d;
typedef struct _Edge3d Edge3d;
typedef struct _Mesh Mesh;
typedef struct _Texture Texture;
typedef struct _Material Material;
typedef struct _Space Space;
typedef struct _Speis Speis;

enum { f_antialias=1, f_lines=2, f_polys=4, f_verts=8,
       f_lights=16, f_fog=32, f_phong=64, f_txt=128,
       f_noantialias=65536, f_nolines=131072, f_nopolys=262144, f_noverts=524288,
       f_nolights=1048576, f_nofog=2097152, f_nophong=4194304, f_notxt=8388608 };

Vertex2d *new_v2d(void);
Tri2d    *new_tri(void);
Vertex3d *new_v3d(void);
Edge3d   *new_edge(void);
Mesh     *new_mesh(void);
Texture  *new_txt(void);
Material *new_mat(void);
Space    *new_space(void);
Speis    *new_speis(void);

Vertex2d *save_v2d  (Vertex2d *x);
Tri2d    *save_tri  (Tri2d    *x);
Vertex3d *save_v3d  (Vertex3d *x);
Edge3d   *save_edge (Edge3d   *x);
Mesh     *save_mesh (Mesh     *x);
Texture  *save_txt  (Texture  *x);
Speis    *save_speis(Speis    *x);
Material *save_mat  (Material *x);
Space    *save_space(Space    *x);

Vertex2d *destroy_v2d  (Vertex2d *x);
Tri2d    *destroy_tri  (Tri2d    *x);
Vertex3d *destroy_v3d  (Vertex3d *x);
Edge3d   *destroy_edge (Edge3d   *x);
Mesh     *destroy_mesh (Mesh     *x);
Texture  *destroy_txt  (Texture  *x);
Speis    *destroy_speis(Speis    *x);
Material *destroy_mat  (Material *x);
Space    *destroy_space(Space    *x);



void mesh_draw(Space *f, Mesh *o, Matrix *m, int flgs);
Mesh *mesh_clip_in(Mesh *o, Plane *pl, int pc);
Mesh *mesh_clip_out(Mesh *o, Plane *pl, int pc);
Mesh *mesh_flipnormals(Mesh *o);
int mesh_to_planes(Plane *pl, Mesh *o, Matrix *m);
Mesh *mesh_set_mat(Mesh *o, Material *m);

Texture *txt_load(Texture *t, void *data, int xs, int ys, int sb, int ds, int db, float gamma);
Texture *new_txt_load_tga(char *fname, float gamma);
Texture *new_txt_load_mfb(char *fname, float gamma);

Material *mat_something(Material *m);
Material *mat_set_txt1(Material *m, Texture *t);
Material *mat_set_speis(Material *m, Speis *s);
Material *mat_set_refl(Material *m);
Material *mat_set_filler(Material *m, void *filler);


Space *new_space();
Space *space_setambient(Space *f, Vector *col);
Space *space_addmesh(Space *f, Mesh *o, Matrix *m);
Space *space_addlight(Space *f, Vector *pos, Vector *col);
Space *space_render(Space *f, Lfb *lfb, Matrix *cam, float zoom, int flgs);
Space *space_clear(Space *s);
Space *space_fill(Space *s, Vector *col);


void teeplane(Plane *d, Vector *a, Vector *b, Vector *c);
void teeplane2(Plane *d, Vector *p, Vector *v1, Vector *v2);





extern float monitor_gamma;
extern int use_lfb;


