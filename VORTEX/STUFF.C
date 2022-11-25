#include "tinymath.h"
#include "tinyheap.h"
#include "tinyfile.h"

typedef unsigned short ushort;

#define pi 3.1415936536
ushort *videomem;

extern char mjuzic[100000];
Xmpgds *mdata=(Xmpgds*)mjuzic;

float musatimer() {
  return ((mdata->curord*64+mdata->currow)+(mdata->curtick
    +(xmpGetTimer()-mdata->maxtimerrate)/(float)(mdata->stimerlen))/mdata->curtempo);
}


char dbuf[64000];
ushort zbuf[64000];
ushort envi[65536];
char mappi[65536], mappi2[65536];
int pienix[81*51], pieniz[81*51], varjot[81*51], varjot2[81*51];
char isox[64000], isoz[64000], valo[64000], valo2[64000];
extern char tmap[];
ushort paletti1[16384];
char plasma[64000];
short hfield1[65536], hfield2[65536], sint1[256], sint2[256], sint3[256];
//ushort valospr162[256*256];
//char valospr16[256*256];
char sprite[256*256];
ushort spritepal[64*256];



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
float fabs(float a);
#pragma aux fabs= "fabs" parm [8087] value [8087];
int seed=3242342, seed2=8754567, seed3=1231231;
int rand() {
  seed=seed*(765342345+seed3--)+78761567+seed2++;
  return seed>>8&0x7fff;
}




