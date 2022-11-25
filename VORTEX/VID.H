#ifndef __VID_H__
#define __VID_H__

typedef void (*Copyfnc)(void *dest, void *src, int x, int y, int xs, int ys, int w);

struct Vesastuf {
  int _w, _h, _b, _mde, _ysh, _vesa;
  int _rw, _rh, _rb;
  Copyfnc _cpy;
  char *_buf, *_vbuf;
};
int v_width();
int v_height();
int v_bpp();
void *v_getlfb();
void v_copy();
int v_open(int x);
int v_close();

#endif
