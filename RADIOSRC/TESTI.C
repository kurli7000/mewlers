#include <conio.h>
#include <ctype.h>
#include <libya\libya.h>
#include <libya\rawkb.h>
#include "user.h"
#include "obuja.h"
#include "functs.h"
#include "font.h"
#include "phile.h"
#include "mesh.h"
#include <libya\modplyer.h>
#include <libya\video.h>
#include "inifile.h"

__int64 cnter2;

#define once(x) do { static int onceflag=1; if (onceflag) { x; onceflag=0; } } while (0)
#define onceroot(x) do { static int onceflag=1; if (onceflag) { Ctx *oldctx=ctx_switch(rootctx); { x; } onceflag=0; ctx_switch(oldctx); } } while (0)
#define from(c, x) do { Ctx *oldctx=ctx_switch(c); { x; } ctx_switch(oldctx); } while (0)
#define disc(x) do { Ctx *oldctx=ctx(); { x; } ctx_switch(oldctx); } while (0)
float guess(float t) { return 2.0/(fexp2(t)+fexp2(-t)); }
Mesh *read3ds(char *, Material *);



static Video *video;
static Inifile *cfg;
Fontti *fnt;


static Texture *maa_txt, *taivas_txt, *la_txt, *hava_txt, *anta_txt, *pilvet_txt, *rain_txt;
static Material *maa, *taivas, *la, *hava, *anta, *pilvet, *rain;
static Mesh *maapallo, *maataivas, *los, *havaji, *puolihava, *antar, *pilvitaivas, *matto, *rainier;




Lfb *new_img(int wid, int hgt, int dep) {
  Lfb *l=newobj(Lfb);
  l->buf=mem((l->pitch=(l->width=wid)*(l->depth=dep))*(l->height=hgt));
  return l;
}



Lfb *lfb_clear(Lfb *l) { stosd(l->buf, 0, l->height*l->pitch>>2); return l; }
Lfb *img_loadtga(Phile *ph) {
  struct {
    char idlen;
    char paltype;
    char imtype;
    char palstuf[5];
    short x, y;
    short width, height;
    char bpp;
    char flags;
  } hdr;
  int size, x, y, i, j, k, pitch;
  Lfb *lfb; //=memz(sizeof(Lfb));
  //FILE *fp=fopen(fname, "rb");
  phile_seek(ph, 0); phile_read(ph, &hdr, 18);
  //fread(&hdr, 1, 18, fp);
  phile_seek(ph, hdr.idlen+18);
  //lfb->width=hdr.width;
  //lfb->height=hdr.height;
  //lfb->depth=hdr.bpp+7>>3;
  //lfb->pitch=lfb->depth*lfb->width;
  lfb=new_img(hdr.width, hdr.height, hdr.bpp+7>>3);
  phile_read(ph, lfb->buf, lfb->pitch*lfb->height);
  //fclose(fp);
  if (~hdr.flags&0x20) for (y=0; y*2<lfb->height; y++) {
    int y1=y*lfb->pitch, y2=(lfb->height-1-y)*lfb->pitch;
    for (x=0; x<lfb->pitch; x++)
      j=lfb->buf[y1+x], lfb->buf[y1+x]=lfb->buf[y2+x], lfb->buf[y2+x]=j;
  }
  if (hdr.bpp==16||hdr.bpp==15) for (x=0; x+x<lfb->height*lfb->pitch; x++)
    ((short*)lfb->buf)[x]+=((short*)lfb->buf)[x]&0xffe0;
  return lfb;
}
Lfb *img_to16bpp(Lfb *l) {
  if (l->depth==3) {
    int x, y;
    for (y=0; y<l->height; y++)
      for (x=0; x<l->width; x++) ((short*)(l->buf+y*l->pitch))[x]=l->buf[y*l->pitch+x*3+2]<<8&0xf800|l->buf[y*l->pitch+x*3+1]<<3&0x7e0|l->buf[y*l->pitch+x*3]>>3&0x1f;
    l->depth=2;
  }
  return l;
}
void img_scale(Lfb *dest, Lfb *src,
               int xc, int yc, int xs, int ys) {
  int x, y, xx, yy, a, b, c;
  char xx2, yy2;
  char *sourke, *zourke2, *zourke3;
  unsigned short *dp;

  //for (y=0; y<y1; y++) for (x=0; x<80; x++) *dest++=0;
  for (y=0; y<dest->height; y++) {
    yy=y*ys+yc; yy2=yy>>12&0xf;
    sourke=src->buf+(yy>>16)*src->pitch;
    xx=xc; b=0;
    //for (x=0; x<x1; x++) *dest++=0;
    dp=lfbrow2(dest, y);
    for (x=0; x<dest->width; x+=2) {
      xx2=xx>>12&0xf;
      zourke2=sourke+(xx>>16); zourke3=zourke2+src->pitch;
      c=(zourke2[0]<<4)+(zourke2[1]-zourke2[0])*xx2;
      a=(c<<4)+((zourke3[0]<<4)+(zourke3[1]-zourke3[0])*xx2-c)*yy2+0x40;
      xx+=xs;
      xx2=xx>>12&0xf;
      zourke2=sourke+(xx>>16); zourke3=zourke2+src->pitch;
      c=(zourke2[0]<<4)+(zourke2[1]-zourke2[0])*xx2;
      c=(c<<4)+((zourke3[0]<<4)+(zourke3[1]-zourke3[0])*xx2-c)*yy2+0x40;
      //*dp++=(a+b>>9)+(a&0xff00)+((a+c<<7)&0xff0000)+(c<<16&0xff000000);
      *dp++=a>>8|c&0xff00;
      b=c;
      xx+=xs;
    }
    //for (; x<80; x++) *dest++=0;
  }
  //for (; y<200; y++) for (x=0; x<80; x++) *dest++=0;
}







/*
Lfb *kewlplur(Lfb *l) {
  int x, y, i, j;
  for (y=0; y<l->height; y++) {
    short *p=(short*)(l->buf+y*l->pitch);
    i=p[0]*256;
    for (x=0; x<l->width; x++, p++) {
      //p[0]=(i+=p[0]*256-i+4>>3)+128>>8; p++;
       j=p[0]*4+((i+=p[0]*256-i+4>>3)+2>>2); if (j>65535) j=65535; p[0]=j;
    }
  }
  return l;
}
Lfb *vihrea(Lfb *l) {
  int x, y, i;
  for (y=0; y<l->height; y++) {
    unsigned short *p=(unsigned short*)(l->buf+y*l->pitch);
    i=p[0]*256;
    for (x=0; x<l->width; x+=4, p+=4) {
      p[0]=p[0]>>5&0x7e0; p[1]=p[1]>>5&0x7e0; p[2]=p[2]>>5&0x7e0; p[3]=p[3]>>5&0x7e0;
    }
  }
  return l;
}
*/




/*
volatile char keys[144], kcnt[144];
void (__interrupt __far __loadds *oldk)();
static void __interrupt __far __loadds newk() {
  int a;
  a=inb(0x60);
  if (a<0x80) { keys[a]=1; kcnt[a]++; } else keys[a-0x80]=0;

  a=keys[128]; keys[128]=keys[29]&&keys[56]&&keys[1]; kcnt[128]+=keys[128]&&!a;
  //a=keys[128]; keys[128]=keys[29]&&keys[56]&&keys[83]; kcnt[128]+=keys[128]&&!a;
  a=keys[129]; keys[129]=keys[29]&&keys[69]; kcnt[129]+=keys[129]&&!a;
  outb(0x20, 0x20);

  if ((kcnt[128]&1)&&!keys[128]) {
    err("Now it's safe to turn off your computer.");
  }
}
static void unint(void *a) {
  set_intr(0x9, oldk);
}
static void setint() {
  oldk=get_intr(0x9); set_intr(0x9, newk);
  ctx_atfree(curctx, unint, 0);
}*/



void init_engine() {
  extern float monitor_gamma;
  //extern float texel_size;
  cfg=lueini("demo.ini");
  //texture_contrast=ini_float(cfg, "texture_contrast", 15.);
  monitor_gamma=ini_float(cfg, "monitor_gamma", 1.8);
  //palette_gamma=ini_float(cfg, "ramp_gamma_correct", .6);
  //force_ramp=ini_int(cfg, "force_ramp", 0);
  //texel_size=ini_float(cfg, "texel_size", 0.);
  if (ini_int(cfg, "disable_lfb", 0)) use_lfb=0;
  {
    int wid=ini_int(cfg, "width", 320), hgt=ini_int(cfg, "height", 200),
      bpp=ini_int(cfg, "bpp", 16), ydiv=ini_int(cfg, "y_divisor", 1),
      wid2=ini_int(cfg, "window_width", wid), hgt2=ini_int(cfg, "window_height", hgt/ydiv);
    video=new_vid_screen(wid, hgt, bpp);
    if (!video) {
      int ch, i;
      printf("%ix%ix%i not found\nuse 320x200x8 grayscale instead? [no/no]? ", wid, hgt, bpp); fflush(stdout);
      ch=getch(); exit(1);
    }
    vid_ydiv(video, ydiv);
    vid_cut(video, wid-wid2>>1, hgt/ydiv-hgt2>>1, wid2, hgt2);
  }
  //if (!vid_set_gamma(video, 1.0/palette_gamma)) palette_gamma=1.;
}
void loader(int i) {
  int x;
  Lfb *lfb=vid_openlfb(video);
  for (x=0; x<lfb->pitch*lfb->height; x++) lfb->buf[x]=0;
  if (i>0) gfxprintf(lfb, fnt, 20, 20, "loading...");
  if (i>1) gfxprintf(lfb, fnt, 20, 40, "music               ok");
  if (i>2) gfxprintf(lfb, fnt, 20, 60, "zoomerdata       ok");
  if (i>3) gfxprintf(lfb, fnt, 20, 80, "maapallo          ok");
  if (i>4) gfxprintf(lfb, fnt, 20, 100, "los angeles    ok");
  if (i>5) gfxprintf(lfb, fnt, 20, 120, "hawaii              ok");
  if (i>6) gfxprintf(lfb, fnt, 20, 140, "antarctis         ok");
  if (i>7) gfxprintf(lfb, fnt, 20, 160, "pilvitaivas     ok");
  vid_closelfb(video);
}







/*
 *
 *   earthpart
 *
 */
void loadear() {
  onceroot(
    maa_txt=new_txt_load_tga("..\\ps2_data\\maapin51.tga", 1.5);
    maa=mat_set_txt1(new_mat(), maa_txt);
    maapallo=mesh_wraparound(new_mesh_sphere(maa, 15, 15, 50));

    taivas_txt=new_txt_load_tga("..\\ps2_data\\maapil51.tga", 1.8);
    taivas=mat_set_txt1(new_mat(), taivas_txt);
    maataivas=mesh_wraparound(new_mesh_sphere(taivas, 20, 20, 50.5));
  );
}
void pyoritamaata(Lfb *lfb, float t) {
  float dist;
  Vector cam, sat, erth, v1, v2, v3;
  Space *spc=new_space();

  loadear();
  dist=500;
  v_create(&cam, fcos(t*1.5256), fsin(t*1.8775), fsin(t*2.5456));
  v_create(&erth, 0, 0, 0);
  v_create(&sat, fsin(t*1.2256)*dist, fsin(t*.8775)*300, fcos(t*1.2256)*dist);
  v_add(&cam, &cam, &sat);
  v_mul(&cam, &cam, 1.01);

  space_fill(spc, new_v_c(.0000001, .0000001, .0000001));
  space_setambient(spc, new_v_c(.04, .03, .07));
  //space_addlight(spc,
    //v_mul(new_v(), v_norm(new_v_c(1, .3, 0)), 20000.),
    //new_v_c(500000000., 500000000., 500000000.));
  {
    float t1=t*.5;
    Matrix *m1=new_m_create(new_v_c(fcos(t1), 0, fsin(t1)), &v_cj, new_v_c(-fsin(t1), 0, fcos(t1)), &erth);
    space_addmesh(spc, maapallo, m1);
  }
  space_render(spc, lfb, new_m_lookat(&cam, &erth, v_norm(new_v_c(-.3, 1, 0))), 10, 0);
}





/*
 *
 *  Los angles
 *
 */
void load_los() {
  onceroot(
    static Space *spc;
    Phile *ph;
    //la_txt=new_txt_load_tga("..\\ps2_data\\rain-map.tga", 2.);
    la_txt=new_txt_load_tga("..\\ps2_data\\lsmall.tga", 2.);
    la=mat_set_txt1(new_mat(), la_txt);
    if (0) {
      Ctx *d;
      los=mesh_recalc(new_mesh_maasto(la, img_loadtga(new_phile_disk_ro("..\\ps2_data\\bigdem.tga")), 3000, 70, 0));
      d=ctx();
      mesh_write(los, new_phile_disk_create("..\\ps2_data\\la3000.obu"));
      ctx_free(d);
    } else {
      los=new_mesh_read(ph=new_phile_disk_ro("..\\ps2_data\\la3000.obu"), la); phile_close(ph);
    }
  );
}
extern void (*postfx)(int *, int, int, int);
Lfb *bigture;
void dohassu(int *buf, int x, int y, int w) {
  int i;
  #define darken(x) ((x)>>1&0x7bef)
  //for (i=0; i<w; i++) buf[i]=buf[i]<<(i&15);
  for (i=0; i<w; i++) buf[i]=darken(buf[i])+darken(lfbpix2(bigture, x+i, y));
}
void losangeles(Lfb *lfb, float t) {
  Vector v1, v2, v3;
  float range, juttu;
  Space *spc=new_space();
  Vector *cam=new_v_c(fsin(t*1.551)*400, 200, fcos(t*2.123)*400),
         *tgt=new_v_c(0, 100, 0);

  load_los();
  postfx=dohassu;
  if (t<1) juttu=fsqr(1-t)*500+30; else juttu=30;
  range=5;

  //v_create(&cam, fsin(t*.7423)*120*range, 70+juttu+guess((t-14)*1.3)*100-guess((t-14)*10-1)*150, fsin(t*1.2123)*80*range);
  //v_create(&tgt, fsin(t*.7423+.1)*120*range*.8, 75-guess((t-14)*10-1)*200, fsin(t*1.2123+.1)*80*range*.8);

  space_setambient(spc, new_v_c(.04, .03, .07));
  space_addmesh(spc, los, &m_c_ident);
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_render(spc, lfb, new_m_lookat(cam, tgt,
    v_norm(new_v_c(-.3, 1, 0))), .7, 0);

}






/*
 *
 *  hawaii part
 *
 */
void load_hava() {
  onceroot(
    Phile *ph;
    hava_txt=new_txt_load_tga("..\\ps2_data\\hsmall.tga", 2.);
    hava=mat_set_txt1(new_mat(), hava_txt);
    //{
      //void filltri_tv(Tri2d *);
      //mat_set_filler(hava, filltri_tv);
    //}
    if (0) {
      Plane pl; pl.n.x=0; pl.n.y=-1; pl.n.z=0; pl.d=0;
      puolihava=mesh_recalc(mesh_clip_in(new_mesh_maasto(hava, img_loadtga(new_phile_disk_ro("..\\ps2_data\\ydem555.tga")), 1000, 150, -72), &pl, 1));
      mesh_write(puolihava, new_phile_disk_create("..\\ps2_data\\havajji.obu"));
    } else {
      puolihava=new_mesh_read(ph=new_phile_disk_ro("..\\ps2_data\\havajji.obu"), hava); phile_close(ph);
    }
    pilvet_txt=new_txt_load_tga("..\\ps2_data\\taevas.tga", 1.);
    //pilvet_txt=new_txt_load_mfb("mapconv\\taivas.mfb", 1.);
    pilvet=mat_set_txt1(new_mat(), pilvet_txt);
    pilvitaivas=mesh_wraparound(new_mesh_hemisphere(pilvet, 8, 4, 2000));
    //pilvitaivas=mesh_wraparound(new_mesh_hemisphere(pilvet, 30, 20, 5000));
    mesh_flipnormals(pilvitaivas);
    //{
      //void filltri_noshade(Tri2d *);
      //mat_set_filler(pilvet, filltri_noshade);
    //}
    //loader(6);
  );
}
void hawaii(Lfb *lfb, float t) {
  float aa=1+guess(t*3)*2, bee=1-guess((t-8)*.5)*.7;
  float per=0.7; //0.7+guess((t-8)*6-1)*5;
  Vector *cam=new_v_c(fsin(t*1.551)*300*aa, 52+fsin(t*.5433)*5+guess(t-18), fcos(t*2.123)*300*aa),
         *tgt=new_v_c(fsin(t*1.551*1.05+.1)*240*bee, 42+guess((t-8)*4)*1500, fcos(t*2.123*1.05+.1)*240*bee);
  Matrix *kamera=new_m_lookat(cam, tgt, v_norm(new_v_c(-.05, 1, .05)));
  Space *spc=new_space();
  static char temprgb[3*1600];
  load_hava();
  //vesi=new_mesh_xube(hava, -1000, -3, -1000, 1000, 3, 1000);

  //heijastus (taivas+saari)
  space_addmesh(spc, pilvitaivas, new_m_create(&v_ci, &v_cmj, &v_ck, &v_c0));
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, puolihava, new_m_create(&v_ci, v_mul(new_v(), &v_cj, -.7), &v_ck, &v_c0));

  //itse taivas+itse saari
  space_addmesh(spc, pilvitaivas, &m_c_ident);
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, puolihava, new_m_create(&v_ci, v_mul(new_v(), &v_cj, .7), &v_ck, &v_c0));
  space_render(spc, lfb, kamera, per, f_antialias);
}







/*
 *
 *  antarktis part
 *
 */
void load_anta() {
  onceroot(
    Phile *ph;
    anta_txt=new_txt_load_tga("..\\ps2_data\\asmall.tga", 2.);
    anta=mat_set_txt1(new_mat(), anta_txt);
    antar=new_mesh_read(ph=new_phile_disk_ro("..\\ps2_data\\antar.obu"), anta); phile_close(ph);
  );
}
void antark(Lfb *lfb, float t) {
  float per=0.7;
  Vector *cam=new_v_c(fsin(t*1.551)*400, 200, fcos(t*2.123)*400),
         *tgt=new_v_c(0, 100, 0);
  Matrix *kamera=new_m_lookat(cam, tgt, v_norm(new_v_c(-.05, 1, .05)));
  Space *spc=new_space();

  load_anta();
  //space_addmesh(spc, pilvitaivas, &m_c_ident);
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, antar, new_m_create(&v_ci, v_mul(new_v(), &v_cj, .7), &v_ck, &v_c0));
  space_render(spc, lfb, kamera, per, f_antialias);
}







/*
 *
 *  monty python part
 *
 */
void load_rain() {
  onceroot(
    Phile *ph;
    rain_txt=new_txt_load_tga("..\\ps2_data\\rsmall.tga", 2.);
    rain=mat_set_txt1(new_mat(), rain_txt);
    rainier=new_mesh_read(ph=new_phile_disk_ro("..\\ps2_data\\rainier.obu"), rain); phile_close(ph);
    //pilvet_txt=new_txt_load_tga("..\\ps2_data\\taevas.tga", 1.);
    //pilvet=mat_set_txt1(new_mat(), pilvet_txt);
    //pilvitaivas=mesh_wraparound(new_mesh_hemisphere(pilvet, 8, 4, 2000));
    //mesh_flipnormals(pilvitaivas);
  );
}
void brainier(Lfb *lfb, float t) {
  float per=2.7;
  Vector *cam=new_v_c(fsin(t*1.551)*400, 200, fcos(t*2.123)*400),
         *tgt=new_v_c(0, 100, 0);
  Matrix *kamera=new_m_lookat(cam, tgt, v_norm(new_v_c(-.05, 1, .05)));
  Space *spc=new_space();

  load_rain();
  //space_addmesh(spc, pilvitaivas, &m_c_ident);
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, rainier, new_m_create(&v_ci, v_mul(new_v(), &v_cj, .7), &v_ck, &v_c0));
  space_render(spc, lfb, kamera, per, f_antialias);
}







/*
 *
 *  tsoomerpart
 *
 */
Lfb *sfpics[8];
Lfb *tsootaus;
void loadtsoo() {
  onceroot(
    Phile *ph;
    sfpics[0]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom8.tga")); phile_close(ph);
    sfpics[1]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom7.tga")); phile_close(ph);
    sfpics[2]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom6.tga")); phile_close(ph);
    sfpics[3]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom5.tga")); phile_close(ph);
    sfpics[4]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom4.tga")); phile_close(ph);
    sfpics[5]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom3.tga")); phile_close(ph);
    sfpics[6]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom2.tga")); phile_close(ph);
    sfpics[7]=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\sfzoom1.tga")); phile_close(ph);
    tsootaus=img_loadtga(ph=new_phile_disk_ro("..\\ps2_data\\tausta1.tga")); phile_close(ph);
  );
}
Lfb *tsoomto(Lfb *s, int xp, int yp, int ositus, float t) {
  Lfb *d=new_img(s->width, s->height, s->depth);
  float z=fpow(ositus, -t);///(1+t*(ositus-1));
  img_scale(d, s, xp*(1-z)/(ositus-1)*s->width*65536.,
                  yp*(1-z)/(ositus-1)*s->height*65536.,
                  z*65536., z*65536.);
  return d;
}
Lfb *img_clone(Lfb *s) {
  Lfb *d=new_img(s->width, s->height, s->depth);
  int y;
  for (y=0; y<s->height; y++) memcpy(lfbrow1(d, y), lfbrow1(s, y), d->pitch);
  return d;
}
void lbur(Lfb *b, float k) {
  Lfb *b2=new_img(b->width>>1, b->height>>1, 1);
  int k0=k*1024.5, k1=1024-k0;
  int x, y;
  if (k<.01 || b->width<4 || b->height<4) return;
  for (y=0; y<b2->height; y++) {
    char *s=lfbrow1(b, y*2), *d=lfbrow1(b2, y);
    for (x=0; x<b2->width; x++) d[x]=s[x*2]+s[x*2+1]+s[x*2+b->pitch]+s[x*2+b->pitch+1]+2>>2;
  }
  lbur(b2, fpow(k, 2));
  for (y=0; y<b2->height*2; y++) {
    char *s=lfbrow1(b2, y>>1), *d=lfbrow1(b, y);
    for (x=0; x<b2->width*2; x++) d[x]=d[x]*k1+s[x>>1]*k0+512>>10;
  }
}

void tsoomer(Lfb *lfb, float t) {
  Lfb *l1;
  static Lfb *kuva1, *kuva2, *kuva1b;
  static Ctx *c;
  int pts[][3]={{4, 4, 7}, {0, 1, 3}, {4, 1, 7}, {1, 2, 4},
                {1, 1, 5}, {1, 1, 3}, {0, 2, 5}, {1, 1, 3}};
  float z, tt;
  float t2;
  int i, j, x, y;
  static int oldi=-1;
  loadtsoo();
  onceroot(c=ctx());

  t*=2; t2=fprem(fprem(t, 1)+1, 1); i=((int)t)&7;
  if (t2<.5) {
    t2=t2*2;
    if (t2<.0) t2=.0;
    l1=tsoomto(sfpics[i], pts[i][0], pts[i][1], pts[i][2], t2);
    for (y=0; y<350; y++) {
      unsigned short *d=lfbrow2(lfb, y);
      char *s=lfbrow1(l1, y);
      for (x=0; x<350; x++) d[x]=(s[x]>>3)*2113;
      s=lfbrow1(tsootaus, y);
      for (x=350; x<640; x++) d[x]=(s[x*3+2]<<8&0xf800|s[x*3+1]<<3&0x7e0|s[x*3]>>3&0x1f);
    }
  } else {
    int k, k1;
    t2=(t2-.5)*2;
    k=(1-fsqr((1-t2)*(1-t2)))*256; k1=256-k;
    j=i+1&7;
    if (i!=oldi) {
      ctx_flush(c);
      from(c,
        kuva1=tsoomto(sfpics[j], pts[j][0], pts[j][1], pts[j][2], 0);
        kuva2=tsoomto(sfpics[i], pts[i][0], pts[i][1], pts[i][2], 1);
      );
      oldi=i;
    }
    //lbur(l1, 1, 1, fpow(1-t2, .4)*.99);
    kuva1b=img_clone(kuva1);
    disc(lbur(kuva1b, (1-fpow(t2, 2.5))*.99));
    for (y=0; y<350; y++) {
      unsigned short *d=lfbrow2(lfb, y);
      char *s1=lfbrow1(kuva1b, y), *s2=lfbrow1(kuva2, y);
      for (x=0; x<350; x++) d[x]=(s1[x]*k+s2[x]*k1+128>>11)*2113;
    }
  }
}






/*
 *
 *  satel skene
 *
 */
static Mesh *satel;
void load_sat() {
  satel=read3ds("..\\ps2_data\\satellit.3ds",
    mat_set_txt1(new_mat(), new_txt_load_tga("..\\ps2_data\\blasm.tga", 1.))
  );
}
void satskene(Lfb *lfb, float t) {
  Space *spc=new_space();
  Vector *cam=new_v_c(fsin(t*1.551)*3000, fsin(t*.5433)*1500, fcos(t*2.123)*3000),
         *tgt=new_v_c(0, 0, 0);
  Matrix *kamera=new_m_lookat(cam, tgt, v_norm(new_v_c(-.05, 1, .05)));

  onceroot(load_sat());
  space_setambient(spc, new_v_c(.4, .3, .5));
  space_addlight(spc,
    v_mul(new_v(), v_norm(new_v_c(.3, .5, 1)), 30000.),
    new_v_c(500000000., 500000000., 500000000.));
  space_addmesh(spc, satel, &m_c_ident);

  space_render(spc, lfb, kamera, 2.7, f_antialias);
}







/*
 *
 *  p„„ohjelma
 *
 */
int main(int argc, char **argv) {
  float t;
  Modplayer *musa;
  FILE *f;
  Phile *ph;
  Lfb *fonbuf=new_img(320, 180, 2);
  int i;
  __int64 oldtsc, tsc;
  Lfb *lfb;

  fnt=getfont_scifi();
  musa=new_modplayer("..\\ps2_data\\tranke2.xm");
  init_engine();
  //err("vity");
  rawkb(simplekbd, 0);

  lfb=vid_openlfb(video); vid_closelfb(video);



  bigture=img_to16bpp(img_loadtga(new_phile_disk_ro("..\\ps2_data\\tausta2.tga")));


  modplayer_play(musa);
  //for (i=0; i<24; i++) modplayer_fwd(musa);
  for (t=0; !kcnt[1]; t=kcnt[31]&1?t+.002:modplayer_getpos(musa)) {
    Ctx *cx=ctx();
    {
      cnter2=0;
      lfb=vid_openlfb(video);

      //hawaii(lfb, t*4.5);
      //losangeles(lfb, t*13);
      //pyoritamaata(lfb, t*1);
      //tsoomer(lfb, t);
      if (0) ;
      //else if (t<8) satskene(lfb, t);
      //else if (t<14) brainier(lfb, t-8);
      //else if (t<20) losangeles(lfb, t-14);
      //else if (t<30) pyoritamaata(lfb, t-20);
      //else if (t<34) tsoomer(lfb, t-30);
      else if (t<50) hawaii(lfb, t-34);
      else if (t<64) antark(lfb, t-50);
      else lfb_clear(lfb);

      oldtsc=tsc; tsc=rdtsc();
      gfxprintf(lfb, fnt, 20,  60, "fps = %.2f", 166000000.0/(tsc-oldtsc));
      gfxprintf(lfb, fnt, 20,  90, "a = %i", memfree());
      gfxprintf(lfb, fnt, 20, 120, "t = %.2f", t);
      gfxprintf(lfb, fnt, 20, 150, "fx = %.4f", cnter2/(float)(tsc-oldtsc));
      //fadeimg(lfb, fonbuf, 65536, 65536);

      vid_closelfb(video);
    }
    if (kcnt[78]) modplayer_fwd(musa), kcnt[78]=0;
    while (kcnt[129]&1) ;
    ctx_free(cx);
  }
  return 0;
}



