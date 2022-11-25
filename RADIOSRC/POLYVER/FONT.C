#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "font.h"

void putsprite(Lfb *dest, int x0, int y0, Sprite *s) {
  int x, y, di;
  char *src;
  char  *db;
  short *dw;
  int   *dd;

  src=s->data;
  di=320-s->xs;
  for (y=0; y<s->ys; y++)
    if (dest->depth==2) {
      dw=(short*)((char*)dest->buf+(y0+y-s->yc)*dest->pitch)+x0-s->xc;
      for (x=0; x<s->xs; x++) *dw=*dw^(*src>>3), dw++, src++;
    } else if (dest->depth==4) {
      dd=(int*)((char*)dest->buf+(y0+y-s->yc)*dest->pitch)+x0-s->xc;
      for (x=0; x<s->xs; x++) *dd=*dd^*src, dd++, src++;
    } else {
      db=((char*)dest->buf+(y0+y-s->yc)*dest->pitch)+x0-s->xc;
      for (x=0; x<s->xs; x++) *db=*db^*src, db++, src++;
    }
}


void gfxputs(Lfb *dest, Fontti *fon, int x0, int y0, char *c) {
  Sprite *s;

  for (; *c!=0; c++) {
    if ((s=fon->chars[*c])==0) x0+=fon->spwidth; else {
      putsprite(dest, x0, y0, s); x0+=fon->chwidth[*c];
    }
  }
}

void gfxprintf(Lfb *dest, Fontti *fon, int x0, int y0, char *fmt, ...) {
  static char temppi[512];

  va_list arglist;
  va_start(arglist, fmt);
  vsprintf(temppi, fmt, arglist);
  va_end(arglist);
  gfxputs(dest, fon, x0, y0, temppi);
}


