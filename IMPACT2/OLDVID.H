#ifndef __VID_H__
#define __VID_H__

typedef void (*Copyfnc)(void *dest, void *src, int x, int y, int xs, int ys, int w);

struct Vesastuf {
  int _w, _h, _b, _mde;
  int _rw, _rh, _rb;
  Copyfnc _cpy;
  char *_buf, *_vbuf;
};
void v_addres(int xs, int ys);
void v_addbpp(int b);
int v_setcpy(int dfmt, int sfmt, Copyfnc x);
int v_width();
int v_height();
int v_bpp();
void *v_getlfb();
void v_copy();
int v_open();
int v_close();

void fill32(void *dezt, int kount, int num);
#pragma aux fill32= "cld" "rep stosd" parm[edi][ecx][eax];


#endif
