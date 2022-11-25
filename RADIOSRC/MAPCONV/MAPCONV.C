#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib2\bugs.h>
#include <lib2\mem.h>
#include "video.h"
#include <math.h>

char *obuf;
int obit=0;
static int xx[1024], yy[1024];
unsigned char flip4[]="\x00\x40\x10\x50\x04\x44\x14\x54\x01\x41\x11\x51\x05\x45\x15\x54";

char *loadtga(char *fname) {
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
  int size, x, y, x0, y0, x1, y1, xf, yf, i, j, k, pitch;
  char *tmp, *tmp2;
  FILE *fp=fopen(fname, "rb");
  fread(&hdr, 1, 18, fp);
  fseek(fp, hdr.idlen, SEEK_CUR);
  pitch=(hdr.bpp+7>>3)*hdr.width;
  size=pitch*hdr.height; tmp=getmem(size); if (!tmp) erreur("memory!");
  fread(tmp, 1, size, fp);
  fclose(fp);
  if (hdr.bpp==24) {
    for (i=0; i<size; i+=3) j=tmp[i], tmp[i]=tmp[i+2], tmp[i+2]=j;
  } else if (hdr.bpp==16) {
    for (i=0; i+i<size; i++) j=((short*)tmp)[i], ((short*)tmp)[i]=(j<<11&0xf800)+(j&0x07e0)+(j>>11&0x001f);
  }
  if (~hdr.flags&0x20) {
    for (y=0; y+y<hdr.height; y++) for (x=0; x<pitch; x++)
      j=tmp[y*pitch+x], tmp[y*pitch+x]=tmp[(hdr.height-1-y)*pitch+x], tmp[(hdr.height-1-y)*pitch+x]=j;
  }
  return tmp;
}
int dif5(int a, int b, int c, int d, int e) {
  int f, g;
  f=a>b?a:b; f=f>c?f:c; f=f>d?f:d; f=f>e?f:e;
  g=a<b?a:b; g=g<c?g:c; g=g<d?g:d; g=g<e?g:e;
  return f-g;
}
/*
void wltx(int *p, int cnt) {
  int i, j, c05=cnt>>1, *q, *r;
  int *lo=malloc(cnt*4), *hi=lo+c05;
  for (i=0; i<c05; i++) lo[i]=p[i*2]+p[i*2+1]+1>>1;
  for (i=0; i<c05; i++) hi[i]=(p[i*2]-p[i*2+1])*16;
  for (i=0; i<1    ; i++) hi[i]-=(lo[i  ]-lo[i+1])*8;
  for (i=1; i<c05-1; i++) hi[i]-=(lo[i-1]-lo[i+1])*4;
  for (   ; i<c05  ; i++) hi[i]-=(lo[i-1]-lo[i  ])*8;
  for (i=2; i<c05-2; i++) hi[i]-=((lo[i-1]-lo[i+1])*2-(lo[i-2]-lo[i+2]))*3>>2;
  for (i=0; i<c05; i++) hi[i]=hi[i]+16>>5;
  for (i=0; i<c05; i+=2) hi[i]=-hi[i];
  for (i=0; i<cnt; i++) p[i]=lo[i];
  free(lo);
}
void wltx1(int *p, int cnt) {
  int *lo, *hi;
  int i, j, k, *q, c05=cnt>>1;
  lo=malloc(cnt*4); hi=lo+c05;
  for (i=0; i<cnt; i++) lo[i]=p[i];
  for (i=0; i<c05; i+=2) hi[i]=-hi[i];
  for (i=0; i<c05; i++) hi[i]*=32;
  for (i=0; i<1    ; i++) hi[i]+=(lo[i  ]-lo[i+1])*8;
  for (   ; i<c05-1; i++) hi[i]+=(lo[i-1]-lo[i+1])*4;
  for (   ; i<c05  ; i++) hi[i]+=(lo[i-1]-lo[i  ])*8;
  for (i=2; i<c05-2; i++) hi[i]+=((lo[i-1]-lo[i+1])*2-(lo[i-2]-lo[i+2]))*3>>2;
  for (i=0; i<c05; i++) p[i*2]=lo[i]*32+hi[i]+16>>5, p[i*2+1]=lo[i]*32-hi[i]+16>>5;
  free(lo);
}
void waveline(int *p, int d, int cnt) {
  int *tmp=malloc(4096*4);
  int i, j;
  for (i=0; i<cnt; i++) tmp[i]=p[xx[i]*d];
  wltx(tmp, cnt);
  //for (i=cnt>>1; i<cnt; i++) if (abs(tmp[i])<3000) tmp[i]=0;
  for (i=0; i<cnt; i++) p[xx[i]*d]=tmp[i];
  free(tmp);
}
void unwaveline(int *p, int d, int cnt) {
  int *tmp=malloc(4096*4);
  int i, j;
  for (i=0; i<cnt; i++) tmp[i]=p[xx[i]*d];
  wltx1(tmp, cnt);
  for (i=0; i<cnt; i++) p[xx[i]*d]=tmp[i];
  free(tmp);
}
*/
#define im1 (((i)&0x55555|((i)|0x55555)+(-2&0xaaaaa)&0xaaaaa)&c05-1)
#define ip1 (((i)&0x55555|((i)|0x55555)+2&0xaaaaa)&c05-1)
#define im2 (((i)&0x55555|((i)|0x55555)+(-8&0xaaaaa)&0xaaaaa)&c05-1)
#define ip2 (((i)&0x55555|((i)|0x55555)+8&0xaaaaa)&c05-1)
float flimit(float x, float a, float b) { return x<a?a:x>b?b:x; }
void wltx(int *p, int c05) {
  int i, j, *q, *r;
  int *lo=malloc(c05*8), *hi=lo+c05;
  for (i=0; i<c05; i++) lo[i]=p[i*2]+p[i*2+1]+1>>1;
  for (i=0; i<c05; i++) hi[i]=(p[i*2]-p[i*2+1])*16;
  for (i=0; i<c05; i++) hi[i]-=(lo[im1]-lo[ip1])*2;
  if (c05&0xaaaaaaaa) for (i=0; i<c05; i++) hi[i]-=(lo[i]-lo[ip1])*4;
    else for (i=0; i<c05; i++) hi[i]-=(lo[im1]-lo[i])*4;
  //for (i=2; i<c05-2; i++) hi[i]-=((lo[im1]-lo[ip1])*2-(lo[im2]-lo[ip2]))*3>>2;
  for (i=0; i<c05; i++) hi[i]=hi[i]+16>>5;
  //for (i=0; i<c05; i+=4) hi[i]*=-1, hi[i+1]*=-1;
  memcpy(p, lo, c05*8);
  free(lo);
}
void wltx1(int *p, int c05) {
  int *lo, *hi;
  int i, j, k, *q;
  lo=malloc(c05*8); hi=lo+c05;
  memcpy(lo, p, c05*8);
  //for (i=0; i<c05; i+=4) hi[i]*=-1, hi[i+1]*=-1;
  for (i=0; i<c05; i++) hi[i]*=32;
  for (i=0; i<c05; i++) hi[i]+=(lo[im1]-lo[ip1])*2;
  if (c05&0xaaaaaaaa) for (i=0; i<c05; i++) hi[i]+=(lo[i]-lo[ip1])*4;
    else for (i=0; i<c05; i++) hi[i]+=(lo[im1]-lo[i])*4;
  //for (i=2; i<c05-2; i++) hi[i]+=((lo[im1]-lo[ip1])*2-(lo[im2]-lo[ip2]))*3>>2;
  for (i=0; i<c05; i++) p[i*2]=lo[i]*32+hi[i]+16>>5, p[i*2+1]=lo[i]*32-hi[i]+16>>5;
  free(lo);
}

void screw(int *p, int s) {
  int *q, a, b, c, d;
  for (q=p; q<p+s; q+=4) {
    a=q[0]+q[1]; b=q[0]-q[1];
    c=q[2]+q[3]; d=q[2]-q[3];
    q[0]=a+c>>1; q[2]=a-c>>1;
    q[1]=b+d>>1; q[3]=b-d>>1;
  }
}
void unscrew(int *p, int s) {
  int *q, a, b, c, d;
  for (q=p; q<p+s; q+=4) {
    a=q[0]*2+q[1]*2; b=q[0]*2-q[1]*2;
    c=q[2]*2+q[3]*2; d=q[2]*2-q[3]*2;
    q[0]=a+c>>2; q[2]=a-c>>2;
    q[1]=b+d>>2; q[3]=b-d>>2;
  }
}


void vaveblok(int *p, int s, int dep) {
  int x, y, s2, d2, d3;
  if (dep<=0) return;
  //for (y=0; y<s; y++) wltx(p, 1, s);
  wltx(p, s*s>>1);
  wltx(p, s*s>>2);
  wltx(p+(s*s>>1), s*s>>2);
  //for (x=0; x<s; x++)
  if (s>4) {
    s2=s>>1; d2=dep-1; d3=0; if (d2<d3) d3=d2;
    vaveblok(p, s2, d2);
    //vaveblok(p+s2*s2, s2, d3);
    //vaveblok(p+s2*s2*2, s2, d3);
    //vaveblok(p+s2*s2*3, s2, d3);
    if (s>200) screw(p+s2*s2, 3*s2*s2);
  }
}
void unvaveblok(int *p, int s, int dep) {
  int x, y, s2, d2, d3;
  if (dep<=0) return;
  if (s>4) {
    s2=s>>1; d2=dep-1; d3=0; if (d2<d3) d3=d2;
    unvaveblok(p, s2, d2);
    //unvaveblok(p+s2*s2, s2, d3);
    //unvaveblok(p+s2*s2*2, s2, d3);
    //unvaveblok(p+s2*s2*3, s2, d3);
    if (s>200) unscrew(p+s2*s2, 3*s2*s2);
  }
  //for (x=0; x<s; x++) wltx1(p, 2, s);
  //for (y=0; y<s; y++)
  wltx1(p+(s*s>>1), s*s>>2);
  wltx1(p, s*s>>2);
  wltx1(p, s*s>>1);
}




void writetga(char *fname, char *buf) {
  static char hdr[]="\0\0\2\0\0\0\0\0\0\0\0\0\0\4\0\4\30\0";
  int x, y, i;
  static char tmp[3072];
  FILE *fp=fopen(fname, "wb");
  fwrite(hdr, 1, 18, fp);
  for (y=1023; y>=0; y--) {
    for (x=0; x<1024; x++) tmp[x*3]=buf[y*3072+x*3+2], tmp[x*3+1]=buf[y*3072+x*3+1], tmp[x*3+2]=buf[y*3072+x*3];
    fwrite(tmp, 3072, 1, fp);
  }
  fclose(fp);
}
void writeblok(char *fname, char *buf) {
  int x, y, i;
  static char tmp[3072];
  FILE *fp=fopen(fname, "wb");
  for (i=0; i<3; i++) for (y=0; y<1024; y++) {
    for (x=0; x<1024; x++) tmp[x]=buf[y*3072+x*3+i];
    fwrite(tmp, 1024, 1, fp);
  }
  fclose(fp);
}


static unsigned _num, _min, _max;
static unsigned char *_buf, *_bufp, *_bufm;
void beginputprob(unsigned char *buf, int len) {
  _num=0; _min=0; _max=0xffffffff; _buf=_bufp=buf; _bufm=_buf+len;
}
int endputprob() {
  while (((_min^_max)&0xff000000)!=0)
    _bufp<_bufm?*_bufp++=_min>>24:0, _min<<=8, _max<<=8, _min+=128, _max+=128;
  return _bufp-_buf;
}
void putprob(int bit, float prob) {
  unsigned a=_max-(_max-_min)*prob;
  if (a<_min) a=_min;
  if (a>_max-1) a=_max-1;
  if (bit) _min=a+1; else _max=a;
  while (((_min^_max)&0xff000000)==0)
    _bufp<_bufm?*_bufp++=_min>>24:0, _min<<=8, _max<<=8, _max+=255;
}
void begingetprob(unsigned char *buf, int len) {
  _num=0; _min=0; _max=0xffffffff; _buf=_bufp=buf; _bufm=_buf+len;
  _num=_num<<8|*_bufp++; _num=_num<<8|*_bufp++;
  _num=_num<<8|*_bufp++; _num=_num<<8|*_bufp++;
}
int endgetprob() {
  return _bufp-_buf;
}
int getprob(float prob) {
  int bit;
  unsigned a=_max-(_max-_min)*prob;
  if (a<_min) a=_min;
  if (a>_max-1) a=_max-1;
  bit=_num>a;
  if (bit) _min=a+1; else _max=a;
  while (((_min^_max)&0xff000000)==0)
    _min<<=8, _max<<=8, _max+=255, _num<<=8, _num+=_bufp<_bufm?*_bufp++:128;
  return bit;
}

void encodetest() {
  int i, j;
  static unsigned char buf[4096];
  static char txti[]="TESTITEKSTI1A0003 JOO T””T PIMPOM BLAA KOLIN GSIDFGIUS BLAABLAA 666";
  static char rekonst[40960];
  beginputprob(buf, 4096);
  for (i=0; i<sizeof(txti)*8; i++)
    putprob(txti[i>>3]>>(7-i&7)&1, .37);
  printf("%i\n", i=endputprob());
  begingetprob(buf, i);
  for (i=0; i<sizeof(txti)*8; i++)
    rekonst[i>>3]=rekonst[i>>3]&~(1<<(7-i&7))|getprob(.37)<<(7-i&7);
  //for (i=0; i<sizeof(txti)-1; i++) rekonst[i]-=rekonst[i]==0;
  endgetprob();
  printf("rekonstruoitu: %s\n", rekonst);
  exit(0);
}





float exp2(float x) { return exp(x*log(2)); }

#define nearest(x) ((int)((x)+1000000.5)-1000000)

float sc1, sc2;
int scatter(float x) {
  int a;
  if (x<0) { a=(int)(-x+sc1); if (-x<.2) a=0; sc1=-x+sc1-a; return -a; }
    else { a=(int)(x+sc2); if (x<.2) a=0; sc2=x+sc2-a; return a; }
}


int main(int argc, char **argv) {
  //int sdfgsdfa=(encodetest(),0);
  char *dat;
  int x, y, i, j;
  double size;
  Video *vid=new_vid_screen(1024, 768, 32);
  Lfb *l;
  int *dat2=malloc(1024*1024*3*4);

  for (x=0; x<1024; x++) {
    y=x<<8&0x00ff0000|x&0x000000ff;
    y=y<<4&0x0f000f00|y&0x000f000f;
    y=y<<2&0x30303030|y&0x03030303;
    y=y<<1&0x44444444|y&0x11111111;
    xx[x]=y;
    yy[x]=flip4[x&15]<<12|flip4[x>>4&15]<<4|flip4[x>>8&15]>>4;
  }


  if (!dat2) return 1;

  //printf("\n\n%.8f %.8f %.8f %.8f\n\n",
    //(1.*(1.+sqrt(3.))/sqrt(64.)),
    //(1.*(3.+sqrt(3.))/sqrt(64.)),
    //(1.*(3.-sqrt(3.))/sqrt(64.)),
    //(1.*(1.-sqrt(3.))/sqrt(64.)));
    //return 0;

  dat=loadtga("zmap1k.tga");
  //dat=loadtga("xrainmap.tga");

  for (y=0; y<1024; y++) for (x=0; x<1024; x++) {
    float r=(dat[y*3072+x*3  ]/255.);
    float g=(dat[y*3072+x*3+1]/255.);
    float b=(dat[y*3072+x*3+2]/255.);
    float Y, U, V, k;
    r*=r; g*=g; b*=b;
    Y=.299*r+.587*g+.114*b; U=r-Y; V=b-Y;
    k=.2/(Y>.1?Y:.1);
    //dat2[y*3072+x*3  ]=nearest(U*k*(127./.701)*1024.);
    //dat2[y*3072+x*3+1]=nearest((log2(Y*15.+1.)*(255./4.0)-128.)*1024.);
    //dat2[y*3072+x*3+2]=nearest(V*k*(127./.886)*1024.);
    dat2[xx[y]*2+xx[x]]=nearest(U*k*(127./.701)*1024.);
    dat2[xx[y]*2+xx[x]+1048576]=nearest((log2(Y*15.+1.)*(255./4.0)-128.)*1024.);
    dat2[xx[y]*2+xx[x]+2097152]=nearest(V*k*(127./.886)*1024.);
  }

  l=vid_openlfb(vid);
  for (y=0; y<l->height; y++) for (x=0; x<l->width; x++) for (i=0; i<3; i++)
    j=dat2[xx[y]*2+xx[x]+i*1048576]+131072>>10, l->buf[y*l->pitch+x*4+i]=j;
  vid_closelfb(vid);
  while (*(volatile short*)1050==*(volatile short*)1052) ; *(volatile short*)1050=*(volatile short*)1052;

  vaveblok(dat2, 1024, 6);
  vaveblok(dat2+1048576, 1024, 6);
  vaveblok(dat2+2097152, 1024, 6);
  for (i=0; i<3; i++) {
    float r;
    for (j=0; j<1048576; j++) {
      if ((j&63)==0) sc1=sc2=0;
      if ((j&j-1)==0 && (j&0x55555555)) {
        r=exp(-sqrt(j)*(1/200.))*(.5/1024.);
        if (i!=1 && j>=16384) r=0;
      }
      x=scatter(dat2[i*1048576+j]*r)+128;
      dat[i*1048576+j]=x<0?0:x>255?255:x;
    }
  }

  l=vid_openlfb(vid);
  for (y=0; y<l->height; y++) for (x=0; x<l->width; x++) for (i=0; i<3; i++)
    j=dat[xx[y]*2+xx[x]+i*1048576], l->buf[y*l->pitch+x*4+i]=j==128?0:j;
    //l->buf[y*l->pitch+x*4+i]=dat[(y*1024+x)*3+2-i]==128?0:dat[(y*1024+x)*3+2-i];
  vid_closelfb(vid);
  while (*(volatile short*)1050==*(volatile short*)1052) ; *(volatile short*)1050=*(volatile short*)1052;

  {
    writeblok("vavei.dat", dat);
    size=0;
    for (i=0; i<3*1048576; i+=4096) {
      j=0;
      for (x=0; x<256; x++) {
        j=0;
        for (y=i; y<i+4096; y++) if (dat[y]==x) j++;
        if (j) size+=j*-log(j*(1/4096.))/log(2)+12;
      }
    }
  }


  for (i=0; i<3; i++) {
    float r;
    for (j=0; j<1048576; j++) {
      if ((j&j-1)==0 && (j&0x55555555)) r=exp(sqrt(j)*(1/200.))*(1024./.5);
      dat2[i*1048576+j]=nearest((dat[i*1048576+j]-128)*r);
    }
  }

  unvaveblok(dat2, 1024, 6);
  unvaveblok(dat2+1048576, 1024, 6);
  unvaveblok(dat2+2097152, 1024, 6);

  //for (x=0; x<3072*1024; x++) j=(dat2[x]+512>>10)+128, dat[x]=j<0?0:j>255?255:j;


  for (y=0; y<1024; y++) for (x=0; x<1024; x++) {
    float Y=(exp2((dat2[xx[y]*2+xx[x]+1048576]+131072)*(4/255./1024.))-1)/15.0;
    float k=5.*(Y>0.1?Y:0.1);
    float U=(dat2[xx[y]*2+xx[x]])*k*(.701/127.4/1024.);
    float V=(dat2[xx[y]*2+xx[x]+2097152])*k*(.886/127.4/1024.);
    float r=Y+U, b=Y+V, g=(Y-.299*r-.114*b)/.587;
    r=r<0?0:r>1?1:r; g=g<0?0:g>1?1:g; b=b<0?0:b>1?1:b;
    dat[y*3072+x*3  ]=nearest(sqrt(r)*255);
    dat[y*3072+x*3+1]=nearest(sqrt(g)*255);
    dat[y*3072+x*3+2]=nearest(sqrt(b)*255);
  }
  l=vid_openlfb(vid);
  //for (y=0; y<l->height; y++) for (x=0; x<l->width; x++) for (i=0; i<3; i++)
    //j=dat[xx[y]*2+xx[x]+i*1048576], l->buf[y*l->pitch+x*4+i]=j;
  for (y=0; y<l->height; y++) for (x=0; x<l->width; x++) for (i=0; i<3; i++)
    l->buf[y*l->pitch+x*4+i]=dat[(y*1024+x)*3+2-i];
  vid_closelfb(vid);
  while (*(volatile short*)1050==*(volatile short*)1052) ; *(volatile short*)1050=*(volatile short*)1052;

  writetga("argh.tga", dat);



  vid_close(vid);
  printf("pakked size: %f\n", size*(1./8.));


/*  if (argc!=3) printf("ukase: %s tgafile texturefile\n", argv[0]), exit(1);
  dat=loadtga(argv[1]);
  obuf=malloc(3*1048576);
  putbits(256, 32);
  putbits(256, 32);
  for (y=0; y<256; y++) for (x=0; x<256; x++) {
    char *p=dat+(y*1024+x)*12;
    int s=(p[2]<<8&0xf800)+(p[1]<<3&0x7e0)+(p[0]>>3&0x1f);
    putbits(s, 16); //putbits(3, 8);
  }
  //yfine(dat, 8, 4); xfine(dat, 4, 4);
  yfine(dat, 4, 2); xfine(dat, 2, 2);
  yfine(dat, 2, 1); xfine(dat, 1, 1);
  {
    FILE *outf=fopen(argv[2], "wb");
    fwrite(obuf, obit+7>>3, 1, outf);
  }*/
  return 0;
}
