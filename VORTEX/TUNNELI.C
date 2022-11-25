#include "tinymath.h"
#include "tinyheap.h"
#include "tinyfile.h"



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
//extern ushort valospr162[256*256];
//extern char valospr16[256*256];
extern char sprite[256*256];
extern ushort spritepal[64*256];



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

int hm(int k) { return k<255?k:255; }
int hmx(int k) { return k<255?(k>0?k:0):255; }
int hm2(int k) { return k<63?k:63; }
int hm3(int k) { return k<127?k:127; }




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
}



void precalculatemappi() {
  int x, y;
  float i, j;
  int u, v;

  for (x=0; x<256; x++) {
    sint1[x]=(fsin(x*pi/256)+fsin(x*pi/128))*64;
    sint2[x]=(fcos(x*pi/128)-fsin(x*pi/256))*64;
    sint3[x]=(fcos(x*pi/64)+fcos(x*pi/128))*64;
  }


  for (y=0; y<256; y++) for (x=0; x<256; x++) mappi2[(y<<8)+x]=rand();
  for (i=0; i<32; i++) {
    for (y=0; y<256; y++) for (x=0; x<256; x++) {
      mappi2[(y<<8)+x]=(mappi2[(y<<8)+x]+mappi2[(y+1<<8&0xff00)+(x+1&0xff)]
                       +mappi2[(y<<8)+(x+1&0xff)]+mappi2[(y+1<<8&0xff00)+x])/4;
    }
  }
/*
  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    u=x*2;
    v=y*2;
    mappi[v*256+u    ]=acidmap[y*128+x]*2+1;
    mappi[v*256+u+1  ]=(acidmap[y*128+x]+acidmap[y*128+(x+1)%128])+1;
    mappi[v*256+u+256]=(acidmap[y*128+x]+acidmap[(y+1)%128*128+x])+1;
    mappi[v*256+u+257]=(acidmap[y*128+x]+acidmap[y*128+(x+1)%128]+
                        acidmap[(y+1)%128*128+x]+acidmap[(y+1)%128*128+(x+1)%128])/2+1;
  }
*/
  for (y=0; y<256; y++) for (x=0; x<256; x++) {

    u=qftol(10000/fsqrt((128-y)*(128-y)+(128-x)*(128-x)+0.0001));
    v=qftol(fpatan(128-y, 128-x)/pi*256);

    j=100000/((128-y)*(128-y)+(128-x)*(128-x)+0.0001);
    if (j>255) j=255;
    i=mappi[(v<<8)+u&65535]-fsqrt(fabs(140-u)*fabs(140-u)*3);
    if (i<0) i=0;
//    i+=j;
    if (i>255) i=255;
    envi[y*256+x]=((int)i>>3)+((int)j>>3)*2113;
  }

  for (y=0; y<256; y++) for (x=0; x<256; x++) {
    hfield2[y*256+x]=((fsin(fsin(pi*(x+y)/128)+
                     pi*(x-y)/128)*2.0)-
                     fsin(pi*(x-y)/128)*2.0)*2048.0+
                     (mappi2[y*256+x]-128)*4;
  }


}

void teetunneli2(ushort *dest, float rotx, float roty, float rotz,
                               float posx, float posy, float posz) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, t;
  int x, y;
  float perx, pery;
  float ax, bx, cx;
  int xypos;
//  float r;

  teematriisi(camera, rotx, roty, rotz);

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
      qftolp(pienix+xypos, fpatan(px, py)/pi*256*65536);
      qftolp(pieniz+xypos, pz*64*65536);
      qftolp(varjot+xypos, 65536*255-t*65536*128);
      if (varjot[xypos]>250*65536) varjot[xypos]=250*65536;
      if (varjot[xypos]<0) varjot[xypos]=0;
    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
  for (x=0; x<64000; x++) {
    dest[x]=(mappi[(isox[x]<<8)+isoz[x]]*valo[x]>>11);
//    zbuf[x]=1/isoz[x]*;
  }
//  for (x=0; x<64000; x++) dest[x]=mappi[(isox[x]<<8)+isoz[x]];

}

void teeisopallo(ushort *dest, short *hfield,
                 float posx, float posy, float posz,
                 float tgtx, float tgty, float tgtz) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, t;
  int x, y, u, v;
  float perx, pery;
  float ax, bx, cx;
  int xypos;
  float r;

  teematriisi2(camera, tgtx-posx, tgty-posy, tgtz-posz);

  pery=1/12.5;
  perx=1/15.0;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++) {
      suunta[0]=camera[2][0] - camera[1][0]*(y-13)*pery - camera[0][0]*(x-20)*perx;
      suunta[1]=camera[2][1] - camera[1][1]*(y-13)*pery - camera[0][1]*(x-20)*perx;
      suunta[2]=camera[2][2] - camera[1][2]*(y-13)*pery - camera[0][2]*(x-20)*perx;

      ax=(suunta[0]*suunta[0])+(suunta[1]*suunta[1])+(suunta[2]*suunta[2]);
      bx=(suunta[0]*posx)+(suunta[1]*posy)+(suunta[2]*posz);
      cx=(posx*posx)+(posy*posy)+(posz*posz)-3;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];



//      u=fabs(py*64)*65536.0;
//      v=fabs(pz*64)*65536.0;
      u=fpatan(px, py)*64*65536;
      v=fpatan(pz, fsqrt(px*px+py*py))*64*65536;
      r=1+hfield[(v>>8&0xff00)+(u>>16&0xff)]/16384.0;



      ax=(suunta[0]*suunta[0])+(suunta[1]*suunta[1])+(suunta[2]*suunta[2]);
      bx=(suunta[0]*posx)+(suunta[1]*posy)+(suunta[2]*posz);
      cx=(posx*posx)+(posy*posy)+(posz*posz)-r*r-3;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];


      xypos=y*41+x;
      qftolp(pienix+xypos, fpatan(px, py)/pi*256*65536);
      qftolp(pieniz+xypos, fpatan(pz, fsqrt(px*px+py*py))*128*65536);
      qftolp(varjot+xypos, 65536*255-t*65536*128);
      if (varjot[xypos]>250*65536) varjot[xypos]=250*65536;
      if (varjot[xypos]<0) varjot[xypos]=0;
    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
  for (x=0; x<64000; x++) {
    dest[x]=(mappi[(isox[x]<<8)+isoz[x]]*valo[x]>>11);
//    zbuf[x]=1/isoz[x]*;
  }
//  for (x=0; x<64000; x++) dest[x]=mappi[(isox[x]<<8)+isoz[x]];

}


void plussprite2(int x1, int y1, int x2, int y2, int leveys, int korkeus,
                 char *source, ushort *source2, ushort *dest) {
  int y, x1b, x2b, y1b, y2b, v, i, j, u16, u16inc;
  ushort *d2, *d2m, *d2m2;
  char *s2;
  ushort *s3;

  if (x1<0)   x1b=0;   else x1b=x1;
  if (x2>320) x2b=320; else x2b=x2;
  if (y1<0)   y1b=0;   else y1b=y1;
  if (y2>200) y2b=200; else y2b=y2;
  if (x1b>x2b) return;
  if (y1b>y2b) return;
  u16inc=65536.0*leveys/(x2-x1);
  for (y=y1b; y<y2b; y++) if (y>=0 && y<200) {
    v=korkeus*(y-y1)/(y2-y1);
    s2=source+v*leveys;
    s3=source2+v*leveys;
    d2=dest+y*320; d2m=d2+x2b; d2+=x1b; d2m2=d2m-4;
    u16=(x1b-x1)*u16inc;
    for (; d2<d2m2; d2+=4) {
      i=(d2[0]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[0]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[0]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
      i=(d2[1]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[1]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[1]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
      i=(d2[2]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[2]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[2]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
      i=(d2[3]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[3]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[3]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
    }
    for (; d2<d2m; d2++, u16+=u16inc) {
//      i=*d2+s2[u16>>16]; *d2=i|(i&256)-((i&256)>>8);
//      i=(*d2>>11)+s2[u16>>16]; if (i>31) i=31; *d2=i<<11;
      i=(d2[0]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[0]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[0]=(i<<11)+j+(s3[u16>>16]&0x7e0);
      //if (i>255) *d2=255; else *d2=i;
    }
  }
}


void plussprite3(int x1, int y1, int x2, int y2, int leveys, int korkeus,
                 char *source, ushort *sourcepal, ushort *dest, int br) {
  int y, x1b, x2b, y1b, y2b, v, i, j, u16, u16inc;
  ushort *d2, *d2m, *d2m2;
  char *s2;
  ushort *sp;
  char r, g, b;

  sp=sourcepal+br*256;
  if (x1<0)   x1b=0;   else x1b=x1;
  if (x2>320) x2b=320; else x2b=x2;
  if (y1<0)   y1b=0;   else y1b=y1;
  if (y2>200) y2b=200; else y2b=y2;
  if (x1b>x2b) return;
  if (y1b>y2b) return;
  u16inc=65536.0*leveys/(x2-x1);
  for (y=y1b; y<y2b; y++) if (y>=0 && y<200) {
    v=korkeus*(y-y1)/(y2-y1);
    s2=source+v*leveys;
    d2=dest+y*320; d2m=d2+x2b; d2+=x1b; d2m2=d2m-4;
    u16=(x1b-x1)*u16inc;
    for (; d2<d2m2; d2+=4) {
/*      i=(d2[0]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[0]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[0]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
      i=(d2[1]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[1]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[1]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
      i=(d2[2]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[2]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[2]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc;
      i=(d2[3]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[3]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[3]=(i<<11)+j+(s3[u16>>16]&0x7e0); u16+=u16inc; */

      r=(d2[0]>>11)+(sp[s2[u16>>16]&255]>>11); if (r>31) r=31;
      g=(d2[0]>>5&63)+(sp[s2[u16>>16]&255]>>5&63); if (g>63) g=63;
      b=(d2[0]&31)+(sp[s2[u16>>16]&255]&31); if (b>31) b=31;
      d2[0]=(r<<11)+(g<<5)+b; u16+=u16inc;

      r=(d2[1]>>11)+(sp[s2[u16>>16]&255]>>11); if (r>31) r=31;
      g=(d2[1]>>5&63)+(sp[s2[u16>>16]&255]>>5&63); if (g>63) g=63;
      b=(d2[1]&31)+(sp[s2[u16>>16]&255]&31); if (b>31) b=31;
      d2[1]=(r<<11)+(g<<5)+b; u16+=u16inc;

      r=(d2[2]>>11)+(sp[s2[u16>>16]&255]>>11); if (r>31) r=31;
      g=(d2[2]>>5&63)+(sp[s2[u16>>16]&255]>>5&63); if (g>63) g=63;
      b=(d2[2]&31)+(sp[s2[u16>>16]&255]&31); if (b>31) b=31;
      d2[2]=(r<<11)+(g<<5)+b; u16+=u16inc;

      r=(d2[3]>>11)+(sp[s2[u16>>16]&255]>>11); if (r>31) r=31;
      g=(d2[3]>>5&63)+(sp[s2[u16>>16]&255]>>5&63); if (g>63) g=63;
      b=(d2[3]&31)+(sp[s2[u16>>16]&255]&31); if (b>31) b=31;
      d2[3]=(r<<11)+(g<<5)+b; u16+=u16inc;

    }
    for (; d2<d2m; d2++, u16+=u16inc) {
//      i=(d2[0]>>11)+(s2[u16>>16]); if (i>31) i=31; j=(d2[0]&0x1f)+(s3[u16>>16]&0x1f); if (j>31) j=31; d2[0]=(i<<11)+j+(s3[u16>>16]&0x7e0);
    }
  }
}



void teetunneli3(ushort *dest, short *hfield,
                 float rotx, float roty, float rotz,
                 float posx, float posy, float posz, int br, int br2) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, dx, dy, dz, t;
  int x, y;
  float perx, pery;
  float ax, bx, cx;
  int xypos;
  int u,v, test, test2;
  float r;

  teematriisi(camera, rotx, roty, rotz);

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


      u=(fpatan(px, py)/pi+1)*128.0*65536.0;
      v=fabs(pz*64)*65536.0;
      r=1+hfield[(v>>8&0xff00)+(u>>16&0xff)]/16384.0;

      ax=(suunta[0]*suunta[0])+(suunta[1]*suunta[1]);
      bx=(suunta[0]*posx)+(suunta[1]*posy);
      cx=(posx*posx)+(posy*posy)-r*r;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

//      dx=px; dy=py; dz=0;
      dx=px*(1+(hfield[(v-65536>>8&0xff00)+(u>>16&0xff)]-hfield[(v+65536>>8&0xff00)+(u>>16&0xff)])/1024.0);
      dy=py*(1+(hfield[(v-65536>>8&0xff00)+(u>>16&0xff)]-hfield[(v+65536>>8&0xff00)+(u>>16&0xff)])/1024.0);
      dz=(hfield[(v>>8&0xff00)+(u-65536>>16&0xff)]-hfield[(v>>8&0xff00)+(u+65536>>16&0xff)])/512.0;
      r=1/fsqrt(dx*dx+dy*dy);

      u=(fpatan(px, py)/pi+1)*128.0*65536.0;
      v=fabs(pz*64)*65536.0;

      xypos=y*41+x;
//      if (mdata->curord>10)
//        test=65536*br-((px*px+py*py+pz*pz)*65536);
        test=br*32768/*fsqrt(px*px+py*py+pz*pz)*/*fsqrt((dx*px+dy*py+dz*pz)/(r*(px*px+py*py+pz*pz)));
//        else test=0;
      if (test>60*65536) test=60*65536;
      if (test<32768) test=32768;
      test2=(12*65536.0-t*65536*6.0)*br2/64;
      if (test2<32768) test2=32768;
      pienix[xypos]=u;//fpatan(px, py)/pi*1*128*65536);
      pieniz[xypos]=v;//256*65536-pz*64*65536);
      qftolp(varjot+xypos, test);
      qftolp(varjot2+xypos, test2);
    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
  interpoloi3(varjot2, valo2);
  for (x=0; x<64000; x++) {
    test=mappi2[(isox[x]<<8)+isoz[x]];
    dest[x]=((test*valo[x]>>8)<<11)+(test*valo2[x]>>7);
/*    rt[x]=hm(test*valo[x]>>8);
    bt[x]=hm(test*valo2[x]>>7); */
  }
//  teergbkuva();
//  for (x=0; x<64000; x++) dest[x]=mappi[(isox[x]<<8)+isoz[x]];
}


void teeflare(ushort *dest, float rotx, float roty, float rotz,
              float posx, float posy, float posz, int br) {
  float camera[3][3];
  int uusix, uusiy, uusiz;
  int x, y, z, i, x1, y1, x2, y2;
  int px, py, pz;

  px=posx*16384;
  py=posy*16384;
  pz=posz*16384;

  teematriisi(camera, rotx, roty, rotz);

  uusix=-camera[0][0]*px-camera[0][1]*py-camera[0][2]*pz;
  uusiy=-camera[1][0]*px-camera[1][1]*py-camera[1][2]*pz;
  uusiz=-camera[2][0]*px-camera[2][1]*py-camera[2][2]*pz;


  x=uusix*128/(uusiz+0.001);
  y=uusiy*128/(uusiz+0.001);
  z=64000*48/(uusiz+0.05);
  x1=x-z;
  y1=y-z;
  x2=x+z;
  y2=y+z;
  if (uusiz>0) plussprite3(x1+160 ,y1+100 ,x2+160, y2+100, 256, 256, sprite, spritepal, dest, br);
  //plussprite2(x1+160 ,y1+100 ,x2+160, y2+100, 256, 256, valospr16, valospr162, dest);
//  if (x>0 && x<320 && y>0 && y<200 && uusiz>0) dest[y*320+x]=63<<11;
}

void teefield(int joo, short *dest) {
  int x,y;

  for (y=0; y<256; y++) for (x=0; x<256; x++) {
    dest[(y<<8)+x]=(sint1[y-x+joo&255]+sint2[sint3[x-joo&255]-y&255]-sint3[y+joo*2&255])*32;
  }
}


void teetaso2(char *dest, short *hfield,
              float rotx, float roty, float rotz,
              float posx, float posy, float posz) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, t;
  int x, y;
  float perx, pery;
  float ax, bx, cx;
  int xypos;
  int u,v;
  float r;

  teematriisi(camera, rotx, roty, rotz);

  pery=1/12.5;
  perx=1/15.0;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++) {
      suunta[0]=camera[2][0] + camera[1][0]*(y-13)*pery + camera[0][0]*(x-20)*perx;
      suunta[1]=camera[2][1] + camera[1][1]*(y-13)*pery + camera[0][1]*(x-20)*perx;
      suunta[2]=camera[2][2] + camera[1][2]*(y-13)*pery + camera[0][2]*(x-20)*perx;

      ax=suunta[0]*suunta[0];
      bx=suunta[0]*posx;
      cx=posx*posx-1;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

/*
      u=fabs(py*128);
      v=fabs(pz*128);
      r=1+hfield[(v<<8)+u&65535]/1024.0;
*/
      u=fabs(py*64)*65536.0;
      v=fabs(pz*64)*65536.0;
      r=1+hfield[(v>>8&0xff00)+(u>>16&0xff)]/16384.0;


      ax=suunta[0]*suunta[0];
      bx=suunta[0]*posx;
      cx=posx*posx-r*r;


      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;

      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

      xypos=y*41+x;
      qftolp(pienix+xypos, py*64*65536);
      qftolp(pieniz+xypos, pz*64*65536);
      qftolp(varjot+xypos, 64*65536/t-32*65536);
      if (varjot[xypos]>250*65536) varjot[xypos]=250*65536;
      if (varjot[xypos]<8*65536) varjot[xypos]=8*65536;

    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
/*  for (x=0; x<64000; x++) {
   if (reunat[x]==1) dest[x]=hm3(mappi2[(isox[x]<<8)+isoz[x]]*valo[x]>>6);
   else dest[x]=reunat[x];
  } */

  for (x=0; x<64000; x++) dest[x]=hm(mappi2[(isox[x]<<8)+isoz[x]]*valo[x]>>6);
}



void teetaso3(ushort *dest, short *hfield,
              float rotx, float roty, float rotz,
              float posx, float posy, float posz) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, t;
  int x, y;
  float perx, pery;
  float ax, bx, cx;
  int xypos;
  int u, v, test;
  float r;

  teematriisi(camera, rotx, roty, rotz);

  pery=1/12.5;
  perx=1/15.0;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++) {
      suunta[0]=camera[2][0] + camera[1][0]*(y-13)*pery + camera[0][0]*(x-20)*perx;
      suunta[1]=camera[2][1] + camera[1][1]*(y-13)*pery + camera[0][1]*(x-20)*perx;
      suunta[2]=camera[2][2] + camera[1][2]*(y-13)*pery + camera[0][2]*(x-20)*perx;

      ax=suunta[0]*suunta[0];
      bx=suunta[0]*posx;
      cx=posx*posx-1;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

/*
      u=fabs(py*128);
      v=fabs(pz*128);
      r=1+hfield[(v<<8)+u&65535]/1024.0;
*/
      u=fabs(py*32)*65536.0;
      v=fabs(pz*32)*65536.0;
      r=1+hfield[(v>>8&0xff00)+(u>>16&0xff)]/16384.0;


      ax=suunta[0]*suunta[0];
      bx=suunta[0]*posx;
      cx=posx*posx-r*r;


      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;

      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

      xypos=y*41+x;
      qftolp(pienix+xypos, py*64*65536);
      qftolp(pieniz+xypos, pz*64*65536);

      qftolp(varjot+xypos, 64*65536-t*t*16*65536);
      if (varjot[xypos]>250*65536) varjot[xypos]=250*65536;
      if (varjot[xypos]<65536) varjot[xypos]=65536;

    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
/*  for (x=0; x<64000; x++) {
   if (reunat[x]==1) dest[x]=hm3(mappi2[(isox[x]<<8)+isoz[x]]*valo[x]>>6);
   else dest[x]=reunat[x];
  } */

  for (x=0; x<64000; x++) {
    test=mappi[(isox[x]<<8)+isoz[x]];
    dest[x]=((test*valo[x]>>8)<<11);

//    dest[x]=hm(mappi2[(isox[x]<<8)+isoz[x]]*valo[x]>>6);
  }
}


void teegridisysteemi(char *dest, short *hfield,
                      float posx, float posy, float posz,
                      float tgtx, float tgty, float tgtz,
                      float xsize, float ysize, float zsize) {
  float camera[3][3];
  float suunta[3];
  float px, py, pz, dx, dy, dz, t;
  int x, y, u, v, c;
  float perx, pery;
  float ax, bx, cx;
  int xypos;
  float r;

  teematriisi2(camera, tgtx-posx, tgty-posy, tgtz-posz);
  if (xsize<0.1) xsize=0.1;
  if (ysize<0.1) ysize=0.1;
  if (zsize<0.1) zsize=0.1;

  pery=1/12.5;
  perx=1/15.0;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++) {
      suunta[0]=camera[2][0] - camera[1][0]*(y-13)*pery - camera[0][0]*(x-20)*perx;
      suunta[1]=camera[2][1] - camera[1][1]*(y-13)*pery - camera[0][1]*(x-20)*perx;
      suunta[2]=camera[2][2] - camera[1][2]*(y-13)*pery - camera[0][2]*(x-20)*perx;

      ax=(suunta[0]*suunta[0]*xsize)+(suunta[1]*suunta[1]*ysize)+(suunta[2]*suunta[2]*zsize);
      bx=(suunta[0]*posx*xsize)+(suunta[1]*posy*ysize)+(suunta[2]*posz*zsize);
      cx=(posx*posx*xsize)+(posy*posy*ysize)+(posz*posz*zsize)-1;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

      u=fpatan(px*xsize*0.5, py*ysize*0.5)/xsize/ysize/pi*256*65536;
      v=(fpatan(pz*zsize*0.5, fsqrt(px*px*xsize*0.5+py*py*ysize*0.5))-pi/2)/zsize/fsqrt(xsize*xsize+ysize*ysize)*128*65536;
      r=1+hfield[(v>>8&0xff00)+(u>>16&0xff)]/4096.0;

      ax=(suunta[0]*suunta[0]*xsize)+(suunta[1]*suunta[1]*ysize)+(suunta[2]*suunta[2]*zsize);
      bx=(suunta[0]*posx*xsize)+(suunta[1]*posy*ysize)+(suunta[2]*posz*zsize);
      cx=(posx*posx*xsize)+(posy*posy*ysize)+(posz*posz*zsize)-r*r-1;

      t=(fsqrt(bx*bx-ax*cx)-bx)/ax;
      px=posx+t*suunta[0];
      py=posy+t*suunta[1];
      pz=posz+t*suunta[2];

      xypos=y*41+x;
      dx=px*(1+(hfield[(v-65536>>8&0xff00)+(u>>16&0xff)]-hfield[(v+65536>>8&0xff00)+(u>>16&0xff)])/512.0);
      dy=py*(1+(hfield[(v-65536>>8&0xff00)+(u>>16&0xff)]-hfield[(v+65536>>8&0xff00)+(u>>16&0xff)])/512.0);
      dz=(hfield[(v>>8&0xff00)+(u-65536>>16&0xff)]-hfield[(v>>8&0xff00)+(u+65536>>16&0xff)])/256.0;
      r=1/fsqrt(dx*dx+dy*dy);
      c=64*65536*fsqrt((dx*px+dy*py+dz*pz)/(r*(px*px+py*py+pz*pz)));
      c=c/(t*t);
      if (c>250*65536) c=250*65536; if (c<65536) c=65536;

      qftolp(pienix+xypos, fpatan(px*xsize, py*ysize)/xsize/ysize/pi*256*65536);
      qftolp(pieniz+xypos, (fpatan(pz*zsize, fsqrt(px*px*xsize*xsize+py*py*ysize*ysize))-pi/2)/zsize/fsqrt(xsize*xsize+ysize*ysize)*128*65536);
//      qftolp(varjot+xypos, 65536*255-t*65536*100);
      qftolp(varjot+xypos, c);
/*      if (varjot[xypos]>250*65536) varjot[xypos]=250*65536;
      if (varjot[xypos]<0) varjot[xypos]=0; */
    }
  interpoloi2(pienix, isox);
  interpoloi3(pieniz, isoz);
  interpoloi3(varjot, valo);
  for (x=0; x<64000; x++) {
    dest[x]=(mappi[(isox[x]<<8)+isoz[x]]*valo[x]>>8);
//    zbuf[x]=1/isoz[x]*;
  }
//  for (x=0; x<64000; x++) dest[x]=mappi[(isox[x]<<8)+isoz[x]];

}




