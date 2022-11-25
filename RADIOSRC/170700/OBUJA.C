#include <stdlib.h>
#include "core.h"
#include "obuja.h"
#include "mesh.h"



Mesh *new_mesh_cube(Material *m, float xs, float ys, float zs) {
  Mesh *o=new_mesh_alloc(6, 8);
  mesh_v_set(o, 0, -xs, -ys, -zs, 0, 0);
  mesh_v_set(o, 1, -xs, -ys,  zs, 0, 1);
  mesh_v_set(o, 2, -xs,  ys, -zs, 0, 1);
  mesh_v_set(o, 3, -xs,  ys,  zs, 0, 2);
  mesh_v_set(o, 4,  xs, -ys, -zs, 1, 0);
  mesh_v_set(o, 5,  xs, -ys,  zs, 1, 1);
  mesh_v_set(o, 6,  xs,  ys, -zs, 1, 1);
  mesh_v_set(o, 7,  xs,  ys,  zs, 1, 2);
  mesh_p_set4(o, 0, m, 0, 1, 3, 2);
  mesh_p_set4(o, 1, m, 6, 7, 5, 4);
  mesh_p_set4(o, 2, m, 4, 5, 1, 0);
  mesh_p_set4(o, 3, m, 2, 3, 7, 6);
  mesh_p_set4(o, 4, m, 0, 2, 6, 4);
  mesh_p_set4(o, 5, m, 5, 7, 3, 1);
  return mesh_recalc(o);
}


static void teegpoly(Mesh *o, Material *m, int *vi, int *fi, float x0, float y0, float z0, float ux, float uy, float uz, float vx, float vy, float vz, int uc, int vc) {
  int u, v, ovi=*vi;
  for (v=0; v<vc+1; v++) for (u=0; u<uc+1; u++)
    mesh_v_set(o, (*vi)++, x0+u*ux/uc+v*vx/vc, y0+u*uy/uc+v*vy/vc, x0+u*uz/uc+v*vz/vc, u/(float)uc, v/(float)vc);
  for (v=0; v<vc; v++) for (u=0; u<uc; u++)
    mesh_p_set4(o, (*fi)++, m, ovi+v*(uc+1)+u, ovi+v*(uc+1)+u+1, ovi+(v+1)*(uc+1)+u+1, ovi+(v+1)*(uc+1)+u);
}
Mesh *new_mesh_zube(Material *m, float x0, float y0, float z0, float x1, float y1, float z1, int xg, int yg, int zg) {
  int vi=0, fi=0;
  Mesh *o=new_mesh_alloc((xg*yg+yg*zg+zg*xg)*2, ((xg+1)*(yg+1)+(yg+1)*(zg+1)+(zg+1)*(xg+1))*2);
  teegpoly(o, m, &vi, &fi, x0, y0, z0, 0, y1-y0, 0,  x1-x0, 0, 0, yg, xg);
  teegpoly(o, m, &vi, &fi, x1, y1, z1, x0-x1, 0, 0,  0, y0-y1, 0, xg, yg);
  teegpoly(o, m, &vi, &fi, x0, y0, z0, 0, 0, z1-z0,  0, y1-y0, 0, zg, yg);
  teegpoly(o, m, &vi, &fi, x1, y1, z1, 0, y0-y1, 0,  0, 0, z0-z1, yg, zg);
  teegpoly(o, m, &vi, &fi, x0, y0, z0, x1-x0, 0, 0,  0, 0, z1-z0, xg, zg);
  teegpoly(o, m, &vi, &fi, x1, y1, z1, 0, 0, z0-z1,  x0-x1, 0, 0, zg, xg);
  return mesh_recalc(o);
}

Mesh *new_mesh_matto(Mesh *o, Material *m, float xres, float yres) {
  int x, y;
  for (y=0; y<yres; y++) for (x=0; x<xres; x++) {
    mesh_v_set(o, y*xres+x, x/xres-0.5, 0, y/yres-0.5, x/xres, y/yres);
  }
  for (y=0; y<yres-1; y++) for (x=0; x<xres-1; x++) {
    mesh_p_set4(o, y*yres+x, m, y*yres+x, y*yres+x+1, (y+1)*yres+x, (y+1)*yres+x+1);
  }
  return mesh_recalc(o);
}



Mesh *new_mesh_xube(Material *m, float x0, float y0, float z0, float x1, float y1, float z1) {
  int i;
  Mesh *o=new_mesh_alloc(6, 8);
  mesh_v_set(o, 0, x0, y0, z0, (x0+z0)*(1/256.0), y0*(1/256.0));
  mesh_v_set(o, 1, x0, y0, z1, (x0+z1)*(1/256.0), y0*(1/256.0));
  mesh_v_set(o, 2, x0, y1, z0, (x0+z0)*(1/256.0), y1*(1/256.0));
  mesh_v_set(o, 3, x0, y1, z1, (x0+z1)*(1/256.0), y1*(1/256.0));
  mesh_v_set(o, 4, x1, y0, z0, (x1+z0)*(1/256.0), y0*(1/256.0));
  mesh_v_set(o, 5, x1, y0, z1, (x1+z1)*(1/256.0), y0*(1/256.0));
  mesh_v_set(o, 6, x1, y1, z0, (x1+z0)*(1/256.0), y1*(1/256.0));
  mesh_v_set(o, 7, x1, y1, z1, (x1+z1)*(1/256.0), y1*(1/256.0));
  mesh_p_set4(o, 0, m, 0, 1, 3, 2);
  mesh_p_set4(o, 1, m, 6, 7, 5, 4);
  mesh_p_set4(o, 2, m, 4, 5, 1, 0);
  mesh_p_set4(o, 3, m, 2, 3, 7, 6);
  mesh_p_set4(o, 4, m, 0, 2, 6, 4);
  mesh_p_set4(o, 5, m, 5, 7, 3, 1);

  //for (i=1; i<10; i++) {
    //mesh_v_set(o, 7+0*9+i, x0, y0+i*0.1*(y1-y0), z0, 0, 0);
    //mesh_v_set(o, 7+1*9+i, x0, y0+i*0.1*(y1-y0), z1, 0, 0);
    //mesh_v_set(o, 7+2*9+i, x1, y0+i*0.1*(y1-y0), z0, 0, 0);
    //mesh_v_set(o, 7+3*9+i, x1, y0+i*0.1*(y1-y0), z1, 0, 0);
    //mesh_p_set3(o, 5+0*9+i, m2, 7+0*9+i, 7+0*9+i, 7+1*9+i);
    //mesh_p_set3(o, 5+1*9+i, m2, 7+1*9+i, 7+1*9+i, 7+3*9+i);
    //mesh_p_set3(o, 5+2*9+i, m2, 7+3*9+i, 7+3*9+i, 7+2*9+i);
    //mesh_p_set3(o, 5+3*9+i, m2, 7+2*9+i, 7+2*9+i, 7+0*9+i);
  //}

  return mesh_recalc(o);
}


Mesh *new_mesh_camera(Material *m) {
  Mesh *o=new_mesh_alloc(10, 16);
  mesh_v_set(o,  0, -0.7, -1.1, -1.2, 0, 0);
  mesh_v_set(o,  1,  0.7, -1.1, -1.2, 0, 0);
  mesh_v_set(o,  2, -0.7,  0.8, -1.2, 0, 0);
  mesh_v_set(o,  3,  0.7,  0.8, -1.2, 0, 0);
  mesh_v_set(o,  4, -0.7, -1.1,  1, 0, 0);
  mesh_v_set(o,  5,  0.7, -1.1,  1, 0, 0);
  mesh_v_set(o,  6, -0.7,  0.8,  1, 0, 0);
  mesh_v_set(o,  7,  0.7,  0.8,  1, 0, 0);
  mesh_v_set(o,  8, -0.666, -0.5,  1, 0, 0);
  mesh_v_set(o,  9,  0.666, -0.5,  1, 0, 0);
  mesh_v_set(o, 10, -0.666,  0.5,  1, 0, 0);
  mesh_v_set(o, 11,  0.666,  0.5,  1, 0, 0);
  mesh_v_set(o, 12, -1.303, -0.97,  2, 0, 0);
  mesh_v_set(o, 13,  1.303, -0.97,  2, 0, 0);
  mesh_v_set(o, 14, -1.303,  0.97,  2, 0, 0);
  mesh_v_set(o, 15,  1.303,  0.97,  2, 0, 0);
  mesh_p_set4(o, 0, m,  0,  2,  3,  1);
  mesh_p_set4(o, 1, m,  6,  4,  5,  7);
  mesh_p_set4(o, 2, m,  4,  0,  1,  5);
  mesh_p_set4(o, 3, m,  2,  6,  7,  3);
  mesh_p_set4(o, 4, m,  0,  4,  6,  2);
  mesh_p_set4(o, 5, m,  5,  1,  3,  7);
  mesh_p_set4(o, 6, m,  8, 12, 13,  9);
  mesh_p_set4(o, 7, m,  9, 13, 15, 11);
  mesh_p_set4(o, 8, m, 11, 15, 14, 10);
  mesh_p_set4(o, 9, m, 10, 14, 12,  8);
  return mesh_recalc(o);
}
Mesh *new_mesh_icosohedron(Material *m, float size) {
  Mesh *o=new_mesh_alloc(20, 12);
  size*=1.0/fsqrt(3.618);
  mesh_v_set(o,  0,  size*1.618,  size, 0, 0, 0); mesh_v_set(o,  1, 0,  size*1.618,  size, 0, 0);
  mesh_v_set(o,  2,  size, 0,  size*1.618, 0, 0); mesh_v_set(o,  3,  size*1.618, -size, 0, 0, 0);
  mesh_v_set(o,  4,  size, 0, -size*1.618, 0, 0); mesh_v_set(o,  5, 0,  size*1.618, -size, 0, 0);
  mesh_v_set(o,  6, 0, -size*1.618,  size, 0, 0); mesh_v_set(o,  7, -size, 0,  size*1.618, 0, 0);
  mesh_v_set(o,  8, -size*1.618,  size, 0, 0, 0); mesh_v_set(o,  9, -size, 0, -size*1.618, 0, 0);
  mesh_v_set(o, 10, 0, -size*1.618, -size, 0, 0); mesh_v_set(o, 11, -size*1.618, -size, 0, 0, 0);
  mesh_p_set3(o,  0, m,  0,  1,  2); mesh_p_set3(o,  1, m,  9, 10, 11);
  mesh_p_set3(o,  2, m,  0,  2,  3); mesh_p_set3(o,  3, m,  8,  9, 11);
  mesh_p_set3(o,  4, m,  0,  3,  4); mesh_p_set3(o,  5, m,  7,  8, 11);
  mesh_p_set3(o,  6, m,  0,  4,  5); mesh_p_set3(o,  7, m,  6,  7, 11);
  mesh_p_set3(o,  8, m,  0,  5,  1); mesh_p_set3(o,  9, m, 10,  6, 11);
  mesh_p_set3(o, 10, m,  2,  1,  7); mesh_p_set3(o, 11, m,  4, 10,  9);
  mesh_p_set3(o, 12, m,  3,  2,  6); mesh_p_set3(o, 13, m,  5,  9,  8);
  mesh_p_set3(o, 14, m,  4,  3, 10); mesh_p_set3(o, 15, m,  1,  8,  7);
  mesh_p_set3(o, 16, m,  5,  4,  9); mesh_p_set3(o, 17, m,  2,  7,  6);
  mesh_p_set3(o, 18, m,  1,  5,  8); mesh_p_set3(o, 19, m,  3,  6, 10);
  return mesh_recalc(o);
}
Mesh *new_mesh_sphere(Material *m, int us, int vs, float size) {
  Mesh *o=new_mesh_alloc((vs-1)*us*2, (vs-1)*us+2);
  int u, v, u2, v2;
  float vr=1.0/vs, ur=1.0/us, vk=pi*vr, uk=pi*ur;
  mesh_v_set(o, 0, 0,  ffabs(size), 0, 0.5, 1);
  mesh_v_set(o, 1, 0, -ffabs(size), 0, 0.5, 0);
  for (v=0; v<vs-1; v++) for (u=0; u<us; u++) {
    mesh_v_set(o, 2+v*us+u,
      fsin((u*2+v)*uk)*fsin((v+1)*vk)*size,
      fcos((v+1)*vk)*ffabs(size),
      fcos((u*2+v)*uk)*fsin((v+1)*vk)*ffabs(size), fprem(4-(u+v*0.5)*ur, 1), (vs-1-v)*vr);
  }
  for (u=0; u<us; u++) mesh_p_set3(o, u, m, 0, 2+u, 2+(u+1)%us);
  for (v=0; v<vs-2; v++) for (u=0; u<us; u++) {
    v2=v+1; u2=(u+1)%us;
    mesh_p_set3(o, (v*2+1)*us+u, m, 2+ v*us+u, 2+v2*us+u , 2+v*us+u2);
    mesh_p_set3(o, (v*2+2)*us+u, m, 2+v2*us+u, 2+v2*us+u2, 2+v*us+u2);
  }
  for (u=0; u<us; u++) mesh_p_set3(o, (vs*2-3)*us+u, m, 1, 2+(vs-2)*us+(u+1)%us, 2+(vs-2)*us+u);
  return mesh_recalc(o);
}
Mesh *new_mesh_hemisphere(Material *m, int us, int vs, float size) {
  Mesh *o=new_mesh_alloc((vs*2-1)*us, vs*us+1);
  int u, v, u2, v2;
  float vr=1.0/vs, ur=1.0/us, vk=pi*0.5*vr, uk=pi*ur;
  mesh_v_set(o, 0, 0, ffabs(size), 0, 0.5, 1);
  for (v=0; v<vs; v++) for (u=0; u<us; u++) {
    mesh_v_set(o, 1+v*us+u,
      fsin((u*2+v)*uk)*fsin((v+1)*vk)*size,
      fcos((v+1)*vk)*ffabs(size),
      fcos((u*2+v)*uk)*fsin((v+1)*vk)*ffabs(size), fprem(12-3*(u+v*0.5)*ur, 1), (vs-1-v)*vr);//, (u+v*0.5)*ur, v*vr);
  }
  for (u=0; u<us; u++) mesh_p_set3(o, u, m, 0, 1+u, 1+(u+1)%us);
  for (v=0; v<vs-1; v++) for (u=0; u<us; u++) {
    v2=v+1; u2=(u+1)%us;
    mesh_p_set3(o, (v*2+1)*us+u, m, 1+ v*us+u, 1+v2*us+u , 1+v*us+u2);
    mesh_p_set3(o, (v*2+2)*us+u, m, 1+v2*us+u, 1+v2*us+u2, 1+v*us+u2);
  }
  return mesh_recalc(o);

}

/*Mesh *mesh_levitaedget(Mesh *o, float k) {
  int i;
  k=fsqrt(k);
  for (i=0; i<o->ec; i++) if (!o->el[i]->p2)
    v_mul(&o->el[i]->v1->pos, &o->el[i]->v1->pos, k),
    v_mul(&o->el[i]->v2->pos, &o->el[i]->v2->pos, k);
  return o;
}*/

/*Mesh *new_mesh_maasto(Material *m, char *field) {
  int x, y, xx, yy, i, j;
  Vertex3d *w1;
  Poly3d *f;
  float a1, a2, r=15.95;
  #define res 30

  Mesh *o=new_mesh_alloc(2*(res-1)*(res-1), res*res);

  for (y=0; y<res; y++) for (x=0; x<res; x++) {
    float u, v;
    int z=0;
    xx=x;//-1; if (xx<0) xx=0, z=1; if (xx>res-3) xx=res-3, z=1;
    yy=y;//-1; if (yy<0) yy=0, z=1; if (yy>res-3) yy=res-3, z=1;
    u=(xx+0.01)/(res-1+0.02);
    v=(yy+0.01)/(res-1+0.02);
    mesh_v_set(o, y*res+x,
      (x-res/2.0)*(512/res),
      field[(int)(v*512)*512+(int)(u*512)]*0.14,
      (y-res/2.0)*(256/res), (u-0.5)*1.0+0.5, (v-0.5)*1.0+0.5);
    if (z) {
      float r=3000.0/fsqrt(fsqr(o->vl[y*res+x]->pos.x)+fsqr(o->vl[y*res+x]->pos.z));
      o->vl[y*res+x]->pos.x*=r; o->vl[y*res+x]->pos.z*=r;
    }
  }
  for (y=0; y<res-1; y++) {
    for (x=0; x<res-1; x++) {
      float h;
      i=y*res+x;
      h=field[(o->vl[i]->v+o->vl[i+res+1]->v>>16)*512
             +(o->vl[i]->u+o->vl[i+res+1]->u>>16)]*0.14*2;
      if (ffabs(h-o->vl[i]->pos.y-o->vl[i+res+1]->pos.y)<ffabs(h-o->vl[i+1]->pos.y-o->vl[i+res]->pos.y)) {
        mesh_p_set3(o, (y*(res-1)+x)*2  , m, i, i+res, i+res+1);
        mesh_p_set3(o, (y*(res-1)+x)*2+1, m, i, i+res+1, i+1);
      } else {
        mesh_p_set3(o, (y*(res-1)+x)*2  , m, i, i+res, i+1);
        mesh_p_set3(o, (y*(res-1)+x)*2+1, m, i+1, i+res, i+res+1);
      }
    }
  }
  return mesh_recalc(o);
}
*/




