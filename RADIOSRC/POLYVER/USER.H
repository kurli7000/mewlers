#include <stdio.h>
#include <lib2\bugs.h>
#include <lib2\fasm.h>
#include <lib2\asm.h>
#include <lib2\vectorz.h>
#include <lib2\mem.h>
#include "video.h"

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

Vertex2d *_new_v2d(void);
Tri2d    *_new_tri(void);
Vertex3d *_new_v3d(void);
Edge3d   *_new_edge(void);
Mesh     *_new_mesh(void);
Texture  *_new_txt(void);
Material *_new_mat(void);
Space    *_new_space(void);
Speis    *_new_speis(void);

Vertex2d *_save_v2d  (Vertex2d *x);
Tri2d    *_save_tri  (Tri2d    *x);
Vertex3d *_save_v3d  (Vertex3d *x);
Edge3d   *_save_edge (Edge3d   *x);
Mesh     *_save_mesh (Mesh     *x);
Texture  *_save_txt  (Texture  *x);
Speis    *_save_speis(Speis    *x);
Material *_save_mat  (Material *x);
Space    *_save_space(Space    *x);

Vertex2d *_destroy_v2d  (Vertex2d *x);
Tri2d    *_destroy_tri  (Tri2d    *x);
Vertex3d *_destroy_v3d  (Vertex3d *x);
Edge3d   *_destroy_edge (Edge3d   *x);
Mesh     *_destroy_mesh (Mesh     *x);
Texture  *_destroy_txt  (Texture  *x);
Speis    *_destroy_speis(Speis    *x);
Material *_destroy_mat  (Material *x);
Space    *_destroy_space(Space    *x);



void _mesh_draw(Space *f, Mesh *o, Matrix *m, int flgs);
Mesh *_mesh_clip_in(Mesh *o, Plane *pl, int pc);
Mesh *_mesh_clip_out(Mesh *o, Plane *pl, int pc);
Mesh *_mesh_flipnormals(Mesh *o);
int _mesh_to_planes(Plane *pl, Mesh *o, Matrix *m);
Mesh *_mesh_set_mat(Mesh *o, Material *m);

Texture *_txt_load(Texture *t, void *data, int xs, int ys, int sb, int ds, int db, float gamma);
Texture *_new_txt_load_tga(char *fname, float gamma);
Texture *_new_txt_load_mfb(char *fname, float gamma);

Material *_mat_something(Material *m);
Material *_mat_set_txt1(Material *m, Texture *t);
Material *_mat_set_speis(Material *m, Speis *s);
Material *_mat_set_refl(Material *m);
Material *_mat_set_filler(Material *m, void *filler);


Space *_new_space();
Space *_space_setambient(Space *f, Vector *col);
Space *_space_addmesh(Space *f, Mesh *o, Matrix *m);
Space *_space_addlight(Space *f, Vector *pos, Vector *col);
Space *_space_render(Space *f, Lfb *lfb, Matrix *cam, float zoom, int flgs);
Space *_space_clear(Space *s);
Space *_space_fill(Space *s, Vector *col);
/*
Space *_space_open(Space *f, Lfb *lfb, Matrix *camera, float zoom);
Space *_space_mmul(Space *f, Matrix *m);
Space *_space_addplane(Space *f, Plane *p);
Space *_space_close(Space *f);
Space *_space_new_subf(Space *a);

Speis *_speis_addobj(Speis *s, Mesh *o, Matrix *m);
Speis *_speis_addsimplecut(Speis *s, Mesh *o1, Matrix *m1, Material *mat1, Mesh *o2, Matrix *m2);
void _speis_draw(Space *f, Speis *s);
*/


void teeplane(Plane *d, Vector *a, Vector *b, Vector *c);
void teeplane2(Plane *d, Vector *p, Vector *v1, Vector *v2);





#define new_v2d() bugchked(new_v2d())
//#define nnew_v2d(a) bugchked(nnew_v2d(a))
#define new_tri() bugchked(new_tri())
//#define nnew_tri(a) bugchked(nnew_tri(a))
#define new_v3d() bugchked(new_v3d())
//#define nnew_v3d(a) bugchked(nnew_v3d(a))
//#define nnew_poly(a, b) bugchked(nnew_poly(a, b))
#define new_edge() bugchked(new_edge())
//#define nnew_edge(a) bugchked(nnew_edge(a))
#define new_mesh() bugchked(new_mesh())
#define new_txt() bugchked(new_txt())
#define new_mat() bugchked(new_mat())
#define new_space() bugchked(new_space())
#define save_v2d(a) bugchked(save_v2d(a))
#define save_tri(a) bugchked(save_tri(a))
#define save_v3d(a) bugchked(save_v3d(a))
#define save_edge(a) bugchked(save_edge(a))
#define save_mesh(a) bugchked(save_mesh(a))
#define save_txt(a) bugchked(save_txt(a))
#define save_speis(a) bugchked(save_speis(a))
#define save_mat(a) bugchked(save_mat(a))
#define save_space(a) bugchked(save_space(a))
#define destroy_v2d(a) bugchked(destroy_v2d(a))
#define destroy_tri(a) bugchked(destroy_tri(a))
#define destroy_v3d(a) bugchked(destroy_v3d(a))
#define destroy_edge(a) bugchked(destroy_edge(a))
#define destroy_mesh(a) bugchked(destroy_mesh(a))
#define destroy_txt(a) bugchked(destroy_txt(a))
#define destroy_speis(a) bugchked(destroy_speis(a))
#define destroy_mat(a) bugchked(destroy_mat(a))
#define destroy_space(a) bugchked(destroy_space(a))
#define mesh_draw(a, b, c, d) bugchked(mesh_draw(a, b, c, d))
#define mesh_clip_in(a, b, c) bugchked(mesh_clip_in(a, b, c))
#define mesh_clip_out(a, b, c) bugchked(mesh_clip_out(a, b, c))
#define mesh_flipnormals(a) bugchked(mesh_flipnormals(a))
#define mesh_to_planes(a, b, c) bugchked(mesh_to_planes(a, b, c))
#define mesh_set_mat(a, b) bugchked(mesh_set_mat(a, b))
#define txt_load(a, b, c, d, e, f, g, h) bugchked(txt_load(a, b, c, d, e, f, g, h))
#define new_txt_load_tga(a, b) bugchked(new_txt_load_tga(a, b))
#define new_txt_load_mfb(a, b) bugchked(new_txt_load_mfb(a, b))
#define mat_something(a) bugchked(mat_something(a))
#define mat_set_txt1(a, b) bugchked(mat_set_txt1(a, b))
#define mat_set_speis(a, b) bugchked(mat_set_speis(a, b))
#define mat_set_refl(a) bugchked(mat_set_refl(a))
#define mat_set_filler(a, b) bugchked(mat_set_filler(a, b))

#define new_space() bugchked(new_space())
#define space_setambient(a, b) bugchked(space_setambient(a, b))
#define space_addmesh(a, b, c) bugchked(space_addmesh(a, b, c))
#define space_addlight(a, b, c) bugchked(space_addlight(a, b, c))
#define space_render(a, b, c, d, e) bugchked(space_render(a, b, c, d, e))
#define space_clear(a) bugchked(space_clear(a))
#define space_fill(a, b) bugchked(space_fill(a, b))

extern float monitor_gamma;
extern int use_lfb;


