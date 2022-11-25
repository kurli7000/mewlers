typedef unsigned short ushort;

#include <lib2\fasm.h>
#include <lib2\asm.h>
#include <lib2\mem.h>
#include "rxm.h"

int dflags = df16bit | dfStereo;
int vol = 12*8;
int amp = 0;
int mixrate = 44100;
int iwflag = 0;
int mastervol = 0;
int maxpat = 0;
int rxmconvert = 0;
extern char musiko[];
extern int maincount;
extern short *xtal;

char *videomem=(char *)0xa0000;

int rtbl2[16384];
#define rtbl (rtbl2+8192)

char dbuf[64000], dbuf2[64000];
int ibuf[21600];

int valotbl[65536];
ushort radiaali[65536];
ushort tunneli[64000];
int tbla1[1024], tbla2[256];

/*
signed short schar[64000];
typedef struct {
  int x1, x2;
  char y1, y2;
  int br;
  int vauhti;
} laatikko;
laatikko poksit[1000];
*/
char fisu[7500];
char fish1[15000], fish2[15000];

typedef struct {
  float x, y, z;
  float rx, ry, rz;
  int px, py, pz;
} vector;

typedef struct {
  vector *piste1;
  vector *piste2;
} wire;

typedef struct {
  vector *pisteet;
  wire *langat;
  int vc, wc;
} opu;

int ffloor(float x) { return x-fprem(x,1); }

opu pyra;
wire pyralan[8];
vector pyrapis[5];

opu pyra1;
wire pyralan1[8];
vector pyrapis1[5];

opu pyra2;
wire pyralan2[8];
vector pyrapis2[5];

opu poksi;
wire langat1[12];
vector pisteet1[8];

opu poksi2;
wire langat3[12];
vector pisteet3[8];

opu matto;
wire langat2[19*5];
vector pisteet2[20*5];

opu taso;
wire tasowire[10*10*2];
vector tasopnt[10*10];


//////////////////////////////////////////////////// PASKAA

void exit();

int rand() { static int seed=0x12345679; return (unsigned)(seed=seed*0x97654321+1)>>17; }
int ia(int blah) { if (blah<0) return -blah; else return blah; }
float iaf(float blah) { if (blah<0) return -blah; else return blah; }
void mode(int);
#pragma aux mode="int 0x10" parm[eax] modify[edx];
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
    dosputs("Hmm... Jaahas.\r\n$");
    exit(1);
  }
}

//////////////////////////////////////////////////// PRECALCIT

void precalcoput() {
  int x, y;

  taso.pisteet=tasopnt;
  taso.langat=tasowire;
  taso.vc=10*10;
  taso.wc=10*10*2;

  for (y=0; y<10; y++) for (x=0; x<10; x++) {
    taso.pisteet[y*10+x].x=(x-5);
    taso.pisteet[y*10+x].y=(y-5);
    taso.pisteet[y*10+x].z=0;
  }
  for (y=0; y<9; y++) for (x=0; x<9; x++) {
    taso.langat[y*20+x*2].piste1=&taso.pisteet[y*10+x];
    taso.langat[y*20+x*2].piste2=&taso.pisteet[(y+1)*10+x];
    taso.langat[y*20+x*2+1].piste1=&taso.pisteet[y*10+x];
    taso.langat[y*20+x*2+1].piste2=&taso.pisteet[y*10+x+1];
  }

  poksi.pisteet=pisteet1;
  poksi.langat=langat1;
  poksi.vc=8;
  poksi.wc=12;
  poksi.pisteet[0].x=-1; poksi.pisteet[0].y=1; poksi.pisteet[0].z=1;
  poksi.pisteet[1].x=1; poksi.pisteet[1].y=1; poksi.pisteet[1].z=1;
  poksi.pisteet[2].x=1; poksi.pisteet[2].y=-1; poksi.pisteet[2].z=1;
  poksi.pisteet[3].x=-1; poksi.pisteet[3].y=-1; poksi.pisteet[3].z=1;
  poksi.pisteet[4].x=-1; poksi.pisteet[4].y=1; poksi.pisteet[4].z=-1;
  poksi.pisteet[5].x=1; poksi.pisteet[5].y=1; poksi.pisteet[5].z=-1;
  poksi.pisteet[6].x=1; poksi.pisteet[6].y=-1; poksi.pisteet[6].z=-1;
  poksi.pisteet[7].x=-1; poksi.pisteet[7].y=-1; poksi.pisteet[7].z=-1;
  poksi.langat[0].piste1=&poksi.pisteet[0]; poksi.langat[0].piste2=&poksi.pisteet[1];
  poksi.langat[1].piste1=&poksi.pisteet[1]; poksi.langat[1].piste2=&poksi.pisteet[2];
  poksi.langat[2].piste1=&poksi.pisteet[2]; poksi.langat[2].piste2=&poksi.pisteet[3];
  poksi.langat[3].piste1=&poksi.pisteet[3]; poksi.langat[3].piste2=&poksi.pisteet[0];
  poksi.langat[4].piste1=&poksi.pisteet[4]; poksi.langat[4].piste2=&poksi.pisteet[5];
  poksi.langat[5].piste1=&poksi.pisteet[5]; poksi.langat[5].piste2=&poksi.pisteet[6];
  poksi.langat[6].piste1=&poksi.pisteet[6]; poksi.langat[6].piste2=&poksi.pisteet[7];
  poksi.langat[7].piste1=&poksi.pisteet[7]; poksi.langat[7].piste2=&poksi.pisteet[4];
  poksi.langat[8].piste1=&poksi.pisteet[0]; poksi.langat[8].piste2=&poksi.pisteet[4];
  poksi.langat[9].piste1=&poksi.pisteet[1]; poksi.langat[9].piste2=&poksi.pisteet[5];
  poksi.langat[10].piste1=&poksi.pisteet[2]; poksi.langat[10].piste2=&poksi.pisteet[6];
  poksi.langat[11].piste1=&poksi.pisteet[3]; poksi.langat[11].piste2=&poksi.pisteet[7];
/*
  poksi2.pisteet=pisteet3;
  poksi2.langat=langat3;
  poksi2.vc=8;
  poksi2.wc=12;
  poksi2.langat[0].piste1=&poksi2.pisteet[0]; poksi2.langat[0].piste2=&poksi2.pisteet[1];
  poksi2.langat[1].piste1=&poksi2.pisteet[1]; poksi2.langat[1].piste2=&poksi2.pisteet[2];
  poksi2.langat[2].piste1=&poksi2.pisteet[2]; poksi2.langat[2].piste2=&poksi2.pisteet[3];
  poksi2.langat[3].piste1=&poksi2.pisteet[3]; poksi2.langat[3].piste2=&poksi2.pisteet[0];
  poksi2.langat[4].piste1=&poksi2.pisteet[4]; poksi2.langat[4].piste2=&poksi2.pisteet[5];
  poksi2.langat[5].piste1=&poksi2.pisteet[5]; poksi2.langat[5].piste2=&poksi2.pisteet[6];
  poksi2.langat[6].piste1=&poksi2.pisteet[6]; poksi2.langat[6].piste2=&poksi2.pisteet[7];
  poksi2.langat[7].piste1=&poksi2.pisteet[7]; poksi2.langat[7].piste2=&poksi2.pisteet[4];
  poksi2.langat[8].piste1=&poksi2.pisteet[0]; poksi2.langat[8].piste2=&poksi2.pisteet[4];
  poksi2.langat[9].piste1=&poksi2.pisteet[1]; poksi2.langat[9].piste2=&poksi2.pisteet[5];
  poksi2.langat[10].piste1=&poksi2.pisteet[2]; poksi2.langat[10].piste2=&poksi2.pisteet[6];
  poksi2.langat[11].piste1=&poksi2.pisteet[3]; poksi2.langat[11].piste2=&poksi2.pisteet[7];

  matto.pisteet=pisteet2;
  matto.langat=langat2;
  matto.vc=20*5;
  matto.wc=19*5;
  for (y=0; y<5; y++) {
    for (x=0; x<19; x++) {
      matto.langat[y*19+x].piste1=&matto.pisteet[y*20+x];
      matto.langat[y*19+x].piste2=&matto.pisteet[y*20+x+1];
    }
  }

  pyra.pisteet=pyrapis;
  pyra.langat=pyralan;
  pyra.vc=5;
  pyra.wc=8;

  pyra1.pisteet=pyrapis1;
  pyra1.langat=pyralan1;
  pyra1.vc=5;
  pyra1.wc=8;

  pyra2.pisteet=pyrapis2;
  pyra2.langat=pyralan2;
  pyra2.vc=5;
  pyra2.wc=8;
  pyra.pisteet[0].x=0; pyra.pisteet[0].y=0; pyra.pisteet[0].z=1.5;
  pyra.pisteet[1].x=1; pyra.pisteet[1].y=1; pyra.pisteet[1].z=0;
  pyra.pisteet[2].x=1; pyra.pisteet[2].y=-1; pyra.pisteet[2].z=0;
  pyra.pisteet[3].x=-1; pyra.pisteet[3].y=-1; pyra.pisteet[3].z=0;
  pyra.pisteet[4].x=-1; pyra.pisteet[4].y=1; pyra.pisteet[4].z=0;
  pyra.langat[0].piste1=&pyra.pisteet[0]; pyra.langat[0].piste2=&pyra.pisteet[1];
  pyra.langat[1].piste1=&pyra.pisteet[0]; pyra.langat[1].piste2=&pyra.pisteet[2];
  pyra.langat[2].piste1=&pyra.pisteet[0]; pyra.langat[2].piste2=&pyra.pisteet[3];
  pyra.langat[3].piste1=&pyra.pisteet[0]; pyra.langat[3].piste2=&pyra.pisteet[4];
  pyra.langat[4].piste1=&pyra.pisteet[1]; pyra.langat[4].piste2=&pyra.pisteet[2];
  pyra.langat[5].piste1=&pyra.pisteet[2]; pyra.langat[5].piste2=&pyra.pisteet[3];
  pyra.langat[6].piste1=&pyra.pisteet[3]; pyra.langat[6].piste2=&pyra.pisteet[4];
  pyra.langat[7].piste1=&pyra.pisteet[4]; pyra.langat[7].piste2=&pyra.pisteet[1];

  pyra1.langat[0].piste1=&pyra1.pisteet[0]; pyra1.langat[0].piste2=&pyra1.pisteet[1];
  pyra1.langat[1].piste1=&pyra1.pisteet[0]; pyra1.langat[1].piste2=&pyra1.pisteet[2];
  pyra1.langat[2].piste1=&pyra1.pisteet[0]; pyra1.langat[2].piste2=&pyra1.pisteet[3];
  pyra1.langat[3].piste1=&pyra1.pisteet[0]; pyra1.langat[3].piste2=&pyra1.pisteet[4];
  pyra1.langat[4].piste1=&pyra1.pisteet[1]; pyra1.langat[4].piste2=&pyra1.pisteet[2];
  pyra1.langat[5].piste1=&pyra1.pisteet[2]; pyra1.langat[5].piste2=&pyra1.pisteet[3];
  pyra1.langat[6].piste1=&pyra1.pisteet[3]; pyra1.langat[6].piste2=&pyra1.pisteet[4];
  pyra1.langat[7].piste1=&pyra1.pisteet[4]; pyra1.langat[7].piste2=&pyra1.pisteet[1];

  pyra2.langat[0].piste1=&pyra2.pisteet[0]; pyra2.langat[0].piste2=&pyra2.pisteet[1];
  pyra2.langat[1].piste1=&pyra2.pisteet[0]; pyra2.langat[1].piste2=&pyra2.pisteet[2];
  pyra2.langat[2].piste1=&pyra2.pisteet[0]; pyra2.langat[2].piste2=&pyra2.pisteet[3];
  pyra2.langat[3].piste1=&pyra2.pisteet[0]; pyra2.langat[3].piste2=&pyra2.pisteet[4];
  pyra2.langat[4].piste1=&pyra2.pisteet[1]; pyra2.langat[4].piste2=&pyra2.pisteet[2];
  pyra2.langat[5].piste1=&pyra2.pisteet[2]; pyra2.langat[5].piste2=&pyra2.pisteet[3];
  pyra2.langat[6].piste1=&pyra2.pisteet[3]; pyra2.langat[6].piste2=&pyra2.pisteet[4];
  pyra2.langat[7].piste1=&pyra2.pisteet[4]; pyra2.langat[7].piste2=&pyra2.pisteet[1];
*/
}


void prekalkutsjon() {
  int x, y, i;
  float u, v, s, t;

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    u=159.75-x; v=(99.75-y)*1.2;
    t=fpatan(u, v)/pi*256.0+256.0;
    s=fsqrt(u*u+v*v)*1.25*0.5;
    s=fsqrt(s*128.0);
    s+=t*(1/512.0);
    tunneli[y*320+x]=(qftol(127-s)&0x7f)*512+(qftol(t)&0x1ff);
  }
  for (i=0; i<65536; i++) radiaali[i]=65535;
  for (i=0; i<64000; i++) radiaali[tunneli[i]&0xffff]=i;
  for (i=48000; i>=0; i--) if (radiaali[i]==65535) radiaali[i]=radiaali[i+1];
  for (i=16000; i<65536; i++) if (radiaali[i]==65535) radiaali[i]=radiaali[i-1];

  for (i=0; i<1024; i++) {
    float f=i/128.0; f=f<1?f:1; f=fsqrt(f);
    tbla1[i]=fpow(i/1024.0+0.0001, fpow( 0.30,f+0.0001))*512.0;
    //tblb1[i]=fpow(i/1024.0+0.0001, fpow( 0.20,f+0.0001))*512.0;
  }

  for (i=0; i<256; i++) {
    float f=i/128.0; f=f<1?f:1; f=fsqrt(f);
    tbla2[i]=fpow(i/256.0 +0.0001, fpow( 4.00,f+0.0001))*16384.0;
    //tblb2[i]=fpow(i/256.0 +0.0001, fpow( 5.00,f+0.0001))*16384.0;
  }


/*  for (y=0; y<512; y++) for (x=0; x<128; x++) {
    u=160+sin(y*pi/256.0)*x/1.335*2;
    v=100-cos(y*pi/256.0)*x/1.335*2;
    if (u<0) u=0; if (u>319) u=319; if (v<0) v=0; if (v>199) v=199;
    radiaali[(127-x)*512+511-y]=(int)v*320+(int)u;
  }*/
}


void unpak16(char *sourke, char *dest, int packedsize) {
  int i;
  for (i=0; i<packedsize; i++) {
    dest[i*2]=(sourke[i]&15)*16;
    dest[i*2+1]=((sourke[i]>>4)&15)*16;
  }
}



//////////////////////////////////////////////////// EFEKTIT

void teematriisi(float m[3][3], float a, float b, float c) {
  float sa, ca, sb, cb, sc, cc;
  sa=fsin(a); ca=fcos(a);
  sb=fsin(b); cb=fcos(b);
  sc=fsin(c); cc=fcos(c);
  m[2][0]=-sa*sb*cc-ca*sc;
  m[2][1]=-sa*cb;
  m[2][2]= ca*cc-sa*sb*sc;
  m[1][0]= ca*sb*cc-sa*sc;
  m[1][1]= ca*cb;
  m[1][2]= sa*cc+ca*sb*sc;
  m[0][0]= cb*cc;
  m[0][1]=-sb;
  m[0][2]= cb*sc;
}

void rotateobu(opu *opu_s, opu *opu_d, float x, float y, float z,
               float rot_o_x, float rot_o_y, float rot_o_z,
               float movx, float movy, float movz) {
  float matrix[3][3];
  int i;

  teematriisi(matrix, x, y, z);
  for (i=0; i<opu_s->vc; i++) {
    opu_d->pisteet[i].x=matrix[0][0]*(opu_s->pisteet[i].x+rot_o_x)+
                        matrix[0][1]*(opu_s->pisteet[i].y+rot_o_y)+
                        matrix[0][2]*(opu_s->pisteet[i].z+rot_o_z)-rot_o_x+movx;
    opu_d->pisteet[i].y=matrix[1][0]*(opu_s->pisteet[i].x+rot_o_x)+
                        matrix[1][1]*(opu_s->pisteet[i].y+rot_o_y)+
                        matrix[1][2]*(opu_s->pisteet[i].z+rot_o_z)-rot_o_y+movy;
    opu_d->pisteet[i].z=matrix[2][0]*(opu_s->pisteet[i].x+rot_o_x)+
                        matrix[2][1]*(opu_s->pisteet[i].y+rot_o_y)+
                        matrix[2][2]*(opu_s->pisteet[i].z+rot_o_z)-rot_o_z+movz;
  }
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
  yvec.x=0.1; yvec.y=1.0; yvec.z=0.3; normalisoi(&yvec);
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

void aa_viiva_3(int *dest, int x1, int y1, int c1,
                           int x2, int y2, int c2) {
  int x, y, xx, yy;
  int m1, m2, swap;
  int cplus, c;

  if (ia(x2-x1)>=ia(y2-y1)) {
    if (x1>x2) {
      swap=x2, x2=x1, x1=swap;
      swap=y2, y2=y1, y1=swap;
      swap=c2, c2=c1, c1=swap;
    }
    c=c1;
    cplus=(c2-c1)/((x2-x1)/256.0);
    m1=y2-y1; m2=x2-x1;
    for (x=x1+255&~255; x<x2; x+=256) if (x>1280 && x<44800) {
      y=y1+((x-x1)*1.0*m1)/m2;
      xx=x>>8; yy=y>>8;
      if (yy>4 && yy<115) {
        c+=cplus;
        dest[yy*180+xx]=dest[yy*180+xx]+(255-(y&255))*c;
        dest[(yy+1)*180+xx]=dest[(yy+1)*180+xx]+(y&255)*c;
      }
    }
  } else {
    if (y1>y2) {
      swap=x2, x2=x1, x1=swap;
      swap=y2, y2=y1, y1=swap;
    }
    c=c1;
    cplus=(c2-c1)/((y2-y1)/256.0);
    m1=y2-y1; m2=x2-x1;
    for (y=y1+255&~255; y<y2; y+=256) if (y>1280 && y<29440) {
      x=x1+((y-y1)*1.0*m2)/m1;
      xx=x>>8; yy=y>>8;
      if (xx>4 && xx<175) {
        c+=cplus;
        dest[yy*180+xx]=dest[yy*180+xx]+(255-(x&255))*c;
        dest[yy*180+xx+1]=dest[yy*180+xx+1]+(x&255)*c;
      }
    }
  }
}

void suurenna(int *sourke, char *dest, int w, int h) {
  int x1, y1, x2, y2, u, v, x, y;
  char *d;
  int *s;

  x1=10; y1=10; x2=w+10; y2=h+10;
  for (y=y1, v=0; y<y2; y++, v+=2) {
    d=dest+v*320; s=sourke+y*180+x1;
    for (x=x1; x<x2; x++, d+=2, s++) {
      d[0]=rtbl[s[0]>>13];
      d[1]=rtbl[(s[0]+s[1])*5-(s[-1]+s[2])>>16];
    }
  }
  for (v=1; v<199; v+=2) {
    d=dest+v*320;
    if (v==1 || v==197)
      for (u=0; u<320; u++, d++) d[0]=d[-320]+d[320]+1>>1;
    else
      for (u=0; u<320; u++, d++) d[0]=rtbl[(d[-320]+d[320])*5-(d[-960]+d[960])>>3];
  }
  d=dest+199*320;
  for (u=0; u<320; u++, d++) *d=0;
}

void putpixel(int *dest, int x, int y) {
  if (x<0) x=0; if (x>159) x=159;
  if (y<0) y=0; if (y>99) y=99;
  dest[y*160+x]=255<<8;
}


void teewiretys(int *dest, opu *opux, float posx, float posy, float posz,
                                      float movx, float movy, float movz) {
  float camera[3][3];
  int i;

  teematriisi2(camera, 0-posx, 0-posy, 0-posz);

  for (i=0; i<opux->vc; i++) {
    opux->pisteet[i].rx=camera[0][0]*(opux->pisteet[i].x+movx-posx)+
                        camera[0][1]*(opux->pisteet[i].y+movy-posy)+
                        camera[0][2]*(opux->pisteet[i].z+movz-posz);
    opux->pisteet[i].ry=camera[1][0]*(opux->pisteet[i].x+movx-posx)+
                        camera[1][1]*(opux->pisteet[i].y+movy-posy)+
                        camera[1][2]*(opux->pisteet[i].z+movz-posz);
    opux->pisteet[i].rz=camera[2][0]*(opux->pisteet[i].x+movx-posx)+
                        camera[2][1]*(opux->pisteet[i].y+movy-posy)+
                        camera[2][2]*(opux->pisteet[i].z+movz-posz);
  }

  for (i=0; i<opux->vc; i++) {
    opux->pisteet[i].px=90*256+opux->pisteet[i].rx/opux->pisteet[i].rz*256*64;
    opux->pisteet[i].py=60*256+opux->pisteet[i].ry/opux->pisteet[i].rz*256*64;
    opux->pisteet[i].pz=0.5/opux->pisteet[i].rz*262144;
    if (opux->pisteet[i].rz<0) opux->pisteet[i].px=1234567890;
  }
  for (i=0; i<opux->wc; i++) {
    if (opux->langat[i].piste1->px!=1234567890 && opux->langat[i].piste2->px!=1234567890)
    aa_viiva_3(dest,
      opux->langat[i].piste1->px, opux->langat[i].piste1->py, opux->langat[i].piste1->pz,
      opux->langat[i].piste2->px, opux->langat[i].piste2->py, opux->langat[i].piste2->pz);
      //putpixel(dest, opux->langat[i].piste1->px>>8, opux->langat[i].piste1->py>>8);
      //putpixel(dest, opux->langat[i].piste2->px>>8, opux->langat[i].piste2->py>>8);
    }
}

void teeblurri(int *dest) {
  static int moi[21600];
  int x, y, i, *p;
  char *d;

  //for (y=0; y<100; y++) for (x=0; x<160; x++) moi[y*160+x]=(dest[y*640+x*2]<<20);//+65536*0;
  //for (i=0; i<32000; i++) moi[i]=dest[i]<<17;
  move32(dest, moi, 21600);
  //for (x=0; x<16000; x++) moi[x]=(moi[x]-65536*8)*4+65536*4;
  for (y=1; y<120; y++)
  for (p=moi+y*180+3; p<moi+y*180+175; p+=4) {
    p[ 0]=p[-1]+(p[-177]+(p[ 0]-p[-177]>>2)-p[-1]>>2);
    p[ 1]=p[ 0]+(p[-176]+(p[ 1]-p[-176]>>2)-p[ 0]>>2);
    p[ 2]=p[ 1]+(p[-175]+(p[ 2]-p[-175]>>2)-p[ 1]>>2);
    p[ 3]=p[ 2]+(p[-174]+(p[ 3]-p[-174]>>2)-p[ 2]>>2);
  }

  for (y=118; y>=0; y--)
  for (p=moi+y*180+176; p>=moi+y*180+4; p-=4) {
    p[ 0]=p[ 1]+(p[ 177]+(p[ 0]-p[ 177]>>2)-p[ 1]>>2);
    p[-1]=p[ 0]+(p[ 176]+(p[-1]-p[ 176]>>2)-p[ 0]>>2);
    p[-2]=p[-1]+(p[ 175]+(p[-2]-p[ 175]>>2)-p[-1]>>2);
    p[-3]=p[-2]+(p[ 174]+(p[-3]-p[ 174]>>2)-p[-2]>>2);
  }
/*
  for (y=0, p=moi; y<100; y++) for (d=dest+y*640, x=0; x<160; x++, p++, d+=2) {
    //d[  0]=rtbl[d[  0]+(p[0]>>16)];
    //d[  1]=rtbl[d[  1]+(p[0]+p[1]>>17)];
    //d[320]=rtbl[d[320]+(p[0]+p[160]>>17)];
    //d[321]=rtbl[d[321]+(p[0]+p[161]>>17)];
    d[  0]=rtbl[(p[0]>>16)];
    d[  1]=rtbl[(p[0]+p[1]>>17)];
    d[320]=rtbl[(p[0]+p[160]>>17)];
    d[321]=rtbl[(p[0]+p[161]>>17)];
  } */
  for (i=0; i<21600; i++) dest[i]=(dest[i]>>10)+moi[i];
}

void teeradiaali(char *sourke, int *dest) {
  int x;
  for (x=0; x<65536; x++) dest[x]=tbla2[sourke[radiaali[x]]];
}
void blur2(int *dest, int delta, int p) {
  int *d;

  p>>=2;
  if (delta<0) {
    for (d=dest-delta; d<dest+65536; d++) { d[0]+=(d[delta]-d[0])*p>>14; }
  } else {
    for (d=dest+65535-delta; d>=dest; d--) { d[0]+=(d[delta]-d[0])*p>>14; }
  }
}

/*

void box(signed short *d, laatikko *p) {
  int x1, y1, x2, y2, x;

  x1=p->x1>>8; y1=p->y1;
  x2=p->x2>>8; y2=p->y2;

  if (x1<0) x1=0;
  if (x2>319) x2=319;
  if (y1<0) y1=0;
  if (y2>199) y2=199;

  if (x1<x2)
    for (x=y1; x<y2; x++) {
      d[x*320+x1]+=p->br;
      d[x*320+x2]-=p->br;
    }
}

void alkuefekti(float t) {
  int x, temp, temp2;
  short joops;
  int maara;

  fill32(schar, 32000, 0);
  if (t<2) maara=(int)(t*10);
  else if (t>=3) maara=(int)((t-3)*10);

  for (x=0; x<maara; x++) if (x<sizeof(poksit)/sizeof(poksit[0])) {
    poksit[x].x1-=poksit[x].vauhti;
    poksit[x].x2-=poksit[x].vauhti;
    if (poksit[x].x2<0 && (t<2 || t>=3)) {
      poksit[x].x1=320*256;
      poksit[x].x2=(320+rand()/256+64)*256;
      temp=(rand()-16834)/256+100;
      temp2=(rand()/4096)+16;
      poksit[x].y1=temp-temp2;
      poksit[x].y2=temp+temp2;
      poksit[x].vauhti=rand()/32+256;
      poksit[x].br=rand()/1024+8;
    }
    box(schar, &poksit[x]);
  }
  joops=0;
  for (x=0; x<64000; x++) {
    joops+=schar[x];
    //dbuf[x]=rtbl[joops];
    //if ((int)t==x) dbuf[x]=255;
    dbuf[x]=rtbl[joops*mewlers[x]>>7];
  }
  move32(dbuf, videomem, 16000);
}
*/

void aa_sprite(char *dest, char *sourke, int x1, int y1, int x2, int y2) {
  int x, y;
  int xi, yi;
  int xp, yp, xp2;

  xi=(150<<20)/(x2-x1);
  yi=(100<<20)/(y2-y1);
  xp2=(256-(x1&255))*xi>>12;
  yp=(256-(y1&255))*yi>>12;

  if (x1<0) xp2=-x1*xi>>8, x1=0;
  if (y1<0) yp=-y1*yi>>8, y1=0;
  if (x2>319*256) x2=319*256;
  if (y2>199*256) y2=199*256;

  for (y=y1; y<y2; y+=256) {
    char *rivi=sourke+(yp>>12)*150;
    char *dp=dest+(y>>8)*320+(x1>>8);
    int ypf1=yp&4095, ypf0=4096-ypf1;
    xp=xp2;
    for (x=x1; x<x2; x+=256) {
      char *tex=rivi+(xp>>12);
      *dp++=
        (tex[0]*ypf0+tex[150]*ypf1>>12)*(4096-(xp&4095))+
        (tex[1]*ypf0+tex[151]*ypf1>>12)*(xp&4095)>>12;
      xp+=xi;
    }
    yp+=yi;
  }
}



//////////////////////////////////////////////////// EFEKTIEN PY™RITTŽJŽT


void multibox(float t) {
  int x, y, z;
  float juttu;
  float xp, yp, zp;
  float xx, yy, zz;
  int br;

  //xp=fsin(t*6.123)*2.0;
  //yp=fsin(t*4.223)*6.5;
  //zp=-5.0;
  xp=fsin(t*14.123)*2.0;
  yp=fsin(t*2.123)*1.5;
  zp=fcos(t*14.123)*2.0;

  fill32(ibuf, 21600, 0);
/*
  juttu=fsin(t*4.0)*0.5+1;
  for (z=0; z<5; z++) for (y=0; y<5; y++) for (x=0; x<5; x++) {
    xx=(x-2)*2; yy=(y-2)*2; zz=(z-2)*2;
    if (iaf(xx)>3.5 || iaf(yy)>3.5 || iaf(zz)>3.5)
      teewiretys(ibuf, &poksi, xp, yp, zp, xx*juttu, yy*juttu, zz*juttu);
  } */
  teewiretys(ibuf, &poksi, xp, yp, zp, 0, 0, 0);
  teeblurri(ibuf);
  suurenna(ibuf, dbuf, 160, 100);
  move32(dbuf, videomem, 16000);
}

void multibox2(float t) {
  int x, y, z;
  float juttu;
  float xp, yp, zp;
  float xx, yy, zz;
  int br;

  //xp=fsin(t*6.123)*2.0;
  //yp=fsin(t*4.223)*6.5;
  //zp=-5.0;
  xp=fsin(t*4.123)*2.5;
  yp=fsin(t*2.123)*1.5;
  zp=fcos(t*4.123)*2.5;

  fill32(ibuf, 21600, 0);
/*
  juttu=fsin(t*4.0)*0.5+1;
  for (z=0; z<5; z++) for (y=0; y<5; y++) for (x=0; x<5; x++) {
    xx=(x-2)*2; yy=(y-2)*2; zz=(z-2)*2;
    if (iaf(xx)>3.5 || iaf(yy)>3.5 || iaf(zz)>3.5)
      teewiretys(ibuf, &poksi, xp, yp, zp, xx*juttu, yy*juttu, zz*juttu);
  } */
  teewiretys(ibuf, &poksi, xp, yp, zp, 0, 0, 0);
  teeblurri(ibuf);
  suurenna(ibuf, dbuf, 160, 100);
  for (x=0; x<64000; x++) dbuf[x]=rtbl[dbuf2[x]-(dbuf[x]>>1)];
  teeradiaali(dbuf, valotbl);
  blur2(valotbl, 512, 63000);
  for (y=0; y<64000; y++) dbuf[y]=rtbl[(dbuf[y]<<1)+tbla1[valotbl[tunneli[y]]>>7]];

  move32(dbuf, videomem, 16000);
}


void oski(float t) {
  int x, y;

  fill32(dbuf, 16000, 0);
  for (x=0; x<320; x++) {
    y=(xtal[x])%200;
    dbuf[y*320+x]=255;
  }
  move32(dbuf, videomem, 16000);
}

typedef struct {
  int xp, yp;
  float z, t0;
  int spd;
  float juttu;
  char *s;
} Fishii;

void alkufisu(float t) {
  int x, y;
  static Fishii fishii[5];
  static int init=1;
  int x1, y1, x2, y2;

/*  if (init==1) {
    init=0;
    for (x=0; x<5; x++) fishii[x].xp=-101*256, fishii[x].t0=t;
  }
  for (x=0; x<5; x++) {
    if (fishii[x].xp+fishii[x].spd*(t-fishii[x].t0)<-100*256||fishii[x].xp+fishii[x].spd*(t-fishii[x].t0)>420*256) {
      fishii[x].yp=rand()-16348+25600;
      fishii[x].xp=(rand()>>14)*520*256-25600;
      fishii[x].z=(rand()+32768)/131072.0;
      fishii[x].juttu=1;
      fishii[x].spd=(fishii[x].xp>0?-rand()-32768:rand()+32768);
      fishii[x].s=(fishii[x].xp>0?&fish2:&fish1);
      fishii[x].t0=t;
    }
  }
  fill32(dbuf, 16000, 0);
  for (x=0; x<5; x++) {
    //fishii[x].xp+=fishii[x].spd;
    x1=fishii[x].xp+fishii[x].spd*(t-fishii[x].t0)-150*256*fishii[x].z;
    y1=fishii[x].yp-100*256*fishii[x].z;
    x2=fishii[x].xp+fishii[x].spd*(t-fishii[x].t0)+150*256*fishii[x].z;
    y2=fishii[x].yp+100*256*fishii[x].z;
    aa_sprite(dbuf, fishii[x].s, x1, y1, x2, y2);
  }
  move32(dbuf, videomem, 16000);*/
  x=(fsin(t*13.6)*0.3+1.2)*170*256;
  y=(fsin(t*12.9)*0.3+1.2)*140*256;
  //x=75*256;
  aa_sprite(dbuf, fish1, 160*256-x, 100*256-y, 160*256+x, 100*256+y);
  move32(dbuf, videomem, 16000);
}


//////////////////////////////////////////////////// MUSASYSTEM

char *get_env(char *s) {
  extern char *envoffset;
  char *p=envoffset, *q, *t;
  while (*p) {
    for (q=p, t=s; *q!='=' && *q==*t; q++, t++) ;
    if (*q=='=' && *t==0) return q+1;
    while (*p) p++; p++;
  }
  return 0;
}
int parse(char *s, int mul) {
  int r = 0;

  while (*s && (*s < '0' || *s > '9')) s++;
  while (*s >= '0' && *s <= '9') {
    r *= mul;
    r += *s - '0';
    s++;
  }
  return r;
}
int checkenv(tdinfo *dinfo) {
  int i;
  i = dinfo->base >= 0x200 && dinfo->base <= 0x290;
  i &= dinfo->irq >= 2 && dinfo->irq <= 15;
  i &= dinfo->dma1 <= 7;
  i &= dinfo->dma2 <= 7;
  return i;
}
char *strchr(char *s, char c) {
  while (*s && *s!=c) s++;
  if (*s==c) return s; else return 0;
}
int gusenv(tdinfo *dinfo, int flags, int iwflag) {
  char *s;

  stosb(dinfo, 0, sizeof(tdinfo));
  s = get_env("ULTRASND");
  if (!s) return 0;
  dinfo->base = parse(s,16);
  s = strchr(s,',');
  if (!s) return 0;
  dinfo->dma1 = parse(s,10);
  s = strchr(s+1,',');
  if (!s) return 0;
  dinfo->dma2 = parse(s,10);
  s = strchr(s+1,',');
  if (!s) return 0;
  dinfo->irq = parse(s,10);

  //if (iwflag >= 2 || (iwflag == 1 && get_env("INTERWAVE"))) dinfo->flags |= gus_iw;
  dinfo->flags |= flags;

  return checkenv(dinfo);
}

int sbenv(tdinfo *dinfo, int rate, int flags) {
  char *s;
  char *v;

  stosb(dinfo, 0, sizeof(tdinfo));
  s = get_env("BLASTER");
  if (!s) return 0;
  v = strchr(s,'A');
  if (v) dinfo->base = parse(v,16);
  v = strchr(s,'I');
  if (v) dinfo->irq = parse(v,10);
  v = strchr(s,'D');
  if (!v) return 0;
  dinfo->dma1 = parse(v,10);
  v = strchr(s,'H');
  if (v) dinfo->dma2 = parse(v,10);
  dinfo->rate = rate;
  dinfo->flags = flags;
  return checkenv(dinfo);
}



//////////////////////////////////////////////////// LE MAIN LOOP

void pal() {
  int x;
  for (x=0; x<256; x++) {
    ulos(0x3c8, x);
    ulos(0x3c9, x/4);
    ulos(0x3c9, x/4);
    ulos(0x3c9, x/4);
  }
}


int main() {
  int kortti, port, irq, dma, testi, test=0;
  int x, y, u, v, i, c1, c2, c3;
  float t, teeplus;
  static tdinfo dinfo;
  static void *drv_mem;
  int bpm=200;
  extern void gonvert_samples();

  rxminit();
  mark();
  gonvert_samples(musiko);

  //fldcw(0x33f);

  if (checkenv(&dinfo)!=0) {
    dosputs("\r\n\nInvalid soundcard configuration!\r\n$");
    exit(1);
  }
  //gus
  if (gusenv(&dinfo,dflags,iwflag)) test = u_test(&dinfo);
  if (test) dosputs("Gravis Ultrasound detected...\r\n\n$");
  //sb
  if (!test && sbenv(&dinfo,mixrate,dflags)) {
    test = sb_test(&dinfo);
    if (test) dosputs("Soundblaster detected...\r\n\n$");
  }
  //no sound
  if (!test) n_test(&dinfo), dosputs("No sound... If you have a soundcard, use the environment variables\r\n\n$");

  if (dinfo.mem) drv_mem = zgetmem(dinfo.mem);
  if (dinfo.dmabuf) {
    if ((dinfo.dmabuf = getdmabuf(dinfo.dmabuf))==0) {
      dosputs("\r\n\nNot enough low mem\r\n$");
      exit(1);
    }
  }

  i8_init();
  testi=rxmdevinit(&dinfo, drv_mem);
  if (testi) {
    if (testi == 1) dosputs("\r\n\nDMA error\r\n$");
    if (testi == 2) dosputs("\r\n\nIRQ error\r\n$");
    i8_done();
    exit(1);
  }

  dosputs("\r\n\nPrecalculating...$");

  for (x=0; x<8192; x++) rtbl2[x]=0;
  for (x=8192; x<8192+256; x++) rtbl2[x]=x-8192;
  for (x=8192+256; x<16384; x++) rtbl2[x]=255;

  precalcoput();
  prekalkutsjon();
  for (x=0; x<sizeof(fisu)/sizeof(fisu[0]); x++) fisu[x]=rand();
  unpak16(fisu, fish1, 7500);
  for (y=0; y<100; y++) for (x=0; x<150; x++) fish2[y*150+x]=fish1[y*150+149-x];

  //for (x=0; x<64000; x++) mewlers[x]=mewlers[x]+256>>1;
  //for (x=0; x<1000; x++) {
    //poksit[x].x2=-1;
  //}

  mode(0x13);
  //pal();

  oldk=get_intr(0x9);
  set_intr(0x9, newk);

  maincount=0;
  teeplus=0;
  rxmsetvol(vol);
  rxmplay(&musiko, 0);

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    i=262144*6/((y-100)*(y-100)+(x-160)*(x-160)+6000.1);
    if (i>255) i=255;
    dbuf2[y*320+x]=i;
  }

  while (!kcnt[1] && t<50) {
    t=maincount*bpm/(16*60000.0)+teeplus;

    //if (t<8) alkuefekti(t);
    alkufisu(t);
    //multibox2(t);
    //oski(t);
    //fill32(dbuf, 16000, 0);
    //aa_viiva_3(dbuf, 0, 0, 65535, 319<<8, 199<<9, 65535);
    //move32(dbuf2, videomem, 16000);

    while (kcnt[129]&1) ;
    if (kcnt[0x4e]) { rxmskip(1); teeplus=teeplus+1; kcnt[0x4e]=0; }
  }

  set_intr(0x9, oldk);
  mode(0x3);
  rxmstop(xmStop);
  rxmdevdone();
  i8_done();
  freedmabuf();
  release();
  dosputs("\033[37mNow it's safe to turn off your computer.\r\n$");
  return 0;
}

