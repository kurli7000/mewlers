#include "core.h"

typedef struct _Scan Scan;
struct _Scan {
  int y;
  int x0, x1/*, x0d, x1d*/;
  float u0, ud, v0, vd, z0, zd;
  void *txt;
};
extern Scan *lines[], **linep[], *scanp;


void clearscans(Lfb *lfb);
void rasterify(Lfb *lfb);


