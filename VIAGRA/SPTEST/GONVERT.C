/*
 *  sampak.c
 *
 *
 *  Projekti: sampak.
 *
 *  Tarkoitus: typer„ testi
 *
 *
 *
 *  Copyleft (#) 1900 Jarno Wuolijoki. All rights reversed.
 *
 */

//#define VER  "0.0.5.27.0" // jw: 1.ver
//#define VER  "0.0.5.30.0" // jw: siistitty. s3m-formaatin samplehaku.
//#define VER  "0.0.6.2.0"  // jw: bin-fmt



#include <stdlib.h>
#include <lib2\mem.h>


#define malloc getmem
#define free(x)


/*
 *
 *  Definet ja vakiot
 *
 */

//unsigned char *ifilename="tr14bxmx.s3m";
//unsigned char *ifilename="d:\\temp\\.s3m";
unsigned char *ifilename="tr14bxmx.bin";
//unsigned char *ifilename="c:\\audio\\joonas\\twinnec.s3m";
unsigned char *ofilename="plaa.bin";
unsigned char *ofilename2="plaa2.bin";
#define dbg(x) printf("%s\n", #x);




/*
 *
 *  Globalit
 *
 */

unsigned char buf[2097152];
int sqlvirhekoodi=1;
char ekraan[216];




/*
 *
 *  Tyypit. Ensin typedefit sitte structit.
 *
 */

typedef struct _St3ins St3ins;
typedef struct _St3hdr St3hdr;

struct _St3ins {
  char t;
  char filename[12];
  unsigned char ptr1, ptr2, ptr3;
  int len, lbeg, lend;
  char vol;
  char paskaa2[3];
  int c2sp;
  char paskaa3[12];
  char sampname[28];
  char scrs[4];
};

struct _St3hdr {
  char songname[28];
  char paskaa[4];
  short ordnum, insnum, patnum, flags, ver, format;
  char scrm[4];
  char paskaa2[48];
};





/*
 *
 *  void virhe(char *txt, ...)
 *
 *
 *  Universaali virhek„sittelij„.
 *  Tulostaa virhetekstin ja lopettaa ohjelman.
 *  Parametrit kuten printf:ss„.
 *
 *
 */





/*
 *
 *  Kompressio taajuuden funktiona.
 *
 */

float getk(float frq) {
  float a=.05/frq;
  return a<.15?.15:a>.5?.5:a;
  //return 1.;
}



/*
 *  void gonv(float *buf, int len, int step, int dep, float frq, float fd)
 *
 *  Wavelet-transformaatio.
 *
 *  Parametrit:
 *    buf    samplebufferi
 *    len    samplejen m„„r„
 *    step   indeksidelta (3 k„sittelee arvot buf[0], buf[3], ... buf[len*3-3] jne)
 *    dep    rekursion syvyys.
 *    frq    keskitaajuus
 *    fd     alimman ja ylimm„n taajuuden ero / 4
 *
 */

void gonv(float *buf, int len, int step, int dep, float frq, float fd) {
  int i, j;
  float k, a, b;
  #define B(x) (buf[(x)*step])

  k=getk(frq);
  j=0;
  if (dep>4) {
    for (i=0; i<len; i++)
      if (B(i)*k<1.0 && B(i)*k>-1.0) j++;
    if (j*10>len*8+40) { for (i=0; i<len; i++) B(i)=0; return; }
    //if (frq>.5 && B(0)<-.01) for (i=0; i<len; i++) B(i)=-B(i);
  }


  if (dep<1 || len<10) {
    for (i=0; i<len; i++) B(i)*=k;
    return;
  }

  // jaetaan lo/hi
  for (i=0; i+1<len; i+=2) {
    a=B(i+0);
    b=B(i+1);
    B(i+0)=(a+b)*.5;
    B(i+1)=(a-b)*.625;
  }
  // hi:sta v„hennet„„n 1-aste lo (interpol)
  for (i=2; i+2<len; i+=2) {
    B(i+1)-=(B(i-2)-B(i+2))*.125;
  }
  // hi:sta v„hennet„„n 2-aste lo (bezier)
  for (i=4; i+4<len; i+=2) {
    B(i+1)-=((B(i-2)-B(i+2))*2-(B(i-4)-B(i+4))*3)*(1/32.0);
  }
  // flipataan hipass
  for (i=3; i<len; i+=4) B(i)=-B(i);
  // rekursoi
  gonv(buf     , len>>1, step*2, dep-1, frq-fd, fd*.5);
  gonv(buf+step, len>>1, step*2, dep-1, frq+fd, fd*.5);

  #undef B
}




/*
 *  void xgonv(float *buf, int len, int step, int dep, float frq, float fd)
 *
 *  Wavelet-antitransformaatio.
 *
 *  Parametrit:
 *    buf    samplebufferi
 *    len    samplejen m„„r„
 *    step   indeksidelta (3 k„sittelee arvot buf[0], buf[3], ... buf[len*3-3] jne)
 *    dep    rekursion syvyys.
 *    frq    keskitaajuus
 *    fd     alimman ja ylimm„n taajuuden ero / 4
 *
 */

void xgonv(float *buf, int len, int step, int dep, float frq, float fd) {
  int i;
  float k, a, b;
  #define B(x) (buf[(x)*step])
  if (dep<1 || len<10) {
    k=1./getk(frq);
    for (i=0; i<len; i++) B(i)*=k;
    return;
  }

  xgonv(buf           , len+1>>1, step, dep-1, frq-fd, fd*.5);
  xgonv(buf+(len+1>>1), len  >>1, step, dep-1, frq+fd, fd*.5);

  //xgonv(buf     , len>>1, step*2, dep-1, frq-fd, fd*.5);
  //xgonv(buf+step, len>>1, step*2, dep-1, frq+fd, fd*.5);

  mark();
  {
    // yhdistet„„n splitatut osat
    float *temp=malloc(len*sizeof(float));
    for (i=0; i*2<len; i++) temp[i*2]=B(i);
    for (i=0; i*2+1<len; i++) temp[i*2+1]=B(i+(len+1>>1));
    for (i=0; i<len; i++) B(i)=temp[i];
    free(temp);
    release();
  }
  // flipataan hipass
  for (i=0; i+3<len; i+=4) B(i+3)=-B(i+3);
  // hi:hin lis„t„„n 1-aste lo (interpol)
  for (i=2; i+2<len; i+=2) {
    B(i+1)+=(B(i-2)-B(i+2))*.125;
  }
  // hi:hin lis„t„„n 2-aste lo (bezier)
  for (i=4; i+4<len; i+=2) {
    B(i+1)+=((B(i-2)-B(i+2))*2-(B(i-4)-B(i+4))*3)*(1/32.0);
  }
  /* Tsykkel yle KURSID */
  for (i=0; i+1<len; i+=2) {
    a=B(i+0)*1.0;
    b=B(i+1)*0.8;
    B(i+0)=a+b;
    B(i+1)=a-b;
  }
  #undef B
}






float sc1, sc2;
/*int scatter(float x) {
  int a;
  return (int)(x+1000.5)-1000;
  //if (x<0) { a=(int)(-x+sc1); if (-x<.4) a=0; sc1=-x+sc1-a; return -a; }
    //else { a=(int)(x+sc2); if (x<.4) a=0; sc2=x+sc2-a; return a; }
}*/
int scatter(float x) {
  int a=(int)(x+1000.5)-1000;
  if (a>3||a<-3) return scatter(x*.5)*2;
  return a;
}
int rajo(int x) { return x<0?0:x>255?255:x; }
int srajo(int x) { return x<-32767?-32767:x>32767?32767:x; }
int round(float x);// { return (int)(x+1000.5)-1000; }
#pragma aux round parm[8087] value[eax] modify exact[eax 8087] = "push eax" "fistp dword ptr[esp]" "pop eax";







int tee_listatied(unsigned char *buf, int len) {
  float *xbuf=malloc(len*4);
  int i;
  for (i=0; i<len; i++) xbuf[i]=(buf[i]-128);
  xgonv(xbuf, len, 1, 12, .5, .25);
  for (i=0; i<len; i++) ((short*)buf)[i]=srajo(xbuf[i]*200.+.5);
  for (i=len-1; i>0; i--) ((short*)buf)[i]-=((short*)buf)[i-1];
  free(xbuf);
  return 1;
}




/*
 * void binconv()
 *
 *
 * Bin-konversiop„„proggis
 *
 */
#include "pak.inc"
void gonvert_samples(char *ptr) {
  int i;
  for (i=0; pakdata[i]; i+=2)
    tee_listatied(ptr+pakdata[i], pakdata[i+1]>>1);
}

