#include <libya\vectorz.h>

typedef struct { unsigned short a, b, c, m; } _3dsface;
typedef struct {
  int vertices, faces;
  Vector *vertexlist;
  struct { float u, v;} *uvlist;
  _3dsface *facelist;
} _3dsmesh;
typedef struct { unsigned short id; long npos; } Chunkh;
typedef struct _3dsobjct { char *name; _3dsmesh *mesh; struct _3dsobjct *next;} _3dsobject;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float pos_x, pos_y, pos_z;
  } pos[];
} Postrack;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float rotation_rad;
    float axis_x, axis_y, axis_z;
  } rot[];
} Rottrack;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float scale_x, scale_y, scale_z;
  } scale[];
} Scltrack;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float camera_field_of_view;
  } fov[];
} Fovtrack;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float camera_roll;
  } roll[];
} Rolltrack;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float red, rgn, blu;
  } color[];
} Coltrack;

/*typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    cstr obj_name;
  } morph[];
} Morphtrack;*/

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float hotspot_ang;
  } hotspot[];
} Hottrack;

typedef struct {
  short flags;
  short unknown1[4];
  short keys;
  short unknown2;
  struct {
    short framenum;
    long unknown3;
    float falloff_ang;
  } falloff[];
} Falltrack;



typedef struct {
  Postrack *postrack;
  Rolltrack *rolltrack;
} Camerapath;

typedef struct {
  Postrack *postrack;
} Targetpath;

typedef struct {
  Postrack *postrack;
} Lightpath;





Mesh *read3ds(char *filename, Material *m);
//Mesh *gethierarcy();



