#include "enkine.h"
#include "tinymath.h"
//#include <stdlib.h>

typedef unsigned short ushort;




extern char dbuf[64000];
extern ushort zbuf[64000];
extern ushort envi[65536];
extern char mappi[65536], mappi2[65536];
extern int pienix[41*26], pieniz[41*26], varjot[41*26], varjot2[41*26];
extern char isox[64000], isoz[64000], valo[64000], valo2[64000];
extern char tmap[];
extern ushort paletti1[16384];
extern char plasma[64000];
extern short hfield1[65536], hfield2[65536], sint1[256], sint2[256], sint3[256];
extern ushort valospr162[256*256];
extern char valospr16[256*256];
extern char sprite[256*256];
extern ushort spritepal[64*256];


int raja0319(int k) { return k<319?(k>0?k:0):319; }
void fill32(void *dezt, int kount, int num);
#pragma aux fill32= "cld" "rep stosd" parm[edi][ecx][eax];

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
  yvec.x=0.0; yvec.y=0.1; yvec.z=0.0; normalisoi(&yvec);
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



void viiva(object *opu, int v1, int v2) {
  int xinc, xpos, zinc, z;
  int y, swap;
  int x1, y1, x2, y2, z1, z2;
  int txinc, tyinc, tx, ty;
  int tx1, ty1, tx2, ty2;

  x1=opu->vlist[v1].xp;
  y1=opu->vlist[v1].yp;
  x2=opu->vlist[v2].xp;
  y2=opu->vlist[v2].yp;
  z1=opu->vlist[v1].zp;
  z2=opu->vlist[v2].zp;

  tx1=opu->vlist[v1].tx;
  ty1=opu->vlist[v1].ty;
  tx2=opu->vlist[v2].tx;
  ty2=opu->vlist[v2].ty;

  if (y2<y1) {
    swap=y2; y2=y1; y1=swap;
    swap=x2; x2=x1; x1=swap;
    swap=z2; z2=z1; z1=swap;
    swap=tx2; tx2=tx1; tx1=swap;
    swap=ty2; ty2=ty1; ty1=swap;
  }

  zinc=(z2-z1)/(y2-y1+0.0000001)*65536;
  z=z1*65536;
  xinc=(x2-x1)/(y2-y1+0.0000001)*65536;
  xpos=x1*65536;

  txinc=(tx2-tx1)/(y2-y1+0.0000001)*65536;
  tx=tx1*65536;
  tyinc=(ty2-ty1)/(y2-y1+0.0000001)*65536;
  ty=ty1*65536;

  if (y1<0) {
    xpos-=xinc*y1;
    z-=zinc*y1;
    tx-=txinc*y1;
    ty-=tyinc*y1;
    y1=0;
  }
  if (y2>200) y2=200;

  for (y=y1; y<y2; y++) {
    if (opu->xbgn[y].koord==luku) {
      opu->xbgn[y].koord=xpos;
      opu->xbgn[y].zp=z;
      opu->xbgn[y].tx=tx;
      opu->xbgn[y].ty=ty;
    }
    else if (xpos<opu->xbgn[y].koord) {
      opu->xend[y].koord=opu->xbgn[y].koord;
      opu->xend[y].zp=opu->xbgn[y].zp;
      opu->xend[y].tx=opu->xbgn[y].tx;
      opu->xend[y].ty=opu->xbgn[y].ty;
      opu->xbgn[y].koord=xpos;
      opu->xbgn[y].zp=z;
      opu->xbgn[y].tx=tx;
      opu->xbgn[y].ty=ty;
    } else {
      opu->xend[y].koord=xpos;
      opu->xend[y].zp=z;
      opu->xend[y].tx=tx;
      opu->xend[y].ty=ty;
    }
    xpos+=xinc;
    z+=zinc;
    tx+=txinc;
    ty+=tyinc;
  }
}

void klenzviiva(object *opu, int v1, int v2) {
  int xinc, xpos;
  int y, swap;
  int x1, y1, x2, y2;

  x1=opu->vlist[v1].xp;
  y1=opu->vlist[v1].yp;
  x2=opu->vlist[v2].xp;
  y2=opu->vlist[v2].yp;

  if (y2<y1) {
    swap=y2; y2=y1; y1=swap;
    swap=x2; x2=x1; x1=swap;
  }

  xinc=(x2-x1)/(y2-y1+0.0000001)*65536;
  xpos=x1*65536;

  if (y1<0) {
    xpos-=xinc*y1;
    y1=0;
  }
  if (y2>200) y2=200;

  for (y=y1; y<y2; y++) {
    if (opu->xbgn[y].koord==luku) {
      opu->xbgn[y].koord=xpos;
    }
    else if (xpos<opu->xbgn[y].koord) {
      opu->xend[y].koord=opu->xbgn[y].koord;
      opu->xbgn[y].koord=xpos;
    } else {
      opu->xend[y].koord=xpos;
    }
    xpos+=xinc;
  }
}



void taeyttaejae(ushort *dest, object *opu) {
  int x, y, i, u, v;
  int v1, v2, v3;
  int intz;
  int txinc, tyinc, tx, ty;
  int zinc, z;
  vertex *w1, *w2, *w3;
  int ymin, ymax;

  for (i=0; i<opu->fcount; i++) {
    v1=opu->flist[i].v1;
    v2=opu->flist[i].v2;
    v3=opu->flist[i].v3;
    w1=opu->vlist+v1;
    w2=opu->vlist+v2;
    w3=opu->vlist+v3;

    if ((w2->xp-w1->xp)*(w3->yp-w1->yp)<(w2->yp-w1->yp)*(w3->xp-w1->xp)) continue;
    if (w1->zp<=0) continue;
    if (w2->zp<=0) continue;
    if (w3->zp<=0) continue;

    ymin=w1->yp; if (w2->yp<ymin) ymin=w2->yp; if (w3->yp<ymin) ymin=w3->yp;
    if (ymin<0) ymin=0;
    ymax=w1->yp; if (w2->yp>ymax) ymax=w2->yp; if (w3->yp>ymax) ymax=w3->yp;
    if (ymax>200) ymax=200;

    for (y=ymin; y<ymax; y++) { opu->xbgn[y].koord=luku; opu->xend[y].koord=luku; }
    viiva(opu, v1, v2);
    viiva(opu, v2, v3);
    viiva(opu, v3, v1);

    for (y=ymin; y<ymax; y++) if (opu->xbgn[y].koord!=luku) {
        zinc=(opu->xend[y].zp-opu->xbgn[y].zp)/(opu->xend[y].koord-opu->xbgn[y].koord+0.0000001)*65536;
        z=opu->xbgn[y].zp;
        txinc=(opu->xend[y].tx-opu->xbgn[y].tx)/(opu->xend[y].koord-opu->xbgn[y].koord+0.0000001)*65536;
        tyinc=(opu->xend[y].ty-opu->xbgn[y].ty)/(opu->xend[y].koord-opu->xbgn[y].koord+0.0000001)*65536;
        tx=opu->xbgn[y].tx;
        ty=opu->xbgn[y].ty;
        if (opu->xbgn[y].koord<0) {
          z-=zinc*(opu->xbgn[y].koord>>16);
          tx-=txinc*(opu->xbgn[y].koord>>16);
          ty-=tyinc*(opu->xbgn[y].koord>>16);
          opu->xbgn[y].koord=0;
        }
        if (opu->xend[y].koord>320*65536) opu->xend[y].koord=320*65536;
        for (x=opu->xbgn[y].koord>>16; x<opu->xend[y].koord>>16; x++) {
//          intz=z>>8;
          if (z>>16>=zbuf[y*320+x]) {
            zbuf[y*320+x]=z>>16;
            u=tx>>16;
            v=ty>>16;
            dest[y*320+x]=envi[(v<<8)+u&65535];
          }
          z+=zinc;
          tx+=txinc;
          ty+=tyinc;
        }
    }
  }
}

void klenzviivat(char *dest, object *opu) {
  int x, i;
  int v1, v2, v3;
  vertex *w1, *w2, *w3;
  int ymin, ymax;

//  for (i=0; i<64000; i++) dbuf[i]=128;
//  fill32(dbuf, 16000, 0x80808080);

  for (i=0; i<opu->fcount; i++) {
    v1=opu->flist[i].v1;
    v2=opu->flist[i].v2;
    v3=opu->flist[i].v3;
    w1=opu->vlist+v1;
    w2=opu->vlist+v2;
    w3=opu->vlist+v3;

//    if ((w2->xp-w1->xp)*(w3->yp-w1->yp)<(w2->yp-w1->yp)*(w3->xp-w1->xp)) continue;
    if (w1->zp<=0) continue;
    if (w2->zp<=0) continue;
    if (w3->zp<=0) continue;

    ymin=w1->yp; if (w2->yp<ymin) ymin=w2->yp; if (w3->yp<ymin) ymin=w3->yp;
    if (ymin<0) ymin=0;
    ymax=w1->yp; if (w2->yp>ymax) ymax=w2->yp; if (w3->yp>ymax) ymax=w3->yp;
    if (ymax>200) ymax=200;

    for (x=ymin; x<ymax; x++) { opu->xbgn[x].koord=luku; opu->xend[x].koord=luku; }
    klenzviiva(opu, v1, v2);
    klenzviiva(opu, v2, v3);
    klenzviiva(opu, v3, v1);
    for (x=ymin; x<ymax; x++) {
      dest[x*320+raja0319(opu->xbgn[x].koord>>16)]+=1;
      dest[x*320+raja0319(opu->xend[x].koord>>16)]-=1;
    }
  }
}


/*void klenztaeyttaejae(ushort *dest, char *sourke, object *opu) {
  int joo, i;

  joo=0;
  for (i=0; i<64000; i++) {
    joo+=sourke[i]-128;
    sourke[i]=128;
    dest[i]+=joo<<11;
  }
}*/


void klenztaeyttaejae(ushort *dest, char *sourke, object *opu) {
  int joo, i, j;

  joo=0;
  for (i=0; i<64000; i+=4) {
    joo+=sourke[i  ]-128; j=joo;
    joo+=sourke[i+1]-128; ((int*)(dest+i))[0]+=(j<<11)+(joo<<27);
    joo+=sourke[i+2]-128; j=joo;
    joo+=sourke[i+3]-128; ((int*)(dest+i))[1]+=(j<<11)+(joo<<27);
    *(int*)(sourke+i)=0x80808080;
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
  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].tx=opu->vlist[x].rnx*128+128;
    opu->vlist[x].ty=opu->vlist[x].rny*128+128;
  }
}

void latex(object *opu, float xpos, float ypos, float zpos,
                        float xrot, float yrot, float zrot) {
  int x;
  float camera[3][3];
//  static object rotated;

  teematriisi(camera, xrot, yrot, zrot);
  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].crx=camera[0][0]*(opu->vlist[x].rx-xpos)+
                      camera[0][1]*(opu->vlist[x].ry-ypos)+
                      camera[0][2]*(opu->vlist[x].rz-zpos);
    opu->vlist[x].cry=camera[1][0]*(opu->vlist[x].rx-xpos)+
                      camera[1][1]*(opu->vlist[x].ry-ypos)+
                      camera[1][2]*(opu->vlist[x].rz-zpos);
    opu->vlist[x].crz=camera[2][0]*(opu->vlist[x].rx-xpos)+
                      camera[2][1]*(opu->vlist[x].ry-ypos)+
                      camera[2][2]*(opu->vlist[x].rz-zpos);
  }

  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].xp=(opu->vlist[x].crx/opu->vlist[x].crz)*cam_z+160;
    opu->vlist[x].yp=(opu->vlist[x].cry/opu->vlist[x].crz)*cam_z+100;
    opu->vlist[x].zp=-1/opu->vlist[x].crz*128*256;
    opu->vlist[x].color=x;
  }
}




void latex2(object *opu, float xpos, float ypos, float zpos,
                         float xtgt, float ytgt, float ztgt) {
  int x;
  float camera[3][3];

  teematriisi2(camera, xtgt-xpos, ytgt-ypos, ztgt-zpos);
  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].crx=camera[0][0]*(opu->vlist[x].rx-xpos)+
                      camera[0][1]*(opu->vlist[x].ry-ypos)+
                      camera[0][2]*(opu->vlist[x].rz-zpos);
    opu->vlist[x].cry=camera[1][0]*(opu->vlist[x].rx-xpos)+
                      camera[1][1]*(opu->vlist[x].ry-ypos)+
                      camera[1][2]*(opu->vlist[x].rz-zpos);
    opu->vlist[x].crz=camera[2][0]*(opu->vlist[x].rx-xpos)+
                      camera[2][1]*(opu->vlist[x].ry-ypos)+
                      camera[2][2]*(opu->vlist[x].rz-zpos);
  }

  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].xp=-(opu->vlist[x].crx/opu->vlist[x].crz)*cam_z+160;
    opu->vlist[x].yp=-(opu->vlist[x].cry/opu->vlist[x].crz)*cam_z+100;
    opu->vlist[x].zp=1/opu->vlist[x].crz*128*256;
    opu->vlist[x].color=x;
  }
}


void laskevektorinormaalit(object *opu) {
  int x;
  int v1, v2, v3;
  float nx, ny, nz;
  vertex *w1, *w2, *w3;
  face *f;
  float vp;

  for (x=0; x<opu->vcount; x++) {
    opu->vlist[x].nx=0;
    opu->vlist[x].ny=0;
    opu->vlist[x].nz=0;
  }
  for (x=0; x<opu->fcount; x++) {
     v1=opu->flist[x].v1;
     v2=opu->flist[x].v2;
     v3=opu->flist[x].v3;
     w1=opu->vlist+v1;
     w2=opu->vlist+v2;
     w3=opu->vlist+v3;

     nx=(w3->y-w1->y)*(w2->z-w1->z)-(w3->z-w1->z)*(w2->y-w1->y);
     ny=(w3->z-w1->z)*(w2->x-w1->x)-(w3->x-w1->x)*(w2->z-w1->z);
     nz=(w3->x-w1->x)*(w2->y-w1->y)-(w3->y-w1->y)*(w2->x-w1->x);

     w1->nx+=nx;
     w1->ny+=ny;
     w1->nz+=nz;
     w2->nx+=nx;
     w2->ny+=ny;
     w2->nz+=nz;
     w3->nx+=nx;
     w3->ny+=ny;
     w3->nz+=nz;
  }

  for (x=0; x<opu->vcount; x++) {
    w1=opu->vlist+x;
    vp=1/fsqrt(w1->nx*w1->nx+w1->ny*w1->ny+w1->nz*w1->nz);
    w1->nx*=vp;
    w1->ny*=vp;
    w1->nz*=vp;
  }

}



void teemunkkirinkila(object *opu) {
  int x, y;
  int v1, v2, v3;
  float nx, ny, nz;
  vertex *w1, *w2, *w3;
  face *f;
  float vp;

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
      f->v1=(x&15)*8+(y&7);
      f->v2=(x+1&15)*8+(y&7);
      f->v3=(x&15)*8+(y+1&7);
    }
    f=opu->flist+x*8+y+128;
    if (x*8+y+128<opu->fcount) {
      f->v1=(x+1&15)*8+(y+1&7);
      f->v2=(x&15)*8+(y+1&7);
      f->v3=(x+1&15)*8+(y&7);
    }
  }

}




