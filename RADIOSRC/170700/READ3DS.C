#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "core.h"
#include "mesh.h"
#include "obuja.h"
#include "read3ds.h"

Material *mater;
struct { unsigned short id; char *cmt; } blockdesc[]=
 {{0x4d4d, "3ds main"},

  {0x3d3d, "3ds editor data"},
  {0xb000, "KFDATA"},

  {0x1100, "unknown"},
  {0x1200, "background colour."},
  {0x1201, "unknown"},
  {0x1300, "unknown"},
  {0x1400, "unknown"},
  {0x1420, "unknown"},
  {0x1450, "unknown"},
  {0x1500, "unknown"},
  {0x2100, "ambient colour block"},
  {0x2200, "fog?"},
  {0x2201, "fog?"},
  {0x2210, "fog?"},
  {0x2300, "unknown"},
  {0x3000, "unknown"},
  {0x4000, "object block"},
  {0x7001, "viewports"},
  {0xafff, "unknown"},
  {0x4010, "unknown"},
  {0x4012, "shadow?"},
  {0x4100, "triangular polygon object"},
  {0x4600, "light"},
  {0x4700, "camera"},
  {0x4110, "vertex list"},
  {0x4111, "unknown"},
  {0x4120, "face list"},
  {0x4140, "map coords list"},
  {0x4160, "translation matrix"},

  {0x0010, "RGB colour"},
  {0x0011, "24 bit colour"},
  {0x4610, "spot light"},
  {0x4620, "light is off"},

  {0xb001, "AMBIENT_NODE_TAG"},
  {0xb002, "OBJECT_NODE_TAG"},
  {0xb003, "CAMERA_NODE_TAG"},
  {0xb004, "TARGET_NODE_TAG"},
  {0xb005, "LIGHT_NODE_TAG"},
  {0xb006, "L_TARGET_NODE_TAG"},
  {0xb007, "SPOTLIGHT_NODE_TAG"},
  {0xb008, "KFSEG"},
  {0xb009, "KFCURTIME"},
  {0xb00a, "KFHDR"},
  {0xb010, "NODE_HDR"},
  {0xb011, "INSTANCE_NAME"},
  {0xb012, "PRESCALE"},
  {0xb013, "PIVOT"},
  {0xb014, "BOUNDBOX"},
  {0xb015, "MORPH_SMOOTH"},
  {0xb020, "POS_TRACK_TAG"},
  {0xb021, "ROT_TRACK_TAG"},
  {0xb022, "SCL_TRACK_TAG"},
  {0xb023, "FOV_TRACK_TAG"},
  {0xb024, "ROLL_TRACK_TAG"},
  {0xb025, "COL_TRACK_TAG"},
  {0xb026, "MORPH_TRACK_TAG"},
  {0xb027, "HOT_TRACK_TAG"},
  {0xb028, "FALL_TRACK_TAG"},
  {0xb029, "HIDE_TRACK_TAG"},
  {0xb030, "NODE_ID"},
};


Mesh *msh;


//_3dsobject *allobjs=0;

int indent=0;
int lcounter=0;

Camerapath *_camerapath;
Targetpath *_targetpath;
Lightpath *_lightpath;






void more() {/* if (++lcounter>=21) { while (*(volatile short*)1050==*(volatile short*)1052) ; *(volatile short*)1050=*(volatile short*)1052; lcounter=0; }*/ }
//void spc1() { int i; for (i=indent*2; i>0; i--) printf(" "); }
//void spc2() { int i; for (i=indent*2+16; i>0; i--) printf(" "); }


void read3dsblock(FILE *f, Chunkh *h) {
  int i;

  fread(h, 6, 1, f);
  h->npos-=6;

/*  for (i=indent; i>0; i--) printf("  ");
  printf("%4x%8i    ", h->id, h->npos);
  spc1();
  for (i=0; i<sizeof(blockdesc)/sizeof(blockdesc[0]); i++)
    if (blockdesc[i].id==h->id) { printf(blockdesc[i].cmt); break; }
  if (i==sizeof(blockdesc)/sizeof(blockdesc[0])) printf(".");
  printf("\n");
*/
  h->npos+=ftell(f);
  more();
}






void dispsubblocks(FILE *f, long skipbytes, long epos) {
  Chunkh h;

  indent++;
  fseek(f, skipbytes, SEEK_CUR);
  do {
    read3dsblock(f, &h);
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
}







void read3dsmesh(FILE *f, _3dsmesh *m, long epos) {
  Chunkh h;
  unsigned short temp;
  int i;
  int uvverts=0;

  indent++;
  m->vertices=0;
  m->faces=0;
  m->vertexlist=0;
  m->facelist=0;
  m->uvlist=0;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0x4110:
        fread(&temp, 2, 1, f);
        //spc2(); printf("%u vertices\r\n", temp); more();
        m->vertices=temp;
        m->vertexlist=malloc(temp*sizeof(*m->vertexlist));
        fread(m->vertexlist, sizeof(*m->vertexlist), temp, f);
        //read3dsvlist(f, m);
        break;
      case 0x4120:
        fread(&temp, 2, 1, f);
        //spc2(); printf("%u faces\r\n", temp); more();
        m->faces=temp;
        m->facelist=malloc(temp*sizeof(*m->facelist));
        fread(m->facelist, sizeof(*m->facelist), temp, f);
//      read3dsplist(f, m);
        break;
      case 0x4140:
        fread(&temp, 2, 1, f);
//        spc2(); printf("%u vertices\r\n", temp); more();
        m->uvlist=malloc(temp*sizeof(*m->uvlist));
        uvverts=temp;
        fread(m->uvlist, sizeof(*m->uvlist), temp, f);
//      read3dsmplist(f, m);
        break;
//    case 0x4160: read3dsmatrix(f, m^.transform); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
  msh=new_mesh_alloc(m->faces, m->vertices);
  for (i=0; i<m->vertices; i++)
    mesh_v_set(msh, i, m->vertexlist[i].x,
      m->vertexlist[i].y, m->vertexlist[i].z, i<uvverts?m->uvlist[i].u:0, i<uvverts?m->uvlist[i].v:0);
  for (i=0; i<m->faces; i++)
    mesh_p_set3(msh, i, mater, m->facelist[i].a,
      m->facelist[i].b, m->facelist[i].c);
  msh=mesh_recalc(msh);
}


/*void read3dslight(FILE *f, long epos) {
Chunkh h;
  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
}*/



void read3dscamera(FILE *f) {
Chunkh h;
  indent++;
  fseek(f, 32, SEEK_CUR);
  indent--;
}








void read3dsobject(FILE *f, _3dsobject *o, long epos) {
  Chunkh h;

  indent++;
  while (getc(f)!=0) ;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0x4100: read3dsmesh(f, o->mesh=malloc(sizeof(_3dsmesh)), h.npos); break;
      case 0x4600: dispsubblocks(f, 12, h.npos); break;
//      case 0x4600: read3dslight(f, h.npos); break;
      case 0x4700: read3dscamera(f); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
}





_3dsobject *newobject() {
  _3dsobject *o;

  o=malloc(sizeof(_3dsobject));
//  if (o==0) {printf("perkele.\r\n"); more(); exit(1); }
  o->mesh=0;
  //o->next=allobjs;
  //allobjs=o;
  return o;
}


void read3dsedit(FILE *f, long epos) {
  Chunkh h;

  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0x4000: read3dsobject(f, newobject(), h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
}


void *readstuff(FILE *f, long epos) {
  void *res;
  int sz;

  sz=epos-ftell(f);
  res=malloc(sz);
  fread(res, sz, 1, f);
  return res;
}

Camerapath *readcamerapath(FILE *f, long epos) {
  Chunkh h;
  Camerapath *cpth;

  cpth=malloc(sizeof(Camerapath));
  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0xb020: cpth->postrack=readstuff(f, h.npos); break;
      case 0xb024: cpth->rolltrack=readstuff(f, h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
  return cpth;
}

Targetpath *readtargetpath(FILE *f, long epos) {
  Chunkh h;
  Targetpath *tpth;

  tpth=malloc(sizeof(Targetpath));
  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0xb020: tpth->postrack=readstuff(f, h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
  return tpth;
}

Lightpath *readlightpath(FILE *f, long epos) {
  Chunkh h;
  Lightpath *lpth;

  lpth=malloc(sizeof(Lightpath));
  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0xb020: lpth->postrack=readstuff(f, h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
  return lpth;
}

void read3dskeyf(FILE *f, long epos) {
  Chunkh h;

  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0xb003: _camerapath=readcamerapath(f, h.npos); break;
      case 0xb004: _targetpath=readtargetpath(f, h.npos); break;
      case 0xb005: _lightpath=readlightpath(f, h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
}



/*
Material defaultmat={realpoly_flatz};
Material *cur_mat=&defaultmat;


Mesh *preprocess(_3dsmesh *meshi) {
  Mesh *s;
  int i;
  float x, y, z, r, r2;

  s=malloc32(sizeof(Mesh));
  s->vertices=meshi->vertices;
  s->vertexlist=malloc32(s->vertices*sizeof(Vertex));
  for(i=0; i<s->vertices; i++) {
    s->vertexlist[i].x=meshi->vertexlist[i].x;
    s->vertexlist[i].y=meshi->vertexlist[i].y;
    s->vertexlist[i].z=meshi->vertexlist[i].z;
    if (meshi->uvlist==0) {
      s->vertexlist[i].u=rand()*512.0+rand()-8388608;
      s->vertexlist[i].v=rand()*512.0+rand()-8388608;
    } else {
      s->vertexlist[i].u=meshi->uvlist[i].u*16777216.0;
      s->vertexlist[i].v=meshi->uvlist[i].v*16777216.0;
    }
/o    s->vertexlist[i].u=(meshi->vertexlist[i].x)*70000.0;
    s->vertexlist[i].v=(meshi->vertexlist[i].y+meshi->vertexlist[i].z)*70000.0; o/
    s->vertexlist[i].nx=0;
    s->vertexlist[i].ny=0;
    s->vertexlist[i].nz=0;
  }

  s->faces=meshi->faces;
  s->facelist=malloc32(s->faces*sizeof(Face));
  for(i=0; i<s->faces; i++) {
    s->facelist[i].mat=cur_mat;
    s->facelist[i].a=s->vertexlist+meshi->facelist[i].a;
    s->facelist[i].b=s->vertexlist+meshi->facelist[i].b;
    s->facelist[i].c=s->vertexlist+meshi->facelist[i].c;
    s->facelist[i].nx=
      (s->facelist[i].c->y-s->facelist[i].a->y)*
      (s->facelist[i].b->z-s->facelist[i].a->z)-
      (s->facelist[i].c->z-s->facelist[i].a->z)*
      (s->facelist[i].b->y-s->facelist[i].a->y);
    s->facelist[i].ny=
      (s->facelist[i].c->z-s->facelist[i].a->z)*
      (s->facelist[i].b->x-s->facelist[i].a->x)-
      (s->facelist[i].c->x-s->facelist[i].a->x)*
      (s->facelist[i].b->z-s->facelist[i].a->z);
    s->facelist[i].nz=
      (s->facelist[i].c->x-s->facelist[i].a->x)*
      (s->facelist[i].b->y-s->facelist[i].a->y)-
      (s->facelist[i].c->y-s->facelist[i].a->y)*
      (s->facelist[i].b->x-s->facelist[i].a->x);
    r=1/sqrt(sqr(s->facelist[i].nx)+sqr(s->facelist[i].ny)+sqr(s->facelist[i].nz));
    s->facelist[i].a->nx+=s->facelist[i].nx;
    s->facelist[i].b->nx+=s->facelist[i].nx;
    s->facelist[i].c->nx+=s->facelist[i].nx;
    s->facelist[i].a->ny+=s->facelist[i].ny;
    s->facelist[i].b->ny+=s->facelist[i].ny;
    s->facelist[i].c->ny+=s->facelist[i].ny;
    s->facelist[i].a->nz+=s->facelist[i].nz;
    s->facelist[i].b->nz+=s->facelist[i].nz;
    s->facelist[i].c->nz+=s->facelist[i].nz;
    s->facelist[i].nx*=r;
    s->facelist[i].ny*=r;
    s->facelist[i].nz*=r;
    s->facelist[i].dist=
      s->facelist[i].nx*s->facelist[i].a->x+
      s->facelist[i].ny*s->facelist[i].a->y+
      s->facelist[i].nz*s->facelist[i].a->z;

    x=(s->facelist[i].a->x+s->facelist[i].b->x+s->facelist[i].c->x)*(1.0/3.0)-50;
    y=(s->facelist[i].a->y+s->facelist[i].b->y+s->facelist[i].c->y)*(1.0/3.0)-80;
    z=(s->facelist[i].a->z+s->facelist[i].b->z+s->facelist[i].c->z)*(1.0/3.0)-30;
    r=223.0/sqrt(x*x+y*y+z*z);
    r*=(x*s->facelist[i].nx+y*s->facelist[i].ny+z*s->facelist[i].nz);
    if (r<0) r=0;
    if (r>223) r=223;
    r+=32;
    s->facelist[i].color=
      ((int)(r*(0.25+rand()/65536.0)))+
      ((int)(r*(0.25+rand()/65536.0))<<8)+
      ((int)(r*(0.00+rand()/131072.0))<<16);


/o    s->facelist[i].u1=s->facelist[i].a->u;
    s->facelist[i].v1=s->facelist[i].a->v;
    s->facelist[i].u21=s->facelist[i].b->u-s->facelist[i].u1;
    s->facelist[i].v21=s->facelist[i].b->v-s->facelist[i].v1;
    s->facelist[i].u31=s->facelist[i].c->u-s->facelist[i].u1;
    s->facelist[i].v31=s->facelist[i].c->v-s->facelist[i].v1;
    while (s->facelist[i].u21>192*65536.0) s->facelist[i].u21-=256*65536.0;
    while (s->facelist[i].v21>192*65536.0) s->facelist[i].v21-=256*65536.0;
    while (s->facelist[i].u31>192*65536.0) s->facelist[i].u31-=256*65536.0;
    while (s->facelist[i].v31>192*65536.0) s->facelist[i].v31-=256*65536.0;
    while (s->facelist[i].u21<-192*65536.0) s->facelist[i].u21+=256*65536.0;
    while (s->facelist[i].v21<-192*65536.0) s->facelist[i].v21+=256*65536.0;
    while (s->facelist[i].u31<-192*65536.0) s->facelist[i].u31+=256*65536.0;
    while (s->facelist[i].v31<-192*65536.0) s->facelist[i].v31+=256*65536.0; o/
  }
  for(i=0; i<s->vertices; i++) {
    r=1/sqrt(sqr(s->vertexlist[i].nx)+sqr(s->vertexlist[i].ny)+sqr(s->vertexlist[i].nz));
    s->vertexlist[i].nx=s->vertexlist[i].nx*r+s->vertexlist[i].x*0.00;
    s->vertexlist[i].ny=s->vertexlist[i].ny*r+s->vertexlist[i].y*0.00;
    s->vertexlist[i].nz=s->vertexlist[i].nz*r+s->vertexlist[i].z*0.00;

    r=1/sqrt(sqr(s->vertexlist[i].nx)+sqr(s->vertexlist[i].ny)+sqr(s->vertexlist[i].nz));
    s->vertexlist[i].nx*=r;
    s->vertexlist[i].ny*=r;
    s->vertexlist[i].nz*=r;
  }
  s->next=0;
  return s;
}









void read3ds(char *filename) {
Chunkh h;
FILE *f;
long epos;

  f=fopen(filename, "rb");
  allobjs=0;

  if (f==0) return;
  read3dsblock(f, &h);

  if (h.id!=0x4d4d) return;
  epos=h.npos;

  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0x3d3d: read3dsedit(f, h.npos); break;
      case 0xb000: read3dskeyf(f, h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;

  fclose(f);
}




Mesh *gethierarcy() {
  _3dsobject *o;
  Mesh *m, *m2;

  m=0;
  for (o=allobjs; o!=0; o=o->next) if (o->mesh!=0) {
    m2=preprocess(o->mesh);
    m2->next=m; m=m2;
  }
  return m;
}

*/

Mesh *read3ds(char *filename, Material *m) {
  Chunkh h;
  FILE *f;
  long epos;

  f=fopen(filename, "rb");
  //allobjs=0;

  mater=m;
  if (!f) return 0;
  read3dsblock(f, &h);

  if (h.id!=0x4d4d) return 0;
  epos=h.npos;
  //dispsubblocks(f, 0, epos);
  indent++;
  do {
    read3dsblock(f, &h);
    switch (h.id) {
      case 0x3d3d: read3dsedit(f, h.npos); break;
      case 0xb000: read3dskeyf(f, h.npos); break;
    }
    fseek(f, h.npos, SEEK_SET);
  } while (h.npos<epos);
  indent--;
  fclose(f);
  return msh;
}





