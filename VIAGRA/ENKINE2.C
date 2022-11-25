#include "enkine2.h"
#include "tinymath.h"
#include "tinyheap.h"
#include "vectorz.h"


typedef unsigned short ushort;


int seed=3242342, seed2=8754567, seed3=1231231;
int rand() {
  seed=seed*(765342345+seed3--)+78761567+seed2++;
  return seed>>8&0x7fff;
}


extern ushort zbuf[];
#define rtbl (rtbl2+8192)
extern int rtbl2[];
extern char dbuf[];


void teematriisi(float m[3][3], float a, float b, float c) {
  float sa, ca, sb, cb, sc, cc;
  sa=fsin(a); ca=fcos(a);
  sb=fsin(b); cb=fcos(b);
  sc=fsin(c); cc=fcos(c);
  m[0][0]=-sa*sb*cc-ca*sc;
  m[0][1]=-sa*cb;
  m[0][2]= ca*cc-sa*sb*sc;
  m[1][0]= ca*sb*cc-sa*sc;
  m[1][1]= ca*cb;
  m[1][2]= sa*cc+ca*sb*sc;
  m[2][0]= cb*cc;
  m[2][1]=-sb;
  m[2][2]= cb*sc;
}



float piste(vector *a, vector *b) { return a->x*b->x+a->y*b->y+a->z*b->z; }
void risti(vector *d, vector *a, vector *b) {
  d->x=a->y*b->z-a->z*b->y;
  d->y=a->z*b->x-a->x*b->z;
  d->z=a->x*b->y-a->y*b->x;
}
void normalisoi(vector *d) {
  float r=1.0/fsqrt(piste(d, d));
  d->x*=r; d->y*=r; d->z*=r;
}


void teematriisi2(float m[3][3], float dx, float dy, float dz) {
  static vector dir, yvec, up, right;

  dir.x=dx; dir.y=dy; dir.z=dz; normalisoi(&dir);
  yvec.x=1; yvec.y=0.5; yvec.z=0.0; normalisoi(&yvec);
  risti(&right, &dir, &yvec); normalisoi(&right);
  risti(&up, &right, &dir); normalisoi(&up);

  m[0][0]=right.x;
  m[0][1]=right.y;
  m[0][2]=right.z;
  m[1][0]=up.x;
  m[1][1]=up.y;
  m[1][2]=up.z;
  m[2][0]=dir.x;
  m[2][1]=dir.y;
  m[2][2]=dir.z;
}


void tri(Vertex2d *v1, Vertex2d *v2, Vertex2d *v3, Material *m) {
  static Tri2d t;

  if (v1->y<v2->y) {
    if (v2->y<v3->y) t.top=v1, t.mid=v2, t.bot=v3;
      else if (v1->y<v3->y) t.top=v1, t.mid=v3, t.bot=v2;
      else t.top=v3, t.mid=v1, t.bot=v2;
  } else {
    if (v1->y<v3->y) t.top=v2, t.mid=v1, t.bot=v3;
      else if (v2->y<v3->y) t.top=v2, t.mid=v3, t.bot=v1;
      else t.top=v3, t.mid=v2, t.bot=v1;
  }

  t.m=m;
  zfilltri(&t);
}

static Vertex2d clip_v[256], *clip_vp;
static Tri2d clip_t[64], *clip_tp;

Vertex2d *leikkaaline(Vertex2d *v1, Vertex2d *v2, float kx, float ky, float k1) {
  float p, x, y, z;
  //(x1+(x2-x1)*p)*kx+(y1+(y2-y1)*p)*ky+k1=0
  //((y2-y1)*ky+(x2-x1)*kx)*p+x1*kx+y1*ky+k1=0
  p=-(v1->x*kx+v1->y*ky+k1)/((v2->y-v1->y)*ky+(v2->x-v1->x)*kx);
/*  clip_vp->x=v1->x+(v2->x-v1->x)*p;
  clip_vp->y=v1->y+(v2->y-v1->y)*p;
  clip_vp->z=v1->z+(v2->z-v1->z)*p;*/
  x=v1->x+(v2->x-v1->x)*p;
  y=v1->y+(v2->y-v1->y)*p;
  z=v1->z+(v2->z-v1->z)*p;
  if (x<-100000000.0) clip_vp->x=-100000000; else if (x>100000000.0) clip_vp->x=100000000; else clip_vp->x=x;
  if (y<-100000000.0) clip_vp->y=-100000000; else if (y>100000000.0) clip_vp->y=100000000; else clip_vp->y=y;
  if (z<-100000000.0) clip_vp->z=-100000000; else if (z>100000000.0) clip_vp->z=100000000; else clip_vp->z=z;
  clip_vp->u=v1->u+(v2->u-v1->u)*p;
  clip_vp->v=v1->v+(v2->v-v1->v)*p;
  clip_vp->clip=0;
  return clip_vp++;
}
int chk(Vertex2d *v, float kx, float ky, float k1) {
  return v->x*kx+v->y*ky+k1<0.0^(v->clip>>4&1);
}
void leikkaa1(Tri2d *t, float kx, float ky, float k1) {
  Vertex2d *a, *b;
  if (chk(t->top, kx, ky, k1)) {
    if (chk(t->mid, kx, ky, k1)) {
      if (chk(t->bot, kx, ky, k1)) {
        t->top=t->mid=t->bot=0;
      } else {
        a=leikkaaline(t->top, t->bot, kx, ky, k1);
        b=leikkaaline(t->mid, t->bot, kx, ky, k1);
        t->top=a; t->mid=b;
      }
    } else {
      if (chk(t->bot, kx, ky, k1)) {
        a=leikkaaline(t->top, t->mid, kx, ky, k1);
        b=leikkaaline(t->bot, t->mid, kx, ky, k1);
        t->top=a; t->bot=b;
      } else {
        a=leikkaaline(t->mid, t->top, kx, ky, k1);
        b=leikkaaline(t->bot, t->top, kx, ky, k1);
        clip_tp->top=a; clip_tp->mid=b; clip_tp->bot=t->bot; clip_tp++;
        t->top=a;
      }
    }
  } else {
    if (chk(t->mid, kx, ky, k1)) {
      if (chk(t->bot, kx, ky, k1)) {
        a=leikkaaline(t->mid, t->top, kx, ky, k1);
        b=leikkaaline(t->bot, t->top, kx, ky, k1);
        t->mid=a; t->bot=b;
      } else {
        a=leikkaaline(t->top, t->mid, kx, ky, k1);
        b=leikkaaline(t->bot, t->mid, kx, ky, k1);
        clip_tp->top=a; clip_tp->mid=b; clip_tp->bot=t->bot; clip_tp++;
        t->mid=a;
      }
    } else {
      if (chk(t->bot, kx, ky, k1)) {
        a=leikkaaline(t->top, t->bot, kx, ky, k1);
        b=leikkaaline(t->mid, t->bot, kx, ky, k1);
        clip_tp->top=a; clip_tp->mid=b; clip_tp->bot=t->mid; clip_tp++;
        t->bot=a;
      } else {
      }
    }
  }
}
void leikkaa(Vertex2d *v1, Vertex2d *v2, Vertex2d *v3, Material *m) {
  Tri2d *t, *t2; Vertex2d *v;
  clip_vp=clip_v; clip_tp=clip_t;
  clip_tp->top=v1; clip_tp->mid=v2; clip_tp->bot=v3; clip_tp++;
  for (t=clip_t, t2=clip_tp; t<t2; t++) leikkaa1(t, 1, 0, -2048);
  for (t=clip_t, t2=clip_tp; t<t2; t++) leikkaa1(t, 0, 1, -2048);
  for (t=clip_t, t2=clip_tp; t<t2; t++) leikkaa1(t, -1, 0, 320*4096-2048);
  for (t=clip_t, t2=clip_tp; t<t2; t++) leikkaa1(t, 0, -1, 200*4096-2048);
  for (v=clip_v; v<clip_vp; v++)
    v->clip=(mask_l(v->x, 0)&1)|(mask_l(v->y, 0)&2)|(mask_g(v->x, 4096*320)&4)|(mask_g(v->y, 4096*200)&8)^(-(v->clip>>4&1)&31);
  for (t=clip_t, t2=clip_tp; t<t2; t++) if (t->top)
    if (!(t->top->clip|t->mid->clip|t->bot->clip))
      tri(t->top, t->mid, t->bot, m);
}


void taeyttaejae(char *dest, object *opu) {
  int x, y, i, u, v;
  int v1, v2, v3;
  int txinc, tyinc, tx, ty;
  int zinc, z;
  vertex *w1, *w2, *w3;
  int ymin, ymax;
  int r, g, b, joo;
  face *f1;

  triprep(dest, zbuf);
  for (i=0; i<opu->fcount; i++) {
    f1=opu->flist+i;
    v1=f1->v1;
    v2=f1->v2;
    v3=f1->v3;
    w1=opu->vlist+v1;
    w2=opu->vlist+v2;
    w3=opu->vlist+v3;

    if ((w2->p.x-w1->p.x)*(float)(w3->p.y-w1->p.y)
       <(w2->p.y-w1->p.y)*(float)(w3->p.x-w1->p.x)
       ^((w1->p.clip^w2->p.clip^w3->p.clip)>>4&1)) continue;
    if (w1->p.clip&w2->p.clip&w3->p.clip) continue;
    w3->p.u=w1->p.u-8388608+(w3->p.u-w1->p.u+8388608&16777215);
    w3->p.v=w1->p.v-8388608+(w3->p.v-w1->p.v+8388608&16777215);
    w2->p.u=w1->p.u-8388608+(w2->p.u-w1->p.u+8388608&16777215);
    w2->p.v=w1->p.v-8388608+(w2->p.v-w1->p.v+8388608&16777215);

    if (w1->p.clip|w2->p.clip|w3->p.clip)
      leikkaa(&w1->p, &w2->p, &w3->p, f1->m);
      else tri(&w1->p, &w2->p, &w3->p, f1->m);
  }
}



void rotateobject(object *opu, float xrot, float yrot, float zrot,
                               float xpos, float ypos, float zpos) {
  float m[3][3];
  int x;

  teematriisi(m, xrot, yrot, zrot);
  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].rx=m[0][0]*(opu->vlist[x].x)+
                     m[0][1]*(opu->vlist[x].y)+
                     m[0][2]*(opu->vlist[x].z)+xpos;
    opu->vlist[x].ry=m[1][0]*(opu->vlist[x].x)+
                     m[1][1]*(opu->vlist[x].y)+
                     m[1][2]*(opu->vlist[x].z)+ypos;
    opu->vlist[x].rz=m[2][0]*(opu->vlist[x].x)+
                     m[2][1]*(opu->vlist[x].y)+
                     m[2][2]*(opu->vlist[x].z)+zpos;
    opu->vlist[x].rnx=m[0][0]*(opu->vlist[x].nx)+
                      m[0][1]*(opu->vlist[x].ny)+
                      m[0][2]*(opu->vlist[x].nz);
    opu->vlist[x].rny=m[1][0]*(opu->vlist[x].nx)+
                      m[1][1]*(opu->vlist[x].ny)+
                      m[1][2]*(opu->vlist[x].nz);
    opu->vlist[x].rnz=m[2][0]*(opu->vlist[x].nx)+
                      m[2][1]*(opu->vlist[x].ny)+
                      m[2][2]*(opu->vlist[x].nz);
  }
  //for (x=0; x<opu->vcount; x++) {
    //opu->vlist[x].p.u=(opu->vlist[x].rnz*128.0+128.0)*65536.0;
    //opu->vlist[x].p.v=(opu->vlist[x].rny*128.0+128.0)*65536.0;
  //}
}



void latex2(object *opu, float xpos, float ypos, float zpos,
                         float xtgt, float ytgt, float ztgt, float per) {
  int i;
  float camera[3][3];
  float x, y, z;
  vertex *v;

  teematriisi2(camera, xtgt-xpos, ytgt-ypos, ztgt-zpos);
  for (i=0; i<opu->vcount; i++) {
    opu->vlist[i].crx=camera[0][0]*(opu->vlist[i].rx-xpos)+
                      camera[0][1]*(opu->vlist[i].ry-ypos)+
                      camera[0][2]*(opu->vlist[i].rz-zpos);
    opu->vlist[i].cry=camera[1][0]*(opu->vlist[i].rx-xpos)+
                      camera[1][1]*(opu->vlist[i].ry-ypos)+
                      camera[1][2]*(opu->vlist[i].rz-zpos);
    opu->vlist[i].crz=camera[2][0]*(opu->vlist[i].rx-xpos)+
                      camera[2][1]*(opu->vlist[i].ry-ypos)+
                      camera[2][2]*(opu->vlist[i].rz-zpos);
  }

  for (i=0; i<opu->vcount; i++) {
    v=opu->vlist+i;
    z=1.0/v->crz;
    x=(-(v->crx*per*z)*cam_z+160.0)*4096.0;
    y=(-(v->cry*per*z)*cam_z+100.0)*4096.0;
    z=z*4.0*256.0*256.0*256.0;
    if (x<-100000000.0) v->p.x=-100000000; else if (x>100000000.0) v->p.x=100000000; else v->p.x=x;
    if (y<-100000000.0) v->p.y=-100000000; else if (y>100000000.0) v->p.y=100000000; else v->p.y=y;
    if (z<-100000000.0) v->p.z=-100000000; else if (z>100000000.0) v->p.z=100000000; else v->p.z=z;
    v->p.clip=(mask_l(v->p.x, 0)&1)|(mask_l(v->p.y, 0)&2)|(mask_g(v->p.x, 4096*320)&4)|(mask_g(v->p.y, 4096*200)&8)^(-(*(int*)&z>>31&1)&31);
    v->color=x;
  }
}


void laskevektorinormaalit(object *opu) {
  int x;
  int v1, v2, v3;
  float nx, ny, nz;
  vertex *w1, *w2, *w3;
  face *f, *fm;
  float vp;
  float x21, y21, z21, x31, y31, z31;

  for (w1=opu->vlist, w2=w1+opu->vcount; w1<w2; w1++)
    w1->nx=w1->ny=w1->nz=0;
  for (f=opu->flist, fm=f+opu->fcount; f<fm; f++) {
     w1=opu->vlist+f->v1;
     w2=opu->vlist+f->v2;
     w3=opu->vlist+f->v3;

     x21=w2->x-w1->x; y21=w2->y-w1->y; z21=w2->z-w1->z;
     x31=w3->x-w1->x; y31=w3->y-w1->y; z31=w3->z-w1->z;
     nx=y31*z21-z31*y21;
     ny=z31*x21-x31*z21;
     nz=x31*y21-y31*x21;
     //vp=1/fsqrt(nx*nx+ny*ny+nz*nz);
     //nx*=vp; ny*=vp; nz*=vp;
     w1->nx+=nx; w1->ny+=ny; w1->nz+=nz;
     w2->nx+=nx; w2->ny+=ny; w2->nz+=nz;
     w3->nx+=nx; w3->ny+=ny; w3->nz+=nz;
  }

  for (w1=opu->vlist, w2=w1+opu->vcount; w1<w2; w1++) {
    vp=1/fsqrt(w1->nx*w1->nx+w1->ny*w1->ny+w1->nz*w1->nz);
    w1->nx*=vp; w1->ny*=vp; w1->nz*=vp;
  }

}


void teedotit(char *dest, object *opu) {
  int x, u, v;
  vertex *w1, *w2, *w3;
  face *f1;

  for (x=0; x<opu->vcount; x++) {
    w1=opu->vlist+x;
    u=(w1->p.x>>12);
    v=(w1->p.y>>12);
    if (u>0 && u<320 && v>0 && v<200) dest[v*320+u]=rtbl[dest[v*320+u]+32];
  }

  for (x=0; x<opu->fcount; x++) {
    f1=opu->flist+x;
    w1=opu->vlist+f1->v1;
    w2=opu->vlist+f1->v2;
    w3=opu->vlist+f1->v3;

    u=((w1->p.x>>12)+(w2->p.x>>12)+(w3->p.x>>12))/3;
    v=((w1->p.y>>12)+(w2->p.y>>12)+(w3->p.y>>12))/3;
    if (u>0 && u<320 && v>0 && v<200) dest[v*320+u]=rtbl[dest[v*320+u]+32];
  }

}

/*
void teemunkkirinkila(object *opu) {
  int x, y;
  //int v1, v2, v3;
  //float nx, ny, nz;
  vertex *w1;//, *w2, *w3;
  face *f;
  //float vp;

  opu->vcount=128;
  opu->fcount=256;
  opu->vlist=halloc(opu->vcount*sizeof(opu->vlist[0]));
  opu->flist=halloc(opu->fcount*sizeof(opu->flist[0]));

  for (x=0; x<16; x++) for (y=0; y<8; y++) {
    w1=opu->vlist+x*8+y;
    if (x*8+y<opu->vcount) {
      w1->x=(1.1+fsin((y+x/2.0)*pi*2/8.0)*0.4)*fsin(x*pi*2/16.0);
      w1->y=fcos((y+x/2.0)*pi*2/8.0)*0.4;
      w1->z=(1.1+fsin((y+x/2.0)*pi*2/8.0)*0.4)*fcos(x*pi*2/16.0);
    }
  }
  for (x=0; x<16; x++) for (y=0; y<8; y++) {
    f=opu->flist+x*8+y;
    if (x*8+y<opu->fcount) {
      f->v3=(x&15)*8+(y&7);
      f->v2=(x+1&15)*8+(y&7);
      f->v1=(x&15)*8+(y+1&7);
    }
    f=opu->flist+x*8+y+128;
    if (x*8+y+128<opu->fcount) {
      f->v3=(x+1&15)*8+(y+1&7);
      f->v2=(x&15)*8+(y+1&7);
      f->v1=(x+1&15)*8+(y&7);
    }
  }

}
*/

void create_pallo(object *opu, Material *m) {
  int x, y;
  //int v1, v2, v3;
  //float nx, ny, nz;
  vertex *w1;//, *w2, *w3;
  face *f;
  float a1, a2, r;
  //float vp;

  opu->vcount=33*32;
  opu->fcount=32*64;
  opu->vlist=halloc(opu->vcount*sizeof(opu->vlist[0]));
  opu->flist=halloc(opu->fcount*sizeof(opu->flist[0]));

  for (y=0; y<33; y++) for (x=0; x<32; x++) {
    //r=rand()/32768.0+1;
    w1=opu->vlist+y*32+x;
    //a1=y*pi/8;
    //a2=(x+1)*pi/17;
    //w1->x=fsin(a1)*(fsin(a2)*r);
    //w1->y=fcos(a2)*r;
    //w1->z=fcos(a1)*(fsin(a2)*r);
    w1->p.u=x*1024*512;
    w1->p.v=y*1024*512;
  }

  //for (x=0; x<16; x++) {
    //opu->vlist[x].x=0; opu->vlist[x].y=1; opu->vlist[x].z=0;
  //}

  //opu->vlist[256].x=0; opu->vlist[257].y=1; opu->vlist[256].z=0;
  //opu->vlist[257].x=0; opu->vlist[257].y=-1; opu->vlist[257].z=0;

  for (y=0; y<32; y++) {
    for (x=0; x<32; x++) {
      f=opu->flist+y*64+x*2;
      f->v1=y*32+x;
      f->v2=y*32+x+32;
      f->v3=y*32+(x+1&31)+32;
      f->m=m;
      f=opu->flist+y*64+x*2+1;
      f->v1=y*32+x;
      f->v2=y*32+(x+1&31)+32;
      f->v3=y*32+(x+1&31);
      f->m=m;
    }
  }
/*  for (x=0; x<16; x++) {
    f=opu->flist+15*32+x;
    f->v1=256;
    f->v3=x+1&15;
    f->v2=x;
  }
  for (x=0; x<16; x++) {
    f=opu->flist+15*32+16+x;
    f->v1=257;
    f->v2=15*16+(x+1&15);
    f->v3=15*16+x;
  }*/


}

void teepallo(object *opu, float t, float size) {
  int x, y;
  vertex *w1;
  float a1, a2;
  float d=3.0, s=0.12;

  for (y=0; y<33; y++) for (x=0; x<32; x++) {
    w1=opu->vlist+y*32+x;
    a2=y*pi/32.0;
    a1=x*pi/16.0;

    w1->x=fsin(a1)*(fsin(a2));
    w1->y=-fcos(a2);
    w1->z=fcos(a1)*fsin(a2);

    s=1+fsin(w1->x*d+t+fsin(w1->y*d-t*0.652131+fcos(w1->z*d+t*0.863326)))*0.7;
    s*=size;
    w1->x*=s; w1->y*=s; w1->z*=s;
  }

}

void create_pallo2(object *opu, Material *m) {
  int x, y;
  //int v1, v2, v3;
  //float nx, ny, nz;
  vertex *w1;//, *w2, *w3;
  face *f;
  float a1, a2, r;
  //float vp;

  opu->vcount=33*32;
  opu->fcount=32*64;
  opu->vlist=halloc(opu->vcount*sizeof(opu->vlist[0]));
  opu->flist=halloc(opu->fcount*sizeof(opu->flist[0]));

  for (y=0; y<33; y++) for (x=0; x<32; x++) {
    //r=rand()/32768.0+1;
    w1=opu->vlist+y*32+x;
    //a1=y*pi/8;
    //a2=(x+1)*pi/17;
    //w1->x=fsin(a1)*(fsin(a2)*r);
    //w1->y=fcos(a2)*r;
    //w1->z=fcos(a1)*(fsin(a2)*r);
    w1->p.u=x*1024*512;
    w1->p.v=y*1024*512;
  }

  //for (x=0; x<16; x++) {
    //opu->vlist[x].x=0; opu->vlist[x].y=1; opu->vlist[x].z=0;
  //}

  //opu->vlist[256].x=0; opu->vlist[257].y=1; opu->vlist[256].z=0;
  //opu->vlist[257].x=0; opu->vlist[257].y=-1; opu->vlist[257].z=0;

  for (y=0; y<32; y++) {
    for (x=0; x<32; x++) {
      f=opu->flist+y*64+x*2;
      f->v1=y*32+x;
      f->v2=y*32+x+32;
      f->v3=y*32+(x+1&31)+32;
      f->m=m;
      f=opu->flist+y*64+x*2+1;
      f->v1=y*32+x;
      f->v2=y*32+(x+1&31)+32;
      f->v3=y*32+(x+1&31);
      f->m=m;
    }
  }
/*  for (x=0; x<16; x++) {
    f=opu->flist+15*32+x;
    f->v1=256;
    f->v3=x+1&15;
    f->v2=x;
  }
  for (x=0; x<16; x++) {
    f=opu->flist+15*32+16+x;
    f->v1=257;
    f->v2=15*16+(x+1&15);
    f->v3=15*16+x;
  }*/


}

void teepallo2(object *opu, float t) {
  int x, y;
  //float joo1, joo2, joo3;
  vertex *w1;
  float a1, a2;
  float d=3.0, s=0.12;

  for (y=0; y<33; y++) for (x=0; x<32; x++) {
    w1=opu->vlist+y*32+x;
    a2=y*pi/32.0;
    a1=x*pi/16.0;

    w1->x=fsin(a1)*(fsin(a2));
    w1->y=-fcos(a2);
    w1->z=fcos(a1)*fsin(a2);

    s=1+fsin(w1->x*d+t+fsin(w1->y*d-t+fcos(w1->z*d+t)))*0.1;
    w1->x*=s; w1->y*=s; w1->z*=s;
  }

  //opu->vlist[256].x=0+fsin(t)*0.1; opu->vlist[256].y=1+fcos(1*d+t)*0.1; opu->vlist[256].z=0;
  //opu->vlist[257].x=0+fsin(t)*0.1; opu->vlist[257].y=-1+fcos(-1*d+t)*0.1; opu->vlist[257].z=0;
  //laskevektorinormaalit(opu);

}

void teepiikkipallo(object *opu, float t) {
  int x, y;
  //float joo1, joo2, joo3;
  vertex *w1;
  float a1, a2;
  float d=3.0, s=0.12, r, e;

  for (y=0; y<33; y++) for (x=0; x<32; x++) {
    w1=opu->vlist+y*32+x;
    a2=y*pi/32.0;
    a1=x*pi/16.0;


    e=(rand()-rand())/16384.0;
    e*=e;
    r=1+e*e*0.05;

    w1->x=fsin(a1)*(fsin(a2))*r;
    w1->y=-fcos(a2)*r;
    w1->z=fcos(a1)*fsin(a2)*r;

    //s=1+fsin(w1->x*d+t+fsin(w1->y*d-t*0.652131+fcos(w1->z*d+t*0.863326)))*0.7;
    //w1->x*=s; w1->y*=s; w1->z*=s;
  }

  //opu->vlist[256].x=0+fsin(t)*0.1; opu->vlist[256].y=1+fcos(1*d+t)*0.1; opu->vlist[256].z=0;
  //opu->vlist[257].x=0+fsin(t)*0.1; opu->vlist[257].y=-1+fcos(-1*d+t)*0.1; opu->vlist[257].z=0;
  //laskevektorinormaalit(opu);

}



