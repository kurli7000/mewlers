#include "tinymath.h"
#include "tinyheap.h"

#define max(a,b)                (((a) > (b)) ? (a) : (b))
#define min(a,b)                (((a) < (b)) ? (a) : (b))
#define bound(x, l, u) (max(min((x), (u)), (l)))
#define sqr(x) ((x)*(x))
#define pi 3.14159265358979323824



typedef struct {float r, g, b; } Color;
typedef struct {float x, y, z; } Xyz;

void load32(void *src,long dwords);
#pragma aux load32=\
  "cld"          \
  "rep lodsd"    \
  parm [esi] [ecx];

void move32(void *src,void *dst,long dwords);
#pragma aux move32=\
  "cld"          \
  "rep movsd"    \
  parm [esi] [edi] [ecx];

void fill32(void *dst,long dwords,long num);
#pragma aux fill32=\
  "cld"          \
  "rep stosd"    \
  parm [edi] [ecx] [eax];




typedef long *plahplah[200][320];
plahplah *distancetbl[4][4];
long remap[800];








void testi(long *dst,long **src);
#pragma aux testi=   \
  "mov ebx,[esi]"    \
  "mov ecx,[esi+4]"  \
  "mov eax,[edi]"    \
  "mov edx,[edi+4]"  \
  "add eax,[ebx]"    \
  "add edx,[ecx]"    \
  "mov [edi],eax"    \
  "mov [edi+4],edx"  \
  "mov ebx,[esi+8]"  \
  "mov ecx,[esi+12]" \
  "mov eax,[edi+8]"  \
  "mov edx,[edi+12]" \
  "add eax,[ebx]"    \
  "add edx,[ecx]"    \
  "mov [edi+8],eax"  \
  "mov [edi+12],edx" \
  parm [edi] [esi]   \
  modify [eax ebx ecx edx];


void doballo(long *buffah,int xc,int yc,int sz) {
  int x,y,a,b,c,d,r,yc2,xc2;
  long *dest,*dloppu;
  long **src;
  int xalku,xloppu,xmin,xmax,ymin,ymax;
  plahplah *dtbl2;

  yc2=(400-yc)>>2;
  xc2=(640-xc)>>2;
  xmin=max(0,160-sz);
  xmax=min(320,160+sz);
  ymin=max(0,100-sz);
  ymax=min(196,100+sz);
  dtbl2=*distancetbl[(-yc)&3][(-xc)&3];
  xalku=(max(0,xmin-xc2)+1)&~3;
  xloppu=min(160,xmax-xc2)&~3;
  for (y=0;y<100;y+=4) {
    if ((y+yc2<ymax)&&(y+yc2>ymin)) {
      dest=buffah+y*168+xalku;
      dloppu=buffah+y*168+xloppu;
      src=(*dtbl2)[y+yc2]+xalku+xc2;
      while (dest<dloppu) {
        testi(dest,src);
        testi(dest+168,src+320);
        testi(dest+336,src+640);
        testi(dest+504,src+960);

        dest+=4;src+=4;
      }
    }
  }
}









int float2longtempbuf;
int float2long(float);
#pragma aux float2long=\
  "fistp dword ptr float2longtempbuf"\
  "mov eax,float2longtempbuf"\
  parm [8087]\
  value [eax];



long build30bitcolor(long red,long green,long blue) {
  return (min(red,511)<<20)+(min(green,511)<<10)+min(blue,511);
}

int buildtbl(float ir1,float ig1,float ib1,float t1,float ir2,float ig2,float ib2,float t2,float r2) {
  int i,r,g,b,size;
  register int c;
  float r2_r,r2_g,r2_b,t1_inv,t2_inv,j,n;
  float int1,int2;

  n=fsqrt((ir1+ig1+ib1)/500.0);
  t1_inv=n/t1;t2_inv=1/t2;
  r2_r=r2;
  r2_g=r2*(1+min(r2*0.00005,0.05));
  r2_b=r2*(1+min(r2*0.0001,0.1));
  ir1*=n;ig1*=n;ib1*=n;
  int1=ir1+ig1+ib1;
  int2=ir2+ig2+ib2;

  size=(int1/6-1)/t1_inv;
  if (int2>6) size=max(size,r2_r+10);
  size=min(size,500);

  ib1*=0.5;ib2*=0.5;
  for (i=0;i<size;i+=2) {
    j=1/(i*t1_inv+1);
    r=float2long(ir1*j+ir2/(sqr(i-r2_r)*t2_inv+1))-2;c=(r>511)?(511<<21):(r<0)?0:(r<<21);
    g=float2long(ig1*j+ig2/(sqr(i-r2_g)*t2_inv+1))-2;c+=(g>511)?(511<<10):(g<0)?0:(g<<10);
    b=float2long(ib1*j+ib2/(sqr(i-r2_b)*t2_inv+1))-2;c+=(b>255)?255:(b<0)?0:b;
    remap[i>>1]=c;
  }
  for (;i<500;i+=2) remap[i>>1]=0;
  return size;
}




float k_[4]={-1.2,-2.3,2,0.4};
float ir1_[4]={0.05,0.04,0.16*0.5,0.20};
float ig1_[4]={0.04,0.06,0.08*0.5,0.35};
float ib1_[4]={0.08,0.06,0.12*0.5,0.24};

#define ir2_0 0
float ir2_[4]={0.0035*2,0.013,0.0015,0.000};
#define ig2_0 0
float ig2_[4]={0.003*2,0.006,0.0015,0.000};
#define ib2_0 0
float ib2_[4]={0.006*2,0.01,0.0015,0.000};

#define t1_0 2
float t1_[4]={0.4,0.4,0.4,0.4};
#define t2_0 3
float t2_[4]={3,5,2,3};
#define r2_0 0
float r2_[4]={40,120,45,35};



void doflarez(long *buffah,float ir1_0,float ig1_0,float ib1_0,float lx,float ly) {
  float cur_k,cur_ir1,cur_ig1,cur_ib1,cur_ir2,cur_ig2,cur_ib2,cur_t1,cur_t2,cur_r2;
  int i,j,sz;
  float numpah;

  numpah=fsqrt(1-(sqr(lx)+sqr(ly))/(sqr(lx)+sqr(ly)+sqr(50)));

  for (i=0;i<16;i++) {
    cur_k=1;
    cur_ir1=ir1_0*numpah;
    cur_ig1=ig1_0*numpah;
    cur_ib1=ib1_0*numpah;
    cur_ir2=ir2_0*numpah;
    cur_ig2=ig2_0*numpah;
    cur_ib2=ib2_0*numpah;
    cur_t1=t1_0;
    cur_t2=t2_0;
    cur_r2=r2_0;
    for (j=0;j<4;j++) if (((i>>j)&1)==1) {
      cur_r2=ffabs(cur_r2-r2_[j]);
      cur_t2=cur_t1*t2_[j]+cur_t2*t1_[j];
      cur_t1+=t1_[j];
      cur_ir2=cur_ir1*ir2_[j]+cur_ir2*ir1_[j];
      cur_ig2=cur_ig1*ig2_[j]+cur_ig2*ig1_[j];
      cur_ib2=cur_ib1*ib2_[j]+cur_ib2*ib1_[j];
      cur_ir1*=ir1_[j];
      cur_ig1*=ig1_[j];
      cur_ib1*=ib1_[j];
      cur_k*=k_[j];
    }
    if ((cur_ir1>8)|(cur_ib1>8)|(cur_ig1>8)|(cur_ir2>8)|(cur_ib2>8)|(cur_ig2>8)) {
      sz=(1/(1.7624727256))*buildtbl(cur_ir1,cur_ig1,cur_ib1,cur_t1,cur_ir2,cur_ig2,cur_ib2,cur_t2/numpah,cur_r2/numpah);
      doballo(buffah,320+cur_k*lx,200+cur_k*ly,sz);
    }
  }
}



void calculate_distance_table_for_flares() {
  int x,y;

  for (y=0;y<4;y+=2) for (x=0;x<4;x+=2) {
    distancetbl[y][x]=malloc(sizeof(*distancetbl[0][0]));
  }
  for (y=1;y<800;y+=2) for (x=1;x<1280;x+=2)
    (*distancetbl[y&2][x&2])[y>>2][x>>2]=remap+
      (int)(fsqrt(sqr(y-400)+sqr(x-640))*1.7624727256/4);
  distancetbl[1][1]=distancetbl[1][0]=distancetbl[0][1]=distancetbl[0][0];
  distancetbl[3][1]=distancetbl[3][0]=distancetbl[2][1]=distancetbl[2][0];
  distancetbl[1][3]=distancetbl[1][2]=distancetbl[0][3]=distancetbl[0][2];
  distancetbl[3][3]=distancetbl[3][2]=distancetbl[2][3]=distancetbl[2][2];

}

















