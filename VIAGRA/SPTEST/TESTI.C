#include <stdio.h>
#include <stdlib.h>
#include <conio.h>


char *videomem=(char*)0xa0000;

void mode(int);
#pragma aux mode="int 0x10" parm[eax] modify[edx];


extern char sp[16000];

char sp1[16000];
char sp2[16000];
char sp3[16000];
char sp4[16000];
char sp5[16000];
char sp6[16000];
char logo1[16000];
char logo2[16000];


void pak1(char *sourke, char *dest, char mask, int count) {
  int x;

  for (x=0; x<count; x++) {
     dest[x]+=(sourke[x]&1)<<mask;
  }
}
void unpak1(char *sourke, char *dest, char mask, int count) {
  int x;

  for (x=0; x<count; x++) {
     dest[x]=(sourke[x]>>mask)&1;
  }
}


void main() {
  FILE *f;
  int i, x, y;
  char *csp;

  unpak1(sp, sp1, 0, 16000);
  unpak1(sp, sp2, 1, 16000);
  unpak1(sp, sp3, 2, 16000);
  unpak1(sp, sp4, 3, 16000);
  unpak1(sp, sp5, 4, 16000);
  unpak1(sp, sp6, 5, 16000);
  unpak1(sp, logo1, 6, 16000);
  unpak1(sp, logo2, 7, 16000);

  mode(0x13);

  i=0;
  while (!kbhit()) {
    if ((i>>6)%6==0) csp=sp1;
    if ((i>>6)%6==1) csp=sp2;
    if ((i>>6)%6==2) csp=sp3;
    if ((i>>6)%6==3) csp=sp4;
    if ((i>>6)%6==4) csp=sp5;
    if ((i>>6)%6==5) csp=sp6;

    for (y=0; y<200; y++) for (x=0; x<80; x++) {
      videomem[y*320+x+240]=csp[y*80+x];
    }

    if ((i>>6)%2==0)
      for (x=0; x<16000; x++) {
        videomem[x+24000]=logo1[x]*2;
      } else
      for (x=0; x<16000; x++) {
        videomem[x+24000]=logo2[x]*2;
      }


    i++;
  }
  mode(0x3);
}


