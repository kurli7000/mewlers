#include <libya\lfb.h>

typedef struct {
  int xs, ys, xc, yc;
  int bpp, fill2, fill3, fill4;
  char data[];
} Sprite;
typedef struct {
  Sprite *chars[256];
  int chwidth[256];
  int spwidth;
} Fontti;

void gfxputs(Lfb *dest, Fontti *fon, int x0, int y0, char *c);
void gfxprintf(Lfb *dest, Fontti *fon, int x0, int y0, char *fmt,...);

extern Fontti *getfont_tiny();
extern Fontti *getfont_korean();
extern Fontti *getfont_scifi();
extern Fontti *getfont_bankgoth();


