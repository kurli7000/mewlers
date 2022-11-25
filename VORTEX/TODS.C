#include "tinymath.h"
#include "tinyheap.h"
#include "tinyfile.h"
#include <stdio.h>

#define dis 65536
/*
typedef struct {
  float x, y, z;
} vector;
*/
extern char buf160[16000], dbuf[64000];
int interpre[16*16*4];
typedef struct {
  float x, y, z;
  int rnd;
} dot;
dot dots[dis];
typedef struct {
  int a, r, h; // angle radius height
  int rnd;     // no use
} wonder;
wonder stars[dis];



extern float sintbl[4096], costbl[4096];
extern int pienix[41*26], pieniz[41*26], varjot[41*26];
extern char isox[64000], isoz[64000], valo[64000];
extern char mappi2[65536];
extern int rtbl[2048];


/*
void mode(int);
#pragma aux mode="int 0x10" parm[eax] modify[edx];
void move32(void *zourke, void *dezt, int kount);
#pragma aux move32= "cld" "rep movsd" parm[esi][edi][ecx];
void fill32(void *dezt, int kount, int num);
#pragma aux fill32= "cld" "rep stosd" parm[edi][ecx][eax];
void ulos(int port, char data);
#pragma aux ulos= "out dx, al" parm[edx][al];
char sisaan(int port);
#pragma aux sisaan= "in al, dx" parm[edx][al] value[al];
//float fabs(float a) { return a>0?a:-a; }
int hm(int k) { return k<255?k:255; }
int hmx(int k) { return k<255?(k>0?k:0):255; }
float fabs(float a);
#pragma aux fabs= "fabs" parm [8087] value [8087];
static long qftoltemp;
long qftol(float);
#pragma aux qftol = "fistp dword ptr[qftoltemp]" \
                    "mov eax, dword ptr[qftoltemp]" parm [8087] value [eax];
void qftolp(int*, float);
#pragma aux qftolp = "fistp dword ptr[eax]" parm [eax][8087];
int random(int luku) {
  return rand2()*luku/32768;
}


void precalculatemappi() {
  int x,y,i;

  for (y=0; y<256; y++) for (x=0; x<256; x++) mappi2[(y<<8)+x]=rand2();
  for (i=0; i<32; i++) { // laskettu mappi
    for (y=0; y<256; y++) for (x=0; x<256; x++) {
      mappi2[(y<<8)+x]=(mappi2[(y<<8)+x]+mappi2[(y+1<<8&0xff00)+(x+1&0xff)]
                       +mappi2[(y<<8)+(x+1&0xff)]+mappi2[(y+1<<8&0xff00)+x])/4;
    }
  }
}  */

int rand2() {
  static int seed=1287942342, seed2=875434567, seed3=1731208931;
  seed=seed*(565342345+seed3--)+2087061567+seed2++;
  return seed>>16&0x7fff;
}


void precalculoitunneli(float zoom, int aste) {
int x, y, u, v;

  for (y=0; y<200; y++) for (x=0; x<320; x++) {
    u=160-x;
    v=100-y;
    isoz[y*320+x]=hm(qftol(3000/fsqrt(u*u+v*v+0.1)*zoom))>>5;
    isox[y*320+x]=qftol(fpatan(u, v)/pi*aste);
//    valo[y*320+x]=hm(isoz[y*320+x]);
  }
}


/*void precalcigalaxi() {
  int x,y;

  for (x=0; x<dis; x++) dots[x].rnd=rand2();
  for (x=0; x<dis; x++) {
    do {
      stars[x].a=rand2()&4095;
      stars[x].r=rand2()*500;
      y=(mappi2[(stars[x].a>>4)+32+(stars[x].r>>18)*256&0xffff]-110)*1024;
      if (y<0) y=0;
      if (y>24000) y=24000;
      y+=8000.0/(1+stars[x].r/2000000.0);
      stars[x].h=(rand2()-rand2()>>6)*y>>12;
    } while (y<rand2());
    stars[x].rnd=rand2();
  }
}*/
void precalcigalaxi() {
  int x,y,xx,yy,zz;
  float r;
  FILE *fp;
  static char blaah[262144];
  int *p;

  fp=fopen("c:\\graphics\\dpaint\\galaxi\\galaxi.tga","rb");
  if (!fp) { v_close(); dosputs("precalc: file not found$"); }
  fseek(fp,786,SEEK_SET);
  fread(blaah, 1, 262144, fp);
  fclose(fp);
  for (x=0; x<dis; x++) dots[x].rnd=rand2();
  for (x=0; x<dis; x++) {
    do {
      xx=rand2()&511; yy=rand2()&511; zz=rand2()&511;
      y=blaah[yy*512+xx]*(blaah[zz*512+yy])*(blaah[xx*512+zz])>>8;// y=y*y>>8;
      dots[x].x=(xx-255.5)*16+(rand2()%16)+(rand2()/32768.0);
      dots[x].z=(yy-255.5)*16+(rand2()%16)+(rand2()/32768.0);
//      r=dots[x].x*dots[x].x+dots[x].z*dots[x].z; r=1000/(r*0.00001+1);
      dots[x].y=(zz-255.5)+blaah[xx*512+511-yy]-128+rand2()/32768.0;//(int)(y+r)*(rand2()-rand2())>>16;
    } while (y<rand2());
    r=(1<<(rand2()+rand2()>>13))/16.0;
    dots[x].x*=r; dots[x].y*=r; dots[x].z*=r;
//    if (rand2()<8000) { dots[x].x*=-8; dots[x].y*=-32; dots[x].z*=-8; }
    dots[x].rnd=rand2();
  }
  fp=fopen("galaxi.pre", "wb");
  fwrite(dots, 1, sizeof(dots), fp);
  fclose(fp);
}
void loadgalaxi() {
  int x,y,*p;
  FILE *fp;

  fp=fopen("galaxi.pre", "rb");
  fread(dots, 1, sizeof(dots), fp);
  fclose(fp);
  p=interpre;
  for (y=0; y<16; y++) for (x=0; x<16; x++) {
    *p++=x*y<<2;
    *p++=16*y-x*y<<2;
    *p++=16*x-x*y<<2;
    *p++=256-16*x-16*y+x*y<<2;
  }
}
/*

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
  yvec.x=0.0; yvec.y=1.0; yvec.z=0.0; normalisoi(&yvec);
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


void interpoloi2(int *sourke, char *dest) {
  int c, d, aa, x, y;
  char *dest2, *destm;
  int *s;

  c=c; d=d; aa=aa;
  s=sourke;
  for (y=0; y<25; y++, s++) {
    dest2=dest+y*8*320;
    for (x=0; x<40; x++, s++, dest2+=8) {
      destm=dest2+8*(320-8);
      _asm {
        mov eax, s
        mov edi, dest2
        mov ebx, [eax+4]
        mov esi, [eax+0]
        mov aa, ebx
        mov ecx, [eax+168]
        mov edx, [eax+164]
        sub esi, ebx
        sub edx, ecx
        sub ecx, ebx
        add esi, 0x800000
        add edx, 0x800000
        and esi, 0xffffff
        and edx, 0xffffff
        sub edx, 0x800000
        sar edx, 3
        sub esi, 0x800000
        sar esi, 3
        add ecx, 0x800000
        and ecx, 0xffffff
        sub edx, esi
        sar edx, 3
        sub ecx, 0x800000
        sar ecx, 3
        mov d, edx
        mov c, ecx

        innerluuppi:
          mov ecx, ebx
          add ebx, esi
          shr ecx, 16
          mov edx, ebx
          shr edx, 16
          mov ah, cl
          mov al, dl
          add ebx, esi
          shl eax, 16
          mov ecx, ebx
          shr ecx, 16
          add ebx, esi
          mov ah, cl
          mov edx, ebx
          shr edx, 16
          add ebx, esi
          mov al, dl
          mov ecx, ebx
          shr ecx, 16
          mov [edi+4], eax
          mov ah, cl
          add ebx, esi
          mov edx, ebx
          add ebx, esi
          shr edx, 16
          mov al, dl
          shl eax, 16
          mov ecx, ebx
          shr ecx, 16
          add ebx, esi
          mov ah, cl
          mov edx, ebx
          shr edx, 16
          mov al, dl
          mov [edi], eax

          add edi, 320
          mov ebx, aa
          add ebx, c
          mov aa, ebx
          add esi, d
          cmp edi, destm
        jb innerluuppi
      }
    }
  }
}
void interpoloi3(int *sourke, char *dest) {
  int c, d, aa, x, y;
  char *dest2, *destm;
  int *s;

  c=c; d=d; aa=aa;
  s=sourke;
  for (y=0; y<25; y++, s++) {
    dest2=dest+y*8*320;
    for (x=0; x<40; x++, s++, dest2+=8) {
      destm=dest2+8*(320-8);
      _asm {
        mov eax, s
        mov edi, dest2
        mov ebx, [eax+4]
        mov esi, [eax+0]
        mov aa, ebx
        mov ecx, [eax+168]
        mov edx, [eax+164]
        sub esi, ebx
        sub edx, ecx
        sub ecx, ebx
        ;add esi, 0x800000
        ;add edx, 0x800000
        ;and esi, 0xffffff
        ;and edx, 0xffffff
        ;sub edx, 0x800000
        sar edx, 3
        ;sub esi, 0x800000
        sar esi, 3
        ;add ecx, 0x800000
        ;and ecx, 0xffffff
        sub edx, esi
        sar edx, 3
        ;sub ecx, 0x800000
        sar ecx, 3
        mov d, edx
        mov c, ecx

        innerluuppi:
          mov ecx, ebx
          add ebx, esi
          shr ecx, 16
          mov edx, ebx
          shr edx, 16
          mov ah, cl
          mov al, dl
          add ebx, esi
          shl eax, 16
          mov ecx, ebx
          shr ecx, 16
          add ebx, esi
          mov ah, cl
          mov edx, ebx
          shr edx, 16
          add ebx, esi
          mov al, dl
          mov ecx, ebx
          shr ecx, 16
          mov [edi+4], eax
          mov ah, cl
          add ebx, esi
          mov edx, ebx
          add ebx, esi
          shr edx, 16
          mov al, dl
          shl eax, 16
          mov ecx, ebx
          shr ecx, 16
          add ebx, esi
          mov ah, cl
          mov edx, ebx
          shr edx, 16
          mov al, dl
          mov [edi], eax

          add edi, 320
          mov ebx, aa
          add ebx, c
          mov aa, ebx
          add esi, d
          cmp edi, destm
        jb innerluuppi
      }
    }
  }
} */

void teevaloefekti(char *dest, float posx, float posy, float posz) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, t;
  int x, y;
  float perx, pery;
  float ax, bx, cx;
  int xypos;

  teematriisi2(camera, 0-posx, 0-posy, 0-posz);

  pery=1/12.5;
  perx=1/15.0;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++) {
      suunta[0]=camera[2][0] + camera[1][0]*(y-13)*pery + camera[0][0]*(x-20)*perx;
      suunta[1]=camera[2][1] + camera[1][1]*(y-13)*pery + camera[0][1]*(x-20)*perx;
      suunta[2]=camera[2][2] + camera[1][2]*(y-13)*pery + camera[0][2]*(x-20)*perx;

      ax=(suunta[0]*suunta[0])+(suunta[1]*suunta[1]);
      bx=(suunta[0]*posx)+(suunta[1]*posy);
      cx=(posx*posx)+(posy*posy)-1;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];
      xypos=y*41+x;
      qftolp(pienix+xypos, fpatan(px, py)/pi*1*128*65536);
      qftolp(pieniz+xypos, pz*64*0.04*65536);
      qftolp(varjot+xypos, t*65536*32);
      if (varjot[xypos]>250*65536) varjot[xypos]=250*65536;
      if (varjot[xypos]<0) varjot[xypos]=0;
    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
  for (x=0; x<64000; x++) dest[x]=rtbl[dest[x]+(mappi2[(isox[x]<<8)+isoz[x]]*valo[x]>>6)+1024];
}


void tee2dvalo(int zoom) {
  int x, y, i, j;
  float k;
  int pikkuvalo[41*26];


  for (y=0; y<26; y++) for (x=0; x<41; x++) {
    i=x-20;
    j=y-13;
    k=(65536*zoom/(i*i+j*j+0.001));
    if (k>255*65536) k=255*65536; if (k<65536) k=65536;
    pikkuvalo[y*41+x]=k;
  }
  interpoloi3(pikkuvalo, valo);
}




void teevalo(char *dest, int z, int koko) {
  int x;
  char u, v;

  tee2dvalo(koko);
  for (x=0; x<64000; x++) {
    u=isoz[x]+z;
    v=isox[x];
    dest[x]=rtbl[1024+dest[x]+((mappi2[(u<<8)+v]-50)*valo[x]>>6)];
  }
}


void dotti(char *dest, int x, int y, int c) {
  char *d2;
  int xx=x&4095, yy=y&4095, xxyy=xx*yy>>12;

  d2=dest+(y>>12)*160+(x>>12);
  d2[0  ]=rtbl[d2[0  ]+((4096-xx-yy+xxyy)*c>>12)+1024];
  d2[1  ]=rtbl[d2[1  ]+((xx-xxyy)*c>>12)+1024];
  d2[160]=rtbl[d2[160]+((yy-xxyy)*c>>12)+1024];
  d2[161]=rtbl[d2[161]+((   xxyy)*c>>12)+1024];
}


void suurenna(char *sourke, char *dest, int w, int h) {
  int x1, y1, x2, y2, u, v, x, y;
  char *s, *d;

  x1=0; y1=0; x2=w; y2=h;
  for (y=y1, v=0; y<y2; y++, v+=2) {
    d=dest+v*320; s=sourke+y*160+x1;
    for (x=x1; x<x2; x++, d+=2, s++) {
      d[0]=s[0];
      d[1]=rtbl[(s[0]+s[1])*5-(s[-1]+s[2])+8196>>3];
    }
  }

  for (v=1; v<199; v+=2) {
    d=dest+v*320;
    if (v==1 || v==197)
      for (u=0; u<320; u++, d++) d[0]=d[-320]+d[320]+1>>1;
    else
      for (u=0; u<320; u++, d++) d[0]=rtbl[(d[-320]+d[320])*5-(d[-960]+d[960])+8196>>3];
  }
  d=dest+199*320;
  for (u=0; u<320; u++, d++) *d=0;
}



void teegalaxi(int *dest, float time, float xpos, float ypos, float zpos) {
  float camera[3][3];
//  static dot newdots[dis];
  int x, y, i, c;
  int px, py, pz;
  float bx, by, bz;
  float xx, yy, zz, x2, y2, z2, f;
  int *d2;
  int *s2;
  int xf, yf, xyf;
  dot *dp;

  px=xpos*500;
  py=ypos*500;
  pz=zpos*500;
  teematriisi2(camera, 0-xpos, 0-ypos, 0-zpos);
  bx=-camera[0][0]*px-camera[0][1]*py-camera[0][2]*pz;
  by=-camera[1][0]*px-camera[1][1]*py-camera[1][2]*pz;
  bz=-camera[2][0]*px-camera[2][1]*py-camera[2][2]*pz;


/*    dots[i].i=(i*i+1000000)*costbl[dots[i].rnd+c&4095]/500;
    dots[i].y=0;
    dots[i].z=(i*i+1000000)*sintbl[dots[i].rnd+c&4095]/500;*/
//    c=(3000000.0/fsqrt(stars[i].r))*time;
//    x2=stars[i].r*costbl[stars[i].a+c&4095]*0.002-px;
//    y2=stars[i].h-py;
//    z2=stars[i].r*sintbl[stars[i].a+c&4095]*0.002-pz;
  camera[2][0]*=1/(100.0*16.0);
  camera[2][1]*=1/(100.0*16.0);
  camera[2][2]*=1/(100.0*16.0);
  bz*=1/(100.0*16.0);
  camera[0][0]+=camera[2][0]*80*16.0;
  camera[0][1]+=camera[2][1]*80*16.0;
  camera[0][2]+=camera[2][2]*80*16.0;
  bx+=bz*80*16.0;
  camera[1][0]+=camera[2][0]*50*16.0;
  camera[1][1]+=camera[2][1]*50*16.0;
  camera[1][2]+=camera[2][2]*50*16.0;
  by+=bz*50*16.0;
  for (dp=dots; dp<dots+dis; dp++) {
    zz=1.0/(camera[2][0]*dp->x+camera[2][1]*dp->y+camera[2][2]*dp->z+bz);
    if (*(int*)&zz>0) {
      qftolp(&x, (camera[0][0]*dp->x+camera[0][1]*dp->y+camera[0][2]*dp->z+bx)*zz);
      qftolp(&y, (camera[1][0]*dp->x+camera[1][1]*dp->y+camera[1][2]*dp->z+by)*zz);
//      if (x>=0 && x<158*16 && y>=0 && y<98*16) {
      if ((unsigned)x<158*16 && (unsigned)y<98*16) {
        d2=dest+(y>>4)*160+(x>>4); s2=interpre+(y&15)*64+(x&15)*4;
        d2[0  ]+=s2[3]; d2[1  ]+=s2[2];
        d2[160]+=s2[1]; d2[161]+=s2[0];
      }
    }
  }
}

void teespiraali(char *dest, float time, float xpos, float ypos, float zpos) {
  float camera[3][3];
//  static dot newdots[dis];
  float xx, yy, zz, x2, y2, z2, f;
  int x, y, i, c;
  int px, py, pz;

  px=xpos*16384;
  py=ypos*16384;
  pz=zpos*16384;
  teematriisi2(camera, 0-xpos, 0-ypos, 0-zpos);

  for (i=0; i<dis; i++) {
    c=(dis-i)*(3000.0/dis)*time;
    f=i*(3000.0/dis);
    x2=((f*f+1000000)*costbl[dots[i].rnd+c&4095]*0.002)-px;
    y2=((f*f+1000000)*sintbl[dots[i].rnd+c&4095]*0.002)-py;
    z2=((dots[i].rnd-16384)*1.5)-pz;

    zz=camera[2][0]*x2+camera[2][1]*y2+camera[2][2]*z2;
    if (zz>0) {
      xx=camera[0][0]*x2+camera[0][1]*y2+camera[0][2]*z2;
      yy=camera[1][0]*x2+camera[1][1]*y2+camera[1][2]*z2;
      zz=1.0/zz;
      x=xx*zz*409600+80*4096;
      y=yy*zz*409600+50*4096;
      if (x>0 && x<158*4096 && y>0 && y<98*4096)
        dotti(dest, x, y, hmx(800000*zz));
    }
  }
}

void blur(char *dest, int *src) {
//  static int src[16000];
  int i;
  for (i=0; i<16000; i++) dest[i]=rtbl[(src[i]+128>>8)+1024];
  for (i=1; i<16000; i++) src[i]=src[i-1]+(src[i]-src[i-1]+4>>3);
  for (i=160; i<16000; i++) src[i]=src[i-160]+(src[i]-src[i-160]+4>>3);
  for (i=15998; i>=0; i--) src[i]=src[i+1]+(src[i]-src[i+1]+4>>3);
  for (i=15838; i>=0; i--) src[i]=src[i+160]+(src[i]-src[i+160]+4>>3);
  for (i=0; i<16000; i++) dest[i]=rtbl[dest[i]+(src[i]+128>>8)+1024];
}

/*
void main() {
  int x, y;
  float xx, yy, zz;

  mode(0x13);
  for (x=0; x<256; x++) {
    ulos(0x3c8, x);
    ulos(0x3c9, x/4);
    ulos(0x3c9, x/4);
    ulos(0x3c9, x/4);
  }

  precalculatemappi();
  precalculoitunneli(0.5, 256);
  precalcigalaxi();

  for (x=0; x<4096; x++) sintbl[x]=fsin(x*3.14159265358979323846264/2048.0);
  for (x=0; x<4096; x++) costbl[x]=fcos(x*3.14159265358979323846264/2048.0);

  for (x=0; x<1024; x++) rtbl[x]=0;
  for (x=1024; x<1280; x++) rtbl[x]=x-1024;
  for (x=1280; x<2048; x++) rtbl[x]=255;

  x=1;
  while (*(short*)1050==*(short*)1052) {
    fill32(buf160, 4000, 0);
    if (0) {
      xx=fsin(x*0.0210);
      yy=fcos(x*0.0144);
      zz=fsin(x*0.0373);
      teespiraali(buf160, x/666.0, xx, yy, zz);
      suurenna(buf160, dbuf, 160, 100);
      teevaloefekti(dbuf, xx*0.5, yy*0.5, zz*0.5);
    } else {
      xx=fsin(x*0.0316)*(5-x*0.01);
      yy=-1;
      zz=fcos(x*0.0537)*(5-x*0.01);
      teegalaxi(buf160, x/2000.0+0.5, xx, yy, zz);
      blur(buf160);
      suurenna(buf160, dbuf, 160, 100);
      teevalo(dbuf, x, 4096/(xx*xx+yy*yy+zz*zz));
    }
    move32(dbuf, videomem, 16000);
    x+=1;
  }
  mode(0x3);
}
*/

