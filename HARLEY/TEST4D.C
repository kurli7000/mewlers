typedef unsigned short ushort;
typedef signed char schar;

#include "video.h"
#include "mxmplay2.h"
#include "tinyfile.h"
#include <lib2\bugs.h>
#include <lib2\fasm.h>
#include <lib2\asm.h>
//#include <lib2\vectorz.h>
#include <lib2\mem.h>


Lfb *videomem;
extern char mjuzic[100000];
Xmpgds *mdata=(Xmpgds*)mjuzic;
#define ulkona 1234567890
#define valom 128

int rtbl2[16384];
#define rtbl (rtbl2+8192)

int paletti1[16384], paletti2[16384], paletti3[16384], paletti4[16384];
int paletti5[16384], paletti6[16384];
char hue[64000];
char logo1[320*50], logo2[320*50];

char dbuf2[64000], dbuf3[64000];

int valot_x[valom], valot_y[valom], valot_z[valom];
schar sprite1[64*64], sprite2[64*64];
char block[64*64];
char block2[10*10];
char sivupee[20*5];

char mappi1[512*512], mappi2[128*128], mappi3[128*128], mappi4[128*128];
char pallo[]={1,1,1,1};
char putki[]={0,1,1,1};
//char isoputki[]={1,1,0,50};
char taso[]={1,0,0,1};
char *opu;
char *texture;
int mapsx, mapsy, wrapy, wrapx;

char nakutus[]=
{6,0,1,0,6,0,1,0,
 6,0,3,0,6,0,8,0,
 6,0,8,0,3,0,1,0,
 8,0,1,0,6,0,3,0,
 6,0,3,0,6,0,1,0,
 6,0,3,0,6,0,8,0,
 6,0,8,0,3,0,1,0,
 8,0,1,0,1,0,3,0};

int xres, yres, xmov, ymov;
int valoxres, valoyres;

int *valotbl;
int *radiaali;
char *dbuf;
int *tunneli;

static int tbl[1024], tbl2[256];
int tabloid[512];


typedef struct {
  int u, v, c, t;
} gridfx;
gridfx pallero[41*26];

typedef struct {
  int x, y;
} dot;

typedef struct {
  float x, y, z;
} vector;

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
  yvec.x=1.0; yvec.y=0.5; yvec.z=0.1; normalisoi(&yvec);
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


//////////////////////////////////////////////////// FONTTISYSTEEMI


typedef struct {
  int xs, ys, xc, yc;
  int bpp, fill2, fill3, fill4;
  char data[];
} Sprite;
typedef struct {
  Sprite *chars[256];
  int chwidth[256];
  int spwidth;
} Fontti;

void gfxputs(char *dest, Fontti *fon, int x0, int y0, char *c, int leve, int br);
extern Fontti *getfont_();
extern Fontti *getfont_21aa();
//void gfxprintf(Lfb *dest, Fontti *fon, int x0, int y0, char *fmt,...);

Fontti *fun;

void putsprite(char *dest, int x0, int y0, Sprite *s, int leve, int br) {
  int x, y, di;
  char *src;
  char  *db;
  short *dw;
  int   *dd;

  if (!s) return;
  src=s->data;
  //di=dest->pitch-s->xs;
  for (y=0; y<s->ys; y++) {
    db=(dest+(y0+y-s->yc)*leve)+x0-s->xc;
    for (x=0; x<s->xs; x++) *db=rtbl[*db+(*src*br>>2)], db++, src++;
  }
}


void gfxputs(char *dest, Fontti *fon, int x0, int y0, char *c, int leve, int br) {
  Sprite *s;

  for (; *c!=0; c++) {
    if ((s=fon->chars[*c])==0) x0+=fon->spwidth; else {
      putsprite(dest, x0, y0, s, leve, br); x0+=fon->chwidth[*c]+2;
    }
  }
}




//////////////////////////////////////////////////// PASKAA

void exit();

int rand() { static int seed=0x12345679; return (unsigned)(seed=seed*0x97654321+1)>>17; }

void move32(void *zourke, void *dezt, int kount);
#pragma aux move32= "cld" "rep movsd" parm[esi][edi][ecx];
void fill32(void *dezt, int kount, int num);
#pragma aux fill32= "cld" "rep stosd" parm[edi][ecx][eax];
float fabs(float a);
#pragma aux fabs= "fabs" parm [8087] value [8087];
static long qftoltemp;
long qftol(float);
#pragma aux qftol = "fistp dword ptr[qftoltemp]" \
                    "mov eax, dword ptr[qftoltemp]" parm [8087] value [eax];
void qftolp(int*, float);
#pragma aux qftolp = "fistp dword ptr[eax]" parm [eax][8087];
void dosputs(char *stringi);
#pragma aux dosputs=\
  "mov ah,9"        \
  "int 21h"         \
  parm [edx] modify [eax ebx ecx edx esi edi];

float musatimer() {
  return ((mdata->curord*64+mdata->currow)+(mdata->curtick
    +(xmpGetTimer()-mdata->maxtimerrate)/(float)(mdata->stimerlen))/mdata->curtempo);
}

void ulos(int port, char data);
#pragma aux ulos= "out dx, al" parm[edx][al];
char sisaan(int port);
#pragma aux sisaan= "in al,dx" parm[edx][al] value[al];

void __interrupt __far (*get_intr(int no))();
#pragma aux get_intr="mov eax,0x204""int 0x31" parm [ebx] value [cx edx] modify [eax ebx ecx edx esi edi];
void set_intr(int no, void __interrupt __far (*f)());
#pragma aux set_intr="mov eax,0x205""int 0x31" parm [ebx][cx edx] modify [eax ebx ecx edx esi edi];

static volatile char keys[144], kcnt[144];
void __interrupt __far (*oldk)();
void __interrupt __far __loadds newk() {
  int a;

  a=sisaan(0x60);
  if (a<0x80) { keys[a]=1; kcnt[a]++; } else keys[a-0x80]=0;

  a=keys[128]; keys[128]=keys[29]&&keys[56]&&keys[83]; kcnt[128]+=keys[128]&&!a;
  a=keys[129]; keys[129]=keys[29]&&keys[69]; kcnt[129]+=keys[129]&&!a;
  ulos(0x20, 0x20);

  if ((kcnt[128]&1)&&!keys[128]) {
    set_intr(0x9, oldk);
    _asm {
      mov eax,3h
      int 10h
    };
    dosputs("ahahsidurhasorehawoher\r\n$");
    exit(1);
  }
}


int pou1(float t, float p) {
  int pos, br;
  pos=(int)(t*65536)&65535;
  if (pos<12288)               br=64000-pos*p;            //0
  if (pos>=12288 && pos<20480) br=64000-(pos-12288)*p;    //12
  if (pos>=20480 && pos<36864) br=64000-(pos-20480)*p;    //20
  if (pos>=36864 && pos<45056) br=64000-(pos-36864)*p;    //36
  if (pos>=45056 && pos<53248) br=64000-(pos-45056)*p;    //44
  if (pos>=53248)              br=64000-(pos-53248)*p;    //52
  if (br<0) br=0;
  return br;
}

int pou2(float t, float p) {
  int pos, br;
  pos=(int)(t*65536)&32767;
  if (pos<12288)               br=64000-pos*p;            //0
  if (pos>=12288 && pos<20480) br=64000-(pos-12288)*p;    //12
  if (pos>=20480 && pos<36864) br=64000-(pos-20480)*p;    //20
  if (br<0) br=0;
  return br;
}

int nakunaku1(float t, float p) {
  int pos, br;
  pos=(int)(t*65536)&32767;
  if (pos<14*1024) br=65536-pos*p;
  if (pos>=14*1024 && pos<18*1024) br=65536-(pos-14*1024)*p;
  if (pos>=18*1024) br=65536-(pos-18*1024)*p;
  if (br<0) br=0;
  return br;
}
int nakunaku2(float t, float p) {
  int pos, br;
  pos=(int)(t*65536+8192)&32767;
  if (pos<16384) br=65536-pos*p;
  if (pos>=16384) br=65536-(pos-16384)*p;
  if (br<0) br=0;
  return br;
}


//////////////////////////////////////////////////// PRECALCIT



void teepaljetti(int *p, float r1, float g1, float b1, float r2, float g2, float b2) {
  int x, y, r, g, b, rr, gg, bb;
  for (y=0; y<64; y++) {
    rr=r1+y*r2;
    gg=g1+y*g2;
    bb=b1+y*b2;
    for (x=0; x<256; x++) {
      r=rr*x>>9; g=gg*x>>9; b=bb*x>>9;
      p[y*256+x]=(rtbl[r]>>3<<11)+(rtbl[g]>>2<<5)+(rtbl[b]>>3);
    }
  }
}


void precalcipaskaa() {
  int x,y,joo;

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    joo=(x-160)*(x-160)+(y-100)*(y-100);
    hue[y*320+x]=(fsin(joo/6400.0)+fsin(x/24.0)+fcos(joo/4800.0)+fsin(fsin(x/16.0)+y/32.0)+4)*8;
  }
  teepaljetti(paletti1, 500, 1000, 700, 0, -300/64.0, 300/64.0);
  teepaljetti(paletti2, 512, 512, 512, 0, 200/64.0, 500/64.0);
  teepaljetti(paletti3, 512, 512, 512, 200/64.0, 200/64.0, 500/64.0);
  teepaljetti(paletti4, 512, 512, 1000, 300/64.0, 300/64.0, 0);
  teepaljetti(paletti5, 500, 500, 1000, 400/64.0, 0, -200/64.0);
  teepaljetti(paletti6, 500, 800, 900, 0, 200/64.0, -400/64.0);
}

void precalciflare() {
  int x, y, u, v, i;

  for (y=0; y<64; y++) for (x=0; x<64; x++) {  //sprite1
    u=x-31.99;
    v=y-31.99;
    i=-8192/(u*u+v*v+0.1)+1;
    if (i<-128) i=-128; if (i>0) i=0;
    sprite1[y*64+x]=i;
  }
  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-31.99;
    v=y-24.99;
    i=2048/(u*u+v*v*2+0.1)+sprite1[y*64+x];
    if (i<-128) i=-128; if (i>127) i=127;
    sprite1[y*64+x]=i;
  }
  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-31.99;
    v=y-37.99;
    i=1024/(u*u+v*v*3+0.1)+sprite1[y*64+x];
    if (i<-128) i=-128; if (i>127) i=127;
    sprite1[y*64+x]=i;
  }

  for (y=0; y<64; y++) for (x=0; x<64; x++) {  // sprite2
    u=x-31.99;
    v=y-31.99;
    i=-8192/(u*u+v*v+0.1)+1;
    if (i<-128) i=-128; if (i>0) i=0;
    sprite2[y*64+x]=i;
  }
  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-31.99;
    v=y-31.99;
    i=2048/(u*u+v*v+0.1)+sprite2[y*64+x];
    if (i<-128) i=-128; if (i>0) i=0;
    sprite2[y*64+x]=i;
  }
/*  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-31.99;
    v=y-23.99;
    i=128/(u*u+v*v+0.1)+sprite2[y*64+x];
    if (i<-128) i=-128; if (i>127) i=127;
    sprite2[y*64+x]=i;
  }
  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-31.99;
    v=y-39.99;
    i=128/(u*u+v*v+0.1)+sprite2[y*64+x];
    if (i<-128) i=-128; if (i>127) i=127;
    sprite2[y*64+x]=i;
  }
  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-23.99;
    v=y-31.99;
    i=128/(u*u+v*v+0.1)+sprite2[y*64+x];
    if (i<-128) i=-128; if (i>127) i=127;
    sprite2[y*64+x]=i;
  }
  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    u=x-39.99;
    v=y-31.99;
    i=128/(u*u+v*v+0.1)+sprite2[y*64+x];
    if (i<-128) i=-128; if (i>127) i=127;
    sprite2[y*64+x]=i;
  } */

  for (x=0; x<valom; x++) {
    valot_x[x]=rand()-rand();
    valot_y[x]=rand()-rand();
    valot_z[x]=rand()-rand();
  }
}

void precalculatemappi() {
  int x,y,i,u,v;
  static char temp[160*25];

  for (y=0; y<10; y++) for (x=0; x<10; x++) {
    u=x-5; v=y-5;
    block2[y*10+x]=255-(u*u+v*v)*2;
  }

  for (y=0; y<64; y++) for (x=0; x<64; x++) {
    i=255-(fsqr(y-32)+fsqr(x-32))*0.5;
    i=(i-128)*5+128;
    if (i<0) i=0; if (i>255) i=255;
    block[y*64+x]=i;
  }

  for (y=0; y<128; y++) for (x=0; x<128; x++) { // mappi2
    u=x-64; v=y-64;
    i=2048-(u*u+v*v)*2.0;
    if (i>255) i=255; if (i<1) i=1;
    mappi1[y*256+x]=i;
  }
  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    mappi1[y*256+x+128]=mappi1[y*256+x];
  }
  for (y=0; y<128; y++) for (x=0; x<256; x++) {
    mappi1[(y+128)*256+x]=mappi1[y*256+((x+64)&255)];
  }
  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    mappi2[y*128+x]=mappi1[(y*2+64&255)*256+(x*2+64&255)]+
                    mappi1[(y*2+64&255)*256+(x*2+64&255)+1]+
                    mappi1[(y*2+64&255)*256+(x*2+64&255)+256]+
                    mappi1[(y*2+64&255)*256+(x*2+64&255)+257]>>2;
  }


  for (y=0; y<32; y++) for (x=0; x<512; x++)  // mappi1
    if (y<16) mappi1[y*512+x]=255; else mappi1[y*512+x]=0;
  for (; y<480; y++) for (x=0; x<512; x++)
    mappi1[y*512+x]=block[((y+32)&63)*64+(x&63)];
  for (; y<512; y++) for (x=0; x<512; x++)
    if (y>496) mappi1[y*512+x]=255; else mappi1[y*512+x]=0;
  for (x=0; x<512*512; x++) if (mappi1[x]==0) mappi1[x]=1;

  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    u=x-64; v=y-64;
    i=2048-(u*u+v*v)*2.0;
    if (i>255) i=255; if (i<1) i=1;
    mappi3[y*128+x]=i;
  }
  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    mappi4[y*128+x]=255-mappi3[y*128+x]>>3;
  }


/*
  for (y=0; y<16; y++) for (x=0; x<16; x++) {
    for (v=0; v<16; v++) for (u=0; u<16; u++) {
      i=block2[v*16+u]*(512-fsqrt(x*x+y*y)*32);
      mappi3[(y*16+v)*512+(x*16+u)]=rtbl[i>>9];
    }
  }
  for (y=0; y<256; y++) for (x=0; x<256; x++) //siirto
    mappi3[(y+256)*512+x+256]=mappi3[y*512+x];
  for (y=0; y<256; y++) for (x=0; x<256; x++) //vas.yl„kulma
    mappi3[y*512+x]=mappi3[(511-y)*512+(511-x)];
  for (y=0; y<256; y++) for (x=0; x<256; x++) //oik.yl„kulma
    mappi3[y*512+(511-x)]=mappi3[y*512+x];
  for (y=0; y<256; y++) for (x=0; x<256; x++) //vas.alakulma
    mappi3[(511-y)*512+x]=mappi3[y*512+x];
  for (x=0; x<512*512; x++) //kontrasti
    mappi3[x]=rtbl[(mappi3[x]-128)*8+128];
*/
  fill32(temp, 1000, 0);
  fill32(logo1, 4000, 0);
  fill32(logo2, 4000, 0);
  gfxputs(temp, fun, 45, 15, "mewlers", 160, 4);
  for (y=0; y<24; y++) for (x=0; x<159; x++) {
    logo1[y*2*320+x*2    ]=temp[y*160+x];
    logo1[y*2*320+x*2+1  ]=temp[y*160+x]+temp[y*160+x+1]>>1;
    logo1[y*2*320+x*2+320]=temp[y*160+x]+temp[y*160+x+160]>>1;
    logo1[y*2*320+x*2+321]=temp[y*160+x]+temp[y*160+x+1]+temp[y*160+x+160]+temp[y*160+x+161]>>2;
  }
  fill32(temp, 1000, 0);
  gfxputs(temp, fun, 40, 15, "harlequin", 160, 4);
  for (y=0; y<24; y++) for (x=0; x<159; x++) {
    logo2[y*2*320+x*2    ]=temp[y*160+x];
    logo2[y*2*320+x*2+1  ]=temp[y*160+x]+temp[y*160+x+1]>>1;
    logo2[y*2*320+x*2+320]=temp[y*160+x]+temp[y*160+x+160]>>1;
    logo2[y*2*320+x*2+321]=temp[y*160+x]+temp[y*160+x+1]+temp[y*160+x+160]+temp[y*160+x+161]>>2;
  }

}
/*
void prekalkutsjon() {
  int x, y, i, a;
  float u, v, s, t;
  int xsize, ysize;

  xsize=xres*2;
  ysize=yres*2;
  a=valoxres*valoyres;

  //for (y=0; y<200; y++) for (x=0; x<320; x++) {
    //u=159.75-x; v=(99.75-y)*1.2;
    //t=fpatan(u, v)/pi*256.0+256.0;
    //s=fsqrt(u*u+v*v)*1.25*0.5;
    //s=fsqrt(s*128.0);
    //s+=t*(1/512.0);
    //tunneli[y*320+x]=(qftol(127-s)&0x7f)*512+(qftol(t)&0x1ff);
  //}
  for (y=0; y<ysize; y++) for (x=0; x<xsize; x++) {
    u=(xsize*0.5-0.25-x)/xsize;
    v=(ysize*0.5-0.25-y)*0.75/ysize;
    t=fpatan(u, v)/pi*(valoxres/2.0)+(valoxres/2.0);
    s=fsqrt(u*u+v*v)*1.4;
    s=fpow(s, 0.6);
    s*=valoyres;
    s+=t*(1/valoxres);
    tunneli[y*xsize+x]=(qftol(valoyres-1-s)&(valoyres-1))*valoxres+(qftol(t)&(valoxres-1));
  }

  //for (i=0; i<a; i++) radiaali[i]=1234567890;
  //for (i=0; i<xsize*ysize; i++) radiaali[tunneli[i]&(valoxres*valoyres-1)]=i;
  //for (i=a-1; i>=0; i--) if (radiaali[i]==1234567890) radiaali[i]=radiaali[i+1];
  //for (i=0; i<a; i++) if (radiaali[i]==1234567890) radiaali[i]=radiaali[i-1];

  for (y=0; y<valoyres; y++) for (x=0; x<valoxres; x++) {
    s=fpow(y/(float)valoyres, 1/0.6)/1.4;
    t=x*(pi*2/valoxres);
    u=320+fcos(t)*s*640;
    v=200-fsin(t)*s*400/0.75;
    if (u<10) u=10; if (u>629) u=629; if (v<10) v=10; if (v>389) v=389;
    radiaali[(valoyres-1-y)*valoxres+valoxres-1-x]=(int)v*640+(int)u;
  }
} */

void prekalkutsjon() {
  int x, y, i, a;
  float u, v, s, t;
  int xsize, ysize;

  xsize=xres*2;
  ysize=yres*2;
  a=valoxres*valoyres;

  //for (y=0; y<200; y++) for (x=0; x<320; x++) {
    //u=159.75-x; v=(99.75-y)*1.2;
    //t=fpatan(u, v)/pi*256.0+256.0;
    //s=fsqrt(u*u+v*v)*1.25*0.5;
    //s=fsqrt(s*128.0);
    //s+=t*(1/512.0);
    //tunneli[y*320+x]=(qftol(127-s)&0x7f)*512+(qftol(t)&0x1ff);
  //}
  for (y=0; y<ysize; y++) for (x=0; x<xsize; x++) {
    u=(xsize*0.5-0.25-x)/xsize;
    v=(ysize*0.5-0.25-y)*0.75/ysize;
    t=fpatan(u, v)/pi*(valoxres/2.0)+(valoxres/2.0);
    s=fsqrt(u*u+v*v)*1.4;
    s=fpow(s, 0.6);
    s*=valoyres;
    s-=t*(1.0/valoxres);
    if (s<0) s=0;//erreur("s<0");
    //tunneli[y*xsize+x]=(fist2(s)&(valoyres-1))*valoxres+((int)(t)&(valoxres-1));
    tunneli[y*xsize+x]=((int)(s)&(valoyres-1))*valoxres+((int)(t)&(valoxres-1));
  }

  //for (i=0; i<a; i++) radiaali[i]=1234567890;
  //for (i=0; i<xsize*ysize; i++) radiaali[tunneli[i]&(valoxres*valoyres-1)]=i;
  //for (i=a-1; i>=0; i--) if (radiaali[i]==1234567890) radiaali[i]=radiaali[i+1];
  //for (i=0; i<a; i++) if (radiaali[i]==1234567890) radiaali[i]=radiaali[i-1];

  for (y=0; y<valoyres; y++) for (x=0; x<valoxres; x++) {
    t=x*(1.0/valoxres);
    s=fpow((y+1.00001-t)*(1.0/valoyres), 1/0.6)/1.4;
    t*=pi*2;
    u=319.75+fcos(t)*s*640;
    v=199.75-fsin(t)*s*400/0.75;
    if (u<10) u=10; if (u>629) u=629; if (v<10) v=10; if (v>389) v=389;
    radiaali[y*valoxres+valoxres-1-x]=(int)v*640+(int)u;
  }
}






//////////////////////////////////////////////////// EFEKTIT


void teeradiaali(char *sourke, int *dest) {
  int x;

  for (x=0; x<valoxres*valoyres; x++) dest[x]=tbl2[sourke[radiaali[x]]];
}


void blur2(int *dest, int delta, int p) {
  int *d;
  int size=valoxres*valoyres;

  p>>=2;

  if (delta<0) {
    for (d=dest-delta; d<dest+size;  d++) { d[0]+=(d[delta]-d[0])*p>>14; }
  } else {
    for (d=dest+(size-1)-delta; d>=dest; d--) { d[0]+=(d[delta]-d[0])*p>>14; }
  }
}
void blur3(char *dest, int delta, int p) {
  char *d;
  int size=320*200;

  p>>=2;

  if (delta<0) {
    for (d=dest-delta; d<dest+size;  d++) { d[0]+=(d[delta]-d[0])*p>>14; }
  } else {
    for (d=dest+(size-1)-delta; d>=dest; d--) { d[0]+=(d[delta]-d[0])*p>>14; }
  }
}




void teeharmaastavari(ushort *dest, char *src, char *plasma, int *paletti, int br) {
/*  ushort *dmax=dest+64000;
  if (br==65536) for (; dest<dmax; dest++, src++, plasma++) *dest=paletti[*src+(*plasma<<8)];
    else for (; dest<dmax; dest++, src++, plasma++) *dest=paletti[(*src*br>>16)+(*plasma<<8)];
*/

  ushort *dmax=dest+64000;
  int x, y, s, p;
  if (br==65536) {
    for (y=0; y<200; y+=4, dest+=960, src+=960, plasma+=960) {
      for (dmax=dest+320; dest<dmax; dest+=4, src+=4, plasma+=4) {
        s=((int*)src)[  0]; p=((int*)plasma)[  0];
        ((int*)dest)[  0]=paletti[(s    &255)+(p<<8&0xff00)]+(paletti[(s>>8 &255)+(p    &0xff00)]<<16);
        ((int*)dest)[  1]=paletti[(s>>16&255)+(p>>8&0xff00)]+(paletti[(s>>24&255)+(p>>16&0xff00)]<<16);
        s=((int*)src)[ 80]; p=((int*)plasma)[ 80];
        ((int*)dest)[160]=paletti[(s    &255)+(p<<8&0xff00)]+(paletti[(s>>8 &255)+(p    &0xff00)]<<16);
        ((int*)dest)[161]=paletti[(s>>16&255)+(p>>8&0xff00)]+(paletti[(s>>24&255)+(p>>16&0xff00)]<<16);
        s=((int*)src)[160]; p=((int*)plasma)[160];
        ((int*)dest)[320]=paletti[(s    &255)+(p<<8&0xff00)]+(paletti[(s>>8 &255)+(p    &0xff00)]<<16);
        ((int*)dest)[321]=paletti[(s>>16&255)+(p>>8&0xff00)]+(paletti[(s>>24&255)+(p>>16&0xff00)]<<16);
        s=((int*)src)[240]; p=((int*)plasma)[240];
        ((int*)dest)[480]=paletti[(s    &255)+(p<<8&0xff00)]+(paletti[(s>>8 &255)+(p    &0xff00)]<<16);
        ((int*)dest)[481]=paletti[(s>>16&255)+(p>>8&0xff00)]+(paletti[(s>>24&255)+(p>>16&0xff00)]<<16);
      }
    }
  } else {
    for (; dest<dmax; dest+=4, src+=4, plasma+=4) {
      ((int*)dest)[0]=paletti[(src[0]*br>>16)+(plasma[0]<<8)]+(paletti[(src[1]*br>>16)+(plasma[1]<<8)]<<16);
      ((int*)dest)[1]=paletti[(src[2]*br>>16)+(plasma[2]<<8)]+(paletti[(src[3]*br>>16)+(plasma[3]<<8)]<<16);
    }
  }
}


void teeharmaastavari2(ushort *dest, char *src, char *plasma, int *paletti, int br) {
  ushort *dmax=dest+64000;
  if (br==65536) for (; dest<dmax; dest++, src++, plasma++) *dest=paletti[*src+(*plasma>>2<<8)];
    else for (; dest<dmax; dest++, src++, plasma++) *dest=paletti[(*src*br>>16)+(*plasma>>2<<8)];

}


void inter8x8(char *dest, int x1, int y1, gridfx *g) {
  int x, y, u, v, c, a;
  int u_xinc, u_yinc, u_xyinc;
  int v_xinc, v_yinc, v_xyinc;
  int c_xinc, c_yinc, c_xyinc;
  char *d=dest;

  u_xinc=(g[1].u-g[0].u)>>4;
  u_yinc=(g[41].u-g[0].u)>>3;
  v_xinc=(g[1].v-g[0].v)>>4;
  v_yinc=(g[41].v-g[0].v)>>3;
  c_xinc=(g[1].c-g[0].c)>>4;
  c_yinc=(g[41].c-g[0].c)>>3;

  u_xyinc=g[42].u-g[41].u-g[1].u+g[0].u>>7;
  v_xyinc=g[42].v-g[41].v-g[1].v+g[0].v>>7;
  c_xyinc=g[42].c-g[41].c-g[1].c+g[0].c>>7;
//  u_xyinc=v_xyinc=c_xyinc=0;
  u=g[0].u;
  v=g[0].v;
  c=g[0].c;
  for (y=0; y<8; y++) {
    for (x=0; x<8; x++) {
      //a=mappi2[(v>>16&0xff00)+(u>>24&0xff)];
      a=texture[(v>>mapsy&wrapy)+(u>>mapsx&wrapx)];
      //a=texture[(v>>14&0x3fe00)+(u>>23&0x1ff)];
      u+=u_xinc; v+=v_xinc; c+=c_xinc;
      //*d++=a+mappi2[(v>>16&0xff00)+(u>>24&0xff)]>>1;
      *d++=a+texture[(v>>mapsy&wrapy)+(u>>mapsx&wrapx)]>>1;
      //*d++=a+texture[(v>>14&0x3fe00)+(u>>23&0x1ff)]>>1;
      u+=u_xinc; v+=v_xinc; c+=c_xinc;
    }
    u+=u_yinc-16*u_xinc;
    v+=v_yinc-16*v_xinc;
    c+=c_yinc-16*c_xinc;
    u_xinc+=u_xyinc;
    v_xinc+=v_xyinc;
    c_xinc+=c_xyinc;
    d+=640-8;
  }
}

void fill8x8(char *dest) {
  //int x, y;
  //for (y=0; y<8; y++) for (x=0; x<8; x++)
    //dest[y*640+x]=100;

  //long *d=(long *)dest;
  //d[0]=0;   d[1]=0;
  //d[160]=0; d[161]=0;
  //d[320]=0; d[321]=0;
  //d[480]=0; d[481]=0;
  //d[640]=0; d[641]=0;
  //d[800]=0; d[801]=0;
  //d[960]=0; d[961]=0;
  //d[1080]=0; d[1081]=0;
}


float camera[3][3];
float suunta[3];
float perx, pery;
float posx, posy, posz;


void ammuu_sade(gridfx *g, int x, int y) {
  float px, py, pz, t;
  float ax, bx, cx;
  float test;

  suunta[0]=camera[2][0] - camera[1][0]*(y-200)*pery - camera[0][0]*(x-320)*perx;
  suunta[1]=camera[2][1] - camera[1][1]*(y-200)*pery - camera[0][1]*(x-320)*perx;
  suunta[2]=camera[2][2] - camera[1][2]*(y-200)*pery - camera[0][2]*(x-320)*perx;

  ax=suunta[0]*suunta[0]*opu[0]+
     suunta[1]*suunta[1]*opu[1]+
     suunta[2]*suunta[2]*opu[2];
  bx=suunta[0]*posx*opu[0]+
     suunta[1]*posy*opu[1]+
     suunta[2]*posz*opu[2];
  cx=posx*posx*opu[0]+
     posy*posy*opu[1]+
     posz*posz*opu[2]-opu[3];

  test=bx*bx-ax*cx;
  if (test<0) {
    g[0].c=ulkona;
  } else {
    t=(-fsqrt(test)-bx)/ax;
    px=posx+t*suunta[0];
    py=posy+t*suunta[1];
    pz=posz+t*suunta[2];
    if (opu==pallo) {
      g[0].u=fpatan(px, py)/pi*128.0*16777216.0;
      g[0].v=(int)(fpatan(pz, fsqrt(px*px+py*py))/pi*128.0*16777216.0)<<1;
      g[0].c=0;
    } else if (opu==putki) {
      g[0].u=(int)(fpatan(py, pz)/pi*16777216.0)*768;
      g[0].v=(int)((px+musatimer()/16.0-100)*16777216.0)*64;
      g[0].c=0;
    //} else if (opu==isoputki) {
      //g[0].u=(int)(fpatan(px, py)/pi*16777216.0)*8192;
      //g[0].v=(int)(pz*16777216.0)*64;
      //g[0].c=0;
    } else if (opu==taso) {
      g[0].u=(int)(py*16777216.0)*64;
      g[0].v=(int)(pz*16777216.0)*64;
      g[0].c=0;
    }
    //g[0].c=65536*255-t*65536*64;
    //if (g[0].c<65536) g[0].c=65536;
  }
}



void subdiv1(int x, int y, gridfx *d, gridfx *s1, gridfx *s2) {//gridfx *d, gridfx *s1, gridfx *s2) {
  if (s1->c==ulkona && s2->c==ulkona) d->c=ulkona; else
    if (s1->c!=ulkona && s2->c!=ulkona && (unsigned)(s1->u-s2->u+0x40000000)<0x80000000) {
      d->u=s1->u+(s2->u-s1->u>>1); d->v=s1->v+(s2->v-s1->v>>1); d->c=s1->c+(s2->c-s1->c>>1);
    } else ammuu_sade(d, x, y);
}
void subdiv8x8(char *dest, int x1, int y1, gridfx *g) {
  int x, y;
  static gridfx gtmp[17*17], *gp;

  fill32(gtmp, sizeof(gtmp)>>2, 0);
  gtmp[0]=g[0]; gtmp[16]=g[1]; gtmp[16*17]=g[41]; gtmp[16*17+16]=g[42];

  subdiv1(x1+ 8, y1+0, gtmp+ 8+ 0*17, gtmp+ 0+ 0*17, gtmp+16+ 0*17);
  subdiv1(x1+ 4, y1+0, gtmp+ 4+ 0*17, gtmp+ 0+ 0*17, gtmp+ 8+ 0*17);
  subdiv1(x1+12, y1+0, gtmp+12+ 0*17, gtmp+ 8+ 0*17, gtmp+16+ 0*17);
  subdiv1(x1+ 2, y1+0, gtmp+ 2+ 0*17, gtmp+ 0+ 0*17, gtmp+ 4+ 0*17);
  subdiv1(x1+ 6, y1+0, gtmp+ 6+ 0*17, gtmp+ 4+ 0*17, gtmp+ 8+ 0*17);
  subdiv1(x1+10, y1+0, gtmp+10+ 0*17, gtmp+ 8+ 0*17, gtmp+12+ 0*17);
  subdiv1(x1+14, y1+0, gtmp+14+ 0*17, gtmp+12+ 0*17, gtmp+16+ 0*17);
  subdiv1(x1+ 1, y1+0, gtmp+ 1+ 0*17, gtmp+ 0+ 0*17, gtmp+ 2+ 0*17);
  subdiv1(x1+ 3, y1+0, gtmp+ 3+ 0*17, gtmp+ 2+ 0*17, gtmp+ 4+ 0*17);
  subdiv1(x1+ 5, y1+0, gtmp+ 5+ 0*17, gtmp+ 4+ 0*17, gtmp+ 6+ 0*17);
  subdiv1(x1+ 7, y1+0, gtmp+ 7+ 0*17, gtmp+ 6+ 0*17, gtmp+ 8+ 0*17);
  subdiv1(x1+ 9, y1+0, gtmp+ 9+ 0*17, gtmp+ 8+ 0*17, gtmp+10+ 0*17);
  subdiv1(x1+11, y1+0, gtmp+11+ 0*17, gtmp+10+ 0*17, gtmp+12+ 0*17);
  subdiv1(x1+13, y1+0, gtmp+13+ 0*17, gtmp+12+ 0*17, gtmp+14+ 0*17);
  subdiv1(x1+15, y1+0, gtmp+15+ 0*17, gtmp+14+ 0*17, gtmp+16+ 0*17);
  subdiv1(x1+ 8, y1+16, gtmp+ 8+16*17, gtmp+ 0+16*17, gtmp+16+16*17);
  subdiv1(x1+ 4, y1+16, gtmp+ 4+16*17, gtmp+ 0+16*17, gtmp+ 8+16*17);
  subdiv1(x1+12, y1+16, gtmp+12+16*17, gtmp+ 8+16*17, gtmp+16+16*17);
  subdiv1(x1+ 2, y1+16, gtmp+ 2+16*17, gtmp+ 0+16*17, gtmp+ 4+16*17);
  subdiv1(x1+ 6, y1+16, gtmp+ 6+16*17, gtmp+ 4+16*17, gtmp+ 8+16*17);
  subdiv1(x1+10, y1+16, gtmp+10+16*17, gtmp+ 8+16*17, gtmp+12+16*17);
  subdiv1(x1+14, y1+16, gtmp+14+16*17, gtmp+12+16*17, gtmp+16+16*17);
  subdiv1(x1+ 1, y1+16, gtmp+ 1+16*17, gtmp+ 0+16*17, gtmp+ 2+16*17);
  subdiv1(x1+ 3, y1+16, gtmp+ 3+16*17, gtmp+ 2+16*17, gtmp+ 4+16*17);
  subdiv1(x1+ 5, y1+16, gtmp+ 5+16*17, gtmp+ 4+16*17, gtmp+ 6+16*17);
  subdiv1(x1+ 7, y1+16, gtmp+ 7+16*17, gtmp+ 6+16*17, gtmp+ 8+16*17);
  subdiv1(x1+ 9, y1+16, gtmp+ 9+16*17, gtmp+ 8+16*17, gtmp+10+16*17);
  subdiv1(x1+11, y1+16, gtmp+11+16*17, gtmp+10+16*17, gtmp+12+16*17);
  subdiv1(x1+13, y1+16, gtmp+13+16*17, gtmp+12+16*17, gtmp+14+16*17);
  subdiv1(x1+15, y1+16, gtmp+15+16*17, gtmp+14+16*17, gtmp+16+16*17);
  for (x=0; x<16; x++) {
    subdiv1(x1+x, y1+ 8, gtmp+x+ 8*17, gtmp+x+ 0*17, gtmp+x+16*17);
    subdiv1(x1+x, y1+ 4, gtmp+x+ 4*17, gtmp+x+ 0*17, gtmp+x+ 8*17);
    subdiv1(x1+x, y1+12, gtmp+x+12*17, gtmp+x+ 8*17, gtmp+x+16*17);
    subdiv1(x1+x, y1+ 2, gtmp+x+ 2*17, gtmp+x+ 0*17, gtmp+x+ 4*17);
    subdiv1(x1+x, y1+ 6, gtmp+x+ 6*17, gtmp+x+ 4*17, gtmp+x+ 8*17);
    subdiv1(x1+x, y1+10, gtmp+x+10*17, gtmp+x+ 8*17, gtmp+x+12*17);
    subdiv1(x1+x, y1+14, gtmp+x+14*17, gtmp+x+12*17, gtmp+x+16*17);
    subdiv1(x1+x, y1+ 1, gtmp+x+ 1*17, gtmp+x+ 0*17, gtmp+x+ 2*17);
    subdiv1(x1+x, y1+ 3, gtmp+x+ 3*17, gtmp+x+ 2*17, gtmp+x+ 4*17);
    subdiv1(x1+x, y1+ 5, gtmp+x+ 5*17, gtmp+x+ 4*17, gtmp+x+ 6*17);
    subdiv1(x1+x, y1+ 7, gtmp+x+ 7*17, gtmp+x+ 6*17, gtmp+x+ 8*17);
    subdiv1(x1+x, y1+ 9, gtmp+x+ 9*17, gtmp+x+ 8*17, gtmp+x+10*17);
    subdiv1(x1+x, y1+11, gtmp+x+11*17, gtmp+x+10*17, gtmp+x+12*17);
    subdiv1(x1+x, y1+13, gtmp+x+13*17, gtmp+x+12*17, gtmp+x+14*17);
    subdiv1(x1+x, y1+15, gtmp+x+15*17, gtmp+x+14*17, gtmp+x+16*17);
  }

  for (gp=gtmp, y=0; y<16; y++, gp++) for (x=0; x<16; x++, gp++)
    gp->c=gp->c==ulkona?0:texture[(gp->v>>mapsy&wrapy)+(gp->u>>mapsx&wrapx)];
/*  for (gp=gtmp, y=0; y<8; y++, gp++) for (x=0; x<8; x++, gp++) {
    if (gp->c==ulkona) dest[y*320+x]=0; else
//      dest[(y+y1)*320+x+x1]=rtbl[(mappi2[(gp->u>>16&0xff00)+(gp->v>>24&0xff)]*gp->c>>23)+1024];
      dest[y*320+x]=sperikal2[(gp->v>>16&0xff00)+(gp->u>>24&0xff)];
  }*/
  for (gp=gtmp, y=0; y<8; y++, gp+=18) {
    char *d=dest+y*640;
    for (x=0; x<8; x++, gp+=2) *d++=gp[0].c+gp[1].c+gp[17].c+gp[18].c+2>>2;
  }

}



dot raycokes(char *dest, float _posx, float _posy, float _posz,
             float tgtx, float tgty, float tgtz, float jep,
             float xs, float ys, float zs) {
  float px, py, pz, t;
  int x, y, u, v;
  float ax, bx, cx;
  int xypos;
  float r, test;
  gridfx *p;
  dot joops;
  float xxx, yyy, zzz;

  posx=_posx; posy=_posy; posz=_posz;
  teematriisi2(camera, tgtx-posx, tgty-posy, tgtz-posz);
  pery=1/300.0; perx=1/300.0;

  if (texture==&mappi1) mapsy=14, mapsx=23, wrapy=0x3fe00, wrapx=0x1ff;
  if (texture==&mappi2) mapsy=16, mapsx=23, wrapy=0x3f80, wrapx=0x7f;
  //if (texture==&mappi2) mapsy=15, mapsx=23, wrapy=0xff00, wrapx=0xff;
  if (texture==&mappi3) mapsy=16, mapsx=25, wrapy=0x3f80, wrapx=0x7f;
  if (texture==&mappi4) mapsy=16, mapsx=25, wrapy=0x3f80, wrapx=0x7f;


  xxx=camera[0][0]*(xs-posx)+camera[0][1]*(ys-posy)+camera[0][2]*(zs-posz);
  yyy=camera[1][0]*(xs-posx)+camera[1][1]*(ys-posy)+camera[1][2]*(zs-posz);
  zzz=camera[2][0]*(xs-posx)+camera[2][1]*(ys-posy)+camera[2][2]*(zs-posz);

  joops.x=(xxx/zzz*100*1.8)*jep;
  joops.y=(yyy/zzz*100*1.8)*jep;
  if (joops.x<-159) joops.x=-159; if (joops.x>159) joops.x=159;
  if (joops.y<-99) joops.y=-99; if (joops.y>99) joops.y=99;
  dest=dest+joops.y*640+joops.x;

  for (y=0; y<26; y++) for (x=0; x<41; x++) ammuu_sade(pallero+y*41+x, x*16, y*16);

  for (y=0; y<25; y++)
    for (x=0; x<40; x++) {
      char *d2=dest+y*640*8+x*8;
      xypos=y*41+x; p=pallero+xypos;
      if (p[0].c!=ulkona && p[1].c!=ulkona && p[41].c!=ulkona && p[42].c!=ulkona
         && (unsigned)(p[0].u-p[42].u+0x40000000)<0x80000000 && (unsigned)(p[1].u-p[41].u+0x40000000)<0x80000000)
        inter8x8(d2, x<<4, y<<4, pallero+xypos);
      else if (p[0].c==ulkona && p[1].c==ulkona && p[41].c==ulkona && p[42].c==ulkona)
        fill8x8(d2);
      else subdiv8x8(d2, x<<4, y<<4, pallero+xypos);
  }
  return joops;
}


void plussprite(int x1, int y1, int x2, int y2, int leveys, int korkeus,
                schar *source, char *dest, int br) {
  int y, x1b, x2b, y1b, y2b, i, u16, u16_2, u16inc, v16, v16inc;
  schar *s2;
  char *d2, *_d2, *d2m, *d2m2;

  if (x1<0)       x1b=0;   else x1b=x1+255>>8;
  if (x2>320*256) x2b=320; else x2b=x2+255>>8;
  if (x1b>=x2b) return;
  if (y1<0)       y1b=0;   else y1b=y1+255>>8;
  if (y2>200*256) y2b=200; else y2b=y2+255>>8;
  if (y1b>=y2b) return;
  u16inc=16777216.0*leveys/(x2-x1);
  u16_2=((x1b<<8)-x1)*u16inc>>8;
  v16inc=16777216.0*korkeus/(y2-y1);
  v16=((y1b<<8)-y1)*v16inc>>8;
  _d2=dest+y1b*320;
  for (y=y1b; y<y2b; y++) {
    s2=source+(v16>>16)*leveys;
    d2=_d2; d2m=d2+x2b; d2+=x1b; d2m2=d2m-4; u16=u16_2;
    for (; d2<d2m2; d2+=4) {
      i=d2[0]+(s2[u16>>16]*br>>16); if (i>255) i=255; if (i<0) i=0; d2[0]=i; u16+=u16inc;
      i=d2[1]+(s2[u16>>16]*br>>16); if (i>255) i=255; if (i<0) i=0; d2[1]=i; u16+=u16inc;
      i=d2[2]+(s2[u16>>16]*br>>16); if (i>255) i=255; if (i<0) i=0; d2[2]=i; u16+=u16inc;
      i=d2[3]+(s2[u16>>16]*br>>16); if (i>255) i=255; if (i<0) i=0; d2[3]=i; u16+=u16inc;
    }
    for (; d2<d2m; d2++, u16+=u16inc) {
      i=*d2+(s2[u16>>16]*br>>16); //*d2=i|(i&256)-((i&256)>>8);
      if (i>255) i=255; if (i<0) i=0; *d2=i;
    }
    v16+=v16inc; _d2+=320;
  }
}



void teepistevektori(char *dest, int *datx, int *daty, int *datz, int dots2,
                     float posx, float posy, float posz, schar *s) {
  float camera[3][3];
  int uusix[valom], uusiy[valom], uusiz[valom];
  int x, y, z, i, x1, y1, x2, y2;
  int px, py, pz;
  int br;

  px=posx*16384;
  py=posy*16384;
  pz=posz*16384;

  teematriisi2(camera, 0-posx, 0-posy, 0-posz);

  for (i=0; i<dots2; i++) {
    uusix[i]=camera[0][0]*(datx[i]-px) + camera[0][1]*(daty[i]-py) + camera[0][2]*(datz[i]-pz);
    uusiy[i]=camera[1][0]*(datx[i]-px) + camera[1][1]*(daty[i]-py) + camera[1][2]*(datz[i]-pz);
    uusiz[i]=camera[2][0]*(datx[i]-px) + camera[2][1]*(daty[i]-py) + camera[2][2]*(datz[i]-pz);
    uusiz[i]*=5.0;
  }

  for (i=0; i<dots2; i++) {
    x=32768.0*4*uusix[i]/(uusiz[i]+0.001);//+(rand()-rand())*0.01;
    y=32768.0*4*uusiy[i]/(uusiz[i]+0.001);//+(rand()-rand())*0.01;
    z=32000/(uusiz[i]+0.001)*10000;
    br=65536/(uusiz[i]*0.00002);
    if (br>65536) br=65536;
    x1=x-z;
    y1=y-z;
    x2=x+z;
    y2=y+z;
    if (uusiz[i]>0) plussprite(x1+160*256 ,y1+100*256 ,x2+160*256, y2+100*256, 64, 64, s, dest, br);
  }
}

void plordbuf() {
  int x,y,i;
  for (y=ymov; y<ymov+199; y++) for (x=xmov; x<xmov+319; x++) {
    i=y*640+x;
    dbuf[y*640+x]=dbuf[i]+dbuf[i+1]+dbuf[i+640]+dbuf[i+641]>>2;
  }
}
void plorbuf(char *dest) {
  int i;
  for (i=0; i<64000; i++) dest[i]=dest[i]+dest[i+1]+dest[i+320]+dest[i+321]>>2;
}

void teesivupalkki(char *dest, int w, int s) {
  int x,y,xx,yy,i,j,br;

  for (y=0; y<20; y++) for (x=0; x<5; x++) {
    i=(y*10+ymov)*w+x*10+xmov+s;
    for (yy=0; yy<10; yy++, i+=w-10) for (xx=0; xx<10; xx++, i++) {
      j=rtbl[((block2[yy*10+xx]*sivupee[y*5+x]>>8)-127)*5+127];
      dest[i]=rtbl[dest[i]+j];
    }
  }

}

void zoomaa(char *sourke, char *dest, int w, int h) {
  int x1,y1,x2,y2;
  int x,y,u,v;
  char *s, *d;

/*  x1=w/2-w/4;
  y1=h/2-h/4;
  x2=w/2+w/4;
  y2=h/2+h/4; */

  x1=160-80;
  y1=100-50;
  x2=160+80;
  y2=100+50;

/*  for (y=y1, v=0; y<y2; y++, v+=2) for (x=x1, u=0; x<x2; x++, u+=2) {
     dest[v*320+u    ]=sourke[y*320+x]+1>>1;
     dest[v*320+u+1  ]=sourke[y*320+x]+sourke[y*320+x+1]+2>>2;
     dest[v*320+u+320]=sourke[y*320+x]+sourke[y*320+x+320]+2>>2;
     dest[v*320+u+321]=sourke[y*320+x]+sourke[y*320+x+1]+sourke[y*320+x+320]+sourke[y*320+x+321]+4>>3;
  }*/
  for (y=y1, v=0; y<y2; y++, v+=2) {
    d=dest+v*320; s=sourke+y*320+x1;
    for (x=x1; x<x2; x++, d+=2, s++) {
      d[0]=s[0]+1>>2;
      d[1]=rtbl[(s[0]+s[1])*5-(s[-1]+s[2])>>5];
    }
  }

  for (v=1; v<199; v+=2) {
    d=dest+v*320;
    if (v==1 || v==197)
      for (u=0; u<320; u++, d++) d[0]=d[-320]+d[320]+1>>1;
    else
      for (u=0; u<320; u++, d++) d[0]=rtbl[(d[-320]+d[320])*5-(d[-960]+d[960])>>3];

   for (u=0; u<320; u++) dest[199*320+u]=0;
  }
/*  for (y=y1, v=0; y<y2; y++, v+=2) {
    d=dest+v*320; s=sourke+y*320+x1;
    for (x=x1; x<x2; x++, d+=2, s++) {
      d[0]=s[0]+1>>1;
      d[1]=(s[0]+s[1])*5-(s[-1]+s[2])+8>>4;
    }
  }
  for (v=1; v<199; v+=2) {
    d=dest+v*320;
    if (v==1 || v==197)
      for (u=0; u<320; u++, d++) d[0]=d[-320]+d[320]+1>>1;
    else
      for (u=0; u<320; u++, d++) d[0]=(d[-320]+d[320])*5-(d[-960]+d[960])+4>>3;
  }*/
}



//////////////////////////////////////////////////// EFEKTIEN PY™RITTŽJŽT

void efekti1(float t) {
  dot joohei;
  int x, y, i, br, pos;

  opu=pallo;
  texture=mappi1;

  fill32(dbuf, 64000, 0);
  joohei=raycokes(dbuf+64160,
         fsin(t*3.37442)*2.1, fcos(t*3.371442)*2.7, fsin(t*5.011354)*2.4,
         fsin(t*2.73382)*0.7, fsin(t*1.123782)*0.6, fsin(t*3.114635)*0.5,
         1, 0, 0, 0);

  xmov=joohei.x+160;
  ymov=joohei.y+100;

  plordbuf();
  teeradiaali(dbuf, valotbl);
  blur2(valotbl, -512, 63000);
  blur2(valotbl, 1, 35000);
  blur2(valotbl, -1, 35000);


  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    i=(y+ymov)*xres*2+x+xmov;
    dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]+(dbuf[i]>>1)];
    //dbuf2[y*320+x]=dbuf[y*2*640+x*2];
    //dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[(y+ymov)*xres*2+x+xmov]]>>4]];
    //dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]];
  }
  br=65536;
  if (t<31.25) br=(t-31)*262144;
  if (t>=36.75) br=65536-(t-36.75)*262144;
  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf2, hue, paletti1, br);
}


void efekti2(float t) {
  dot joohei;
  int x, y, i, br, pos;
  float xx, yy, zz;

  xx=fsin(t*1.55232)*2.0;
  yy=fsin(t*1.92344)*4.0;
  zz=fcos(t*1.92344)*4.0;

  fill32(dbuf, 64000, 0);

  opu=putki;
  texture=mappi3;
  joohei=raycokes(dbuf+64160, xx, yy, zz, 0, 0, 0, 1, 0, 0, 0);
  xmov=joohei.x+160;
  ymov=joohei.y+100;
  plordbuf();
  teeradiaali(dbuf, valotbl);
  blur2(valotbl, -512, 61000);
  blur2(valotbl, 1, pou2(t, 1.5));
  blur2(valotbl, -1, pou2(t, 1.5));

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    i=(y+ymov)*xres*2+x+xmov;
    //dbuf2[y*320+x]=dbuf3[y*320+x];
    dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]+(dbuf[i]>>1)];
    //dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]+dbuf3[y*320+x]+dbuf2[y*320+x]];
  }

  for (y=0; y<20; y++) for (x=0; x<5; x++) {
    //if ((int)(y+t*40)%3==0) sivupee[y*5+x]=128;
    //if ((int)(y+t*40)%3==1) sivupee[y*5+x]=64;
    //if ((int)(y+t*40)%3==2) sivupee[y*5+x]=0;
    sivupee[y*5+x]=fabs(fsin(y*0.3-t*6))*63+64;
  }
  xmov=ymov=0;
  teesivupalkki(dbuf2, 320, 260);
  br=65536;
  if (t<25.25) br=(t-25)*262144;
  if (t>=30.75) br=65536-(t-30.75)*262144;

  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf2, hue, paletti5, br);
}

void efekti3(float t) {
  dot joohei;
  int x, y, i, br, pos;

  opu=pallo;
  texture=mappi1;

  fill32(dbuf, 64000, 0);
  joohei=raycokes(dbuf+64160,
         fsin(t*5.37442)*2.1, fcos(t*5.371442)*2.7, fsin(t*3.011354)*2.4,
         fsin(t*2.73382)*0.4, fsin(t*4.123782)*0.4, fsin(t*3.114635)*0.4,
         1, 0, 0, 0);

  xmov=joohei.x+160;
  ymov=joohei.y+100;

  plordbuf();
  teeradiaali(dbuf, valotbl);
  blur2(valotbl, 512, 63000);
  blur2(valotbl, 1, 30000);
  blur2(valotbl, -1, 30000);

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    i=(y+ymov)*xres*2+x+xmov;
    dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]+(dbuf[i]+1>>1)];
    //dbuf2[y*320+x]=dbuf[i];
    //dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[(y+ymov)*xres*2+x+xmov]]>>4]];
    //dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]];
  }



  zoomaa(dbuf2, dbuf3, 160, 100);

  if (t>=37.0 && t<37.5) gfxputs(dbuf3, fun, 20, 180, "harlequin greets",   320, 2);
  if (t>=37.5 && t<38.0) gfxputs(dbuf3, fun, 20, 180, "coral",              320, 2);
  if (t>=38.0 && t<38.5) gfxputs(dbuf3, fun, 20, 180, "council 4",          320, 2);
  if (t>=38.5 && t<39.0) gfxputs(dbuf3, fun, 20, 180, "mfx",                320, 2);
  if (t>=39.0 && t<39.5) gfxputs(dbuf3, fun, 20, 180, "orange",             320, 2);
  if (t>=39.5 && t<40.0) gfxputs(dbuf3, fun, 20, 180, "pwp",                320, 2);
  if (t>=40.0 && t<40.5) gfxputs(dbuf3, fun, 20, 180, "tpolm",              320, 2);
  if (t>=40.5 && t<41.0) gfxputs(dbuf3, fun, 20, 180, "coma",               320, 2);
  if (t>=41.0 && t<41.5) gfxputs(dbuf3, fun, 20, 180, "laite",              320, 2);
  if (t>=41.5 && t<42.0) gfxputs(dbuf3, fun, 20, 180, "halcyon",            320, 2);
  if (t>=42.0 && t<42.5) gfxputs(dbuf3, fun, 20, 180, "elukka",             320, 2);
  if (t>=42.5 && t<43.0) gfxputs(dbuf3, fun, 20, 180, "sukka",              320, 2);
  if (t>=43.0 && t<43.5) gfxputs(dbuf3, fun, 20, 180, "and his associates", 320, 2);
  if (t>=43.5 && t<44.0) gfxputs(dbuf3, fun, 20, 180, "robot and clone",    320, 2);

  xmov=ymov=0;
  for (y=0; y<20; y++) for (x=0; x<5; x++)
    sivupee[y*5+x]=(rand()&63)+64;
  teesivupalkki(dbuf3, 320, 260);


  blur3(dbuf3, 1,  fcos(t*pi*4)*32767+32768);
  blur3(dbuf3, -1, fcos(t*pi*4)*32767+32768);

  for (x=0; x<64000; x++) dbuf3[x]=rtbl[dbuf3[x]+dbuf2[x]];

  //pos=(int)(t*65536)&32767; pos=65536-pos*4; if (pos<0) pos=0;
  //if (pos>0) for (x=0; x<64000; x++)
    //dbuf2[x]=((255-dbuf2[x])*pos>>16)+(dbuf2[x]*(65536-pos)>>16);

  br=65536;
  if (t<37.25) br=(t-37)*262144;
  if (t>43.75) for (x=0; x<64000; x++) dbuf3[x]=rtbl[dbuf3[x]+(int)((t-43.75)*1024)];
  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf3, hue, paletti6, br);
}


void alkutekstit(float t) {
  int x, y, xmov, ymov, i;

  //if ((int)t%2==0) ymov=fsin(t*2.0*pi+pi/2.0)*50+100;
  //else ymov=-fsin(t*2.0*pi+pi/2.0)*50+100;
  //xmov=fsin(t*4.0*pi)*100+160;
  if (t<11) ymov=fsin(t*4.0*pi)*50+100;
  else ymov=fsin(t*4.0*pi)*20+100;
  xmov=fsin(t*3.0*pi)*30+160;

  fill32(dbuf, 64000, 0);
  if (t>=9 && t<11) {
    gfxputs(dbuf, fun, 122+xmov, 40+ymov,  "rainmaker",  640, 4);
    gfxputs(dbuf, fun, 148+xmov, 70+ymov,  "216",        640, 4);
    gfxputs(dbuf, fun, 113+xmov, 100+ymov, "honeybunny", 640, 4);
    gfxputs(dbuf, fun, 132+xmov, 130+ymov, "uncle x",    640, 4);
    gfxputs(dbuf, fun, 123+xmov, 160+ymov, "synteesi",   640, 4);
  }
  if (t>=11 && t<13) for (y=0; y<50; y++) for (x=0; x<320; x++) {
    dbuf[(y+ymov+75)*640+x+xmov]=logo1[y*320+x];
  }
  if (t>=13 && t<15) for (y=0; y<50; y++) for (x=0; x<320; x++) {
    dbuf[(y+ymov+75)*640+x+xmov]=logo2[y*320+x];
  }
  //gfxputs(dbuf, fun, 120+xmov, 100+ymov, "mewlers", 640, 8);
  //if (t>=13 && t<15) gfxputs(dbuf, fun, 120+xmov, 100+ymov, "harlequin", 640, 8);

  teeradiaali(dbuf, valotbl);
  blur2(valotbl, -512, 63000);
  blur2(valotbl, -1, 40000);
  blur2(valotbl, 1, 40000);

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
   i=(y+ymov)*xres*2+x+xmov;
   dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]+(dbuf[i]>>2)];
  }

}

void tasoefekti(float t) {
  dot joohei;
  int x, y, i, br, pos;
  float xx, yy;
  float kerroin, korkeus;
  static char korkeus2[]={2, 2, 2, 2, 2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 2};

  if (t>=9 && t<15 && mdata->currow>=16 && mdata->currow<48) alkutekstit(t);
  else {
    if (t>=15) kerroin=1-(t-15), korkeus=2-(t-15)*0.5;
      else kerroin=1, korkeus=korkeus2[(int)t];
    if (t<1) kerroin=t;
    if (kerroin<0) kerroin=0;
    xx=(fsin(t*1.9273213)+fsin(t*2.421212)*0.5)*kerroin*1.5;
    yy=(fsin(t*2.5121212)+fsin(t*3.123128)*0.5)*kerroin*1.5;

    opu=taso;
    texture=mappi2;

    fill32(dbuf, 64000, 0);
    joohei=raycokes(dbuf+64160, korkeus, xx, yy, 0, xx*0.5, yy*0.5,
                    -1, 0, 0, 0);

    xmov=(joohei.x+160);
    ymov=(joohei.y+100);
    plordbuf();
    if (t>4) {
      teeradiaali(dbuf, valotbl);
      if (t<15) blur2(valotbl, -512, pou1(t, 1.0));
      else blur2(valotbl, -512, 63000);
      blur2(valotbl, 1, 30000);
      blur2(valotbl, -1, 30000);
      for (y=0; y<200; y++) for (x=0; x<320; x++) {
        i=(y+ymov)*xres*2+x+xmov;
        dbuf2[y*320+x]=rtbl[tbl[valotbl[tunneli[i]]>>4]+(dbuf[i]>>1)];
      }
    } else {
      for (y=0; y<200; y++) for (x=0; x<320; x++) {
        i=(y+ymov)*xres*2+x+xmov;
        dbuf2[y*320+x]=dbuf[i];
      }
    }
  }
  if (t<4 || (t>=9 && t<15 && mdata->currow>=16 && mdata->currow<48)) {
    br=nakutus[mdata->currow]*8192;
  } else br=65536;
  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf2, hue, paletti4, br);
}


int itse(int joo) {
  if (joo<0) return -joo; else return joo;
}

void flaret1(float t) {
  float t2;
  int x,y,i,br;

  t2=t*0.5+((int)(t)^255);

  fill32(dbuf, 16000, 0x7f7f7f7f);
  teepistevektori(dbuf, valot_x, valot_y, valot_z, valom,
                  fsin(t2*2.642)*1.3, fcos(t2*1.921)*1.5, fsin(t2*3.121)*1.2,
                  sprite1);

  for (x=0; x<64000; x++) dbuf2[x]=itse(127-dbuf[x])>>1;

  //for (i=(50-32)*320+280-32, y=0; y<64; y++) for (x=0; x<64; x++)
    //dbuf[y*320+x+i]=rtbl[dbuf[y*320+x+i]+(block[y*64+x]*nakunaku1(t, 6.0)>>17)];
  //for (i=(150-32)*320+280-32, y=0; y<64; y++) for (x=0; x<64; x++)
    //dbuf[y*320+x+i]=rtbl[dbuf[y*320+x+i]+(block[y*64+x]*nakunaku2(t, 6.0)>>17)];
  //if (t>=19) for (i=(100-32)*320+280-32, y=0; y<64; y++) for (x=0; x<64; x++)
    //dbuf[y*320+x+i]=rtbl[dbuf[y*320+x+i]+(block[y*64+x]*(nakutus[mdata->currow]*4096+4096)>>17)];

  //for (i=(50-32)*320+40-32, y=0; y<64; y++) for (x=0; x<64; x++)
    //dbuf[y*320+x+i]=rtbl[dbuf[y*320+x+i]-(block[y*64+x]*nakunaku1(t, 6.0)>>17)];
  //for (i=(150-32)*320+40-32, y=0; y<64; y++) for (x=0; x<64; x++)
    //dbuf[y*320+x+i]=rtbl[dbuf[y*320+x+i]-(block[y*64+x]*nakunaku2(t, 6.0)>>17)];
  //if (t>=19) for (i=(100-32)*320+40-32, y=0; y<64; y++) for (x=0; x<64; x++)
    //dbuf[y*320+x+i]=rtbl[dbuf[y*320+x+i]-(block[y*64+x]*(nakutus[mdata->currow]*4096+4096)>>17)];

  xmov=ymov=0;
  for (y=0; y<20; y++) for (x=0; x<5; x++) {
    //sivupee[y*5+x]=fsin((y*0.2+t)*pi)*63+64;
    sivupee[y*5+x]=rand()&127;
  }
  teesivupalkki(dbuf, 320, 260);
  if (t<17.5) for (x=0; x<64000; x++) dbuf[x]=rtbl[dbuf[x]+(int)(255-(t-17)*512)];
  br=65536;
  if (t>=24.75) br=65536-(t-24.75)*262144;

  if (t>=17 && t<19) gfxputs(dbuf, fun, 20, 180, "float  harlequin  float",    320, 1);
  if (t>=19 && t<21) gfxputs(dbuf, fun, 20, 180, "and  the  harlequin  floats", 320, 1);
  if (t>=21 && t<23) gfxputs(dbuf, fun, 20, 180, "let  it  float",             320, 1);
  if (t>=23 && t<25) gfxputs(dbuf, fun, 20, 180, "it  floats  back  to  you",    320, 1);


  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf, dbuf2, paletti2, br);

}
void flaret2(float t) {
  float t2;
  int x, y, i;
  int jaa,joo;
  int br;

  jaa=(int)(t*65536)&65535; joo=0;
  if (t<49) {
    if ((jaa&32767)>=0    && (jaa&32767)<2048) joo=4;
    if ((jaa&32767)>=2048 && (jaa&32767)<4096) joo=2;
    if ((jaa&32767)>=4096 && (jaa&32767)<8192) joo=3;
    if ( jaa>=10240       &&  jaa<16384)       joo=1;
    if ( jaa>=52*1024     &&  jaa<54*1024)     joo=1;
    if ( jaa>=58*1025     &&  jaa<60*1024)     joo=1;
  } else {
    if ((jaa&32767)>=4096  && (jaa&32767)<6*1024)  joo=1;
    if ((jaa&32767)>=10240 && (jaa&32767)<12*1024) joo=2;
    if ((jaa&32767)>=20480 && (jaa&32767)<22*1024) joo=3;
    if ((jaa&32767)>=26*1024 && (jaa&32767)<28*1024) joo=4;
  }

  t2=t*0.5+((int)(t)^255);

  fill32(dbuf, 16000, 0x77777777);
  teepistevektori(dbuf, valot_x, valot_y, valot_z, valom,
                  fsin(t2*2.642)*1.3, fcos(t2*1.921)*1.5, fsin(t2*3.121)*1.2,
                  sprite2);

  for (x=0; x<64000; x++) dbuf2[x]=itse(0x77-dbuf[x])>>1;

  i=(80+(joo-1)*32)+(100-32)*320;
  if (joo!=0) for (y=0; y<64; y++) for (x=0; x<64; x++)
    dbuf[y*320+x+i]=((255-dbuf[y*320+x+i])*block[y*64+x]>>8)+
                    (dbuf[y*320+x+i]*(255-block[y*64+x])>>8);

  if (t<44.5) for (x=0; x<64000; x++) dbuf[x]=rtbl[dbuf[x]+(int)(255-(t-44)*512)];
  if (t>=49) br=65535-(t-49)*65536; else br=65536;

  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf, dbuf2, paletti3, br);

}

void katomappi(char *s2) {
  int x,y,i;
  char *s;

  for (y=0; y<129; y++) for (x=0; x<129; x++) {
    s=s2+y*4*512+x*4;
    if (y>128 || x>128) dbuf[y*320+x]=255;
    else dbuf[y*320+x]=(s[0]+s[1]+s[2]+s[3]+
                        s[512]+s[513]+s[514]+s[515]+
                        s[1024]+s[1025]+s[1026]+s[1027]+
                        s[1536]+s[1537]+s[1538]+s[1539])>>4;
  }
  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf, hue, paletti1, 65536);
}
void katomappi2(char *s) {
  int x,y,i;

  for (y=0; y<129; y++) for (x=0; x<129; x++) {
    if (y>128 || x>128) dbuf[y*320+x]=255;
    else dbuf[y*320+x]=s[y*128+x];
  }
  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf, hue, paletti1, 65536);
}
void katomappi3(char *s) {
  int x,y,i;

  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    dbuf[y*320+x]=s[y*256*2+x*2]+
                  s[y*256*2+x*2+1]+
                  s[y*256*2+x*2+256]+
                  s[y*256*2+x*2+257]>>2;
  }
  teeharmaastavari((unsigned short*)videomem->buf+20*320, dbuf, hue, paletti1, 65536);
}


//////////////////////////////////////////////////// LE MAIN LOOP


int main() {
  int port, mode;
  int x, y, u, v, i, c1, c2, c3;
  float t;
  Video *vid;

  xres=320;
  yres=200;
  valoxres=512;
  valoyres=128;
  xmov=ymov=0;

  mark();

  fun=getfont_();

  dbuf=getmem(xres*yres*4);
  tunneli=getmem(xres*yres*4*4);
  valotbl=getmem(valoxres*valoyres*4);
  radiaali=getmem(valoxres*valoyres*4);

  for (i=0; i<1024; i++) {
    float f=i/128.0; f=f<1?f:1; f=fsqrt(f);
    tbl [i]=fpow(i/1024.0+0.0001, fpow( 0.80,f+0.0001))*512.0;
  }
  for (i=0; i<256; i++) {
    float f=i/128.0; f=f<1?f:1; f=fsqrt(f);
    tbl2[i]=fpow(i/256.0 +0.0001, fpow( 2.00,f+0.0001))*16384.0;
  }


  dosputs("\033[2J$");
  //dosputs("\033[33m$");
  dosputs("--------------------------------------\r\n$");
  dosputs("Harlequin by Mewlers      a 64kb intro\r\n$");
  dosputs("--------------------------------------\r\n$");
  dosputs("Where is your GUS [2x0]? (enter=quiet)\r\n$");
  dosputs("--------------------------------------\r\n$");
    for (port=0x1234; port==0x1234; ) {
    switch (tiny_inkey()) {
      case '0': port=0x200; break;
      case '1': port=0x210; break;
      case '2': port=0x220; break;
      case '3': port=0x230; break;
      case '4': port=0x240; break;
      case '5': port=0x250; break;
      case '6': port=0x260; break;
      case '7': port=0x270; break;
      case 13: port=0; break;
    }
  }
  xmpInit(mjuzic+16384, port|0x80000000, mjuzic, 65536);

  dosputs("Precalculating...$");

  for (x=0; x<8192; x++) rtbl2[x]=0;
  for (x=8192; x<8192+256; x++) rtbl2[x]=x-8192;
  for (x=8192+256; x<16384; x++) rtbl2[x]=255;

  precalcipaskaa();
  prekalkutsjon();
  precalculatemappi();
  precalciflare();

  vid=new_vid_screen(320, 240, 16);
  oldk=get_intr(0x9);
  set_intr(0x9, newk);

  xmpPlay(0);

  while (!kcnt[1] && t<50) {
    videomem=vid_openlfb(vid);
    t=musatimer()/64.0;

    if (t<17) tasoefekti(t);
    if (t>=17 && t<25) flaret1(t);
    if (t>=25 && t<31) efekti2(t);
    if (t>=31 && t<37) efekti1(t);
    if (t>=37 && t<44) efekti3(t);
    if (t>=44 && t<50) flaret2(t);

    //katomappi2(mappi2);


    vid_closelfb(vid);

    while (kcnt[129]&1) ;
    //if (kcnt[0x4e]) { mdata->jumptoord=mdata->curord+1; kcnt[0x4e]=0; }
  }

  set_intr(0x9, oldk);
  xmpStop();
  vid_close(vid);
  release();
  dosputs("\033[37mNow it's safe to turn off your computer.\r\n$");
  return 0;
}
