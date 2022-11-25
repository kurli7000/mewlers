#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

extern char layer1[16000];
extern char layer2[16000];
extern char layer3[16000];
extern char layer4[16000];
extern char layer5[16000];
extern char layer6[16000];
extern char layer7[16000];
extern char layer8[16000];

char kuva[16000];


void pak1(char *sourke, char *dest, char mask, int count) {
  int x;

  for (x=0; x<count; x++) {
     dest[x]+=(sourke[x]&1)<<mask;
  }
}
void unpak1(char *sourke, char *dest, char mask, int count) {
  int x;

  for (x=0; x<count; x++) {
     sourke[x]=(dest[x]>>mask)&1;
  }
}


void main() {
  FILE *f;

  pak1(layer1, kuva, 0, 16000);
  pak1(layer2, kuva, 1, 16000);
  pak1(layer3, kuva, 2, 16000);
  pak1(layer4, kuva, 3, 16000);
  pak1(layer5, kuva, 4, 16000);
  pak1(layer6, kuva, 5, 16000);
  pak1(layer7, kuva, 6, 16000);
  pak1(layer8, kuva, 7, 16000);


  f=fopen("testi2.1b", "wb");
  fwrite(kuva, 1, sizeof(kuva), f);
  fclose(f);

}


