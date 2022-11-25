#include "tinyheap.h"
#include "vid.h"

struct Vesastuf vesastuf;

void vgasetmode(int);
#pragma aux vgasetmode="int 0x10" parm [eax] modify [eax ebx ecx edx esi edi];
void vesasetmode(int);
#pragma aux vesasetmode="mov eax,0x4f02" "int 0x10" parm [ebx] modify [eax ebx ecx edx esi edi];
void vesasetbank(int);
#pragma aux vesasetbank="mov eax,0x4f05""xor ebx,ebx""int 0x10" parm [edx] modify [eax ebx ecx edx esi edi];

void v_addres(int xs, int ys) {
  (void)xs; (void)ys;
}
void v_addbpp(int b) {
  (void)b;
}
int v_setcpy(int dfmt, int sfmt, Copyfnc x) {
  (void)dfmt; (void)sfmt; (void)x;
  return 1;
}

#define pcast(v, t) (*(t*)&(v))
void vcpy_n_n(void *dest, void *src, int x, int y, int pixels, int lines, int width) {
  int x2, y2;
  pcast(src, char*)+=y*width+x;
  for (y2=0; y2<lines; y2++, pcast(src, char*)+=width, pcast(dest, char*)+=width)
    for (x2=0; x2<pixels>>2; x2++) pcast(dest, long*)[x2]=pcast(src, long*)[x2];
}
void vcpy_08_16(void *dest, void *src, int x, int y, int pixels, int lines, int width) {
  int x2, y2;
  pcast(src, char*)+=y*width+x;
  for (y2=0; y2<lines; y2++, pcast(src, char*)+=width, pcast(dest, char*)+=width)
    for (x2=0; x2<pixels>>2; x2++) pcast(dest, long*)[x2]=pcast(src, long*)[x2]<<3&0x07e007e0;
}
/*
void vcpy_my_16(void *dest2, void *src2, int x, int y, int bytes, int lines, int width) {
  static int x2, y2, y3;
  static long *src, *dest;
  static long c, cd, d, de, i;
  static long tmp[1024];
  static int w, w1, b;

  w=width>>3; w1=w+1; b=bytes>>3;
  for (y2=0; y2<lines; y2++) {
    y3=y+y2;
    if (y3&1) {
      src=(long*)src2+(y3>>1)*w1;
      if (y3==1) for (i=0; i<w1; i++) {
        tmp[i]=src[i]+src[i+w1]>>1&0x1fe3f87f;
      } else for (i=0; i<w1; i+=2) {
        c=(src[i]+src[i+w1]>>1)*5-(src[i-w1]+src[i+2*w1]>>1)+0x0401002>>2;
        d=(src[i+1]+src[i+1+w1]>>1)*5-(src[i+1-w1]+src[i+1+2*w1]>>1)+0x0401002>>2;
        if ((c&0x20040080)!=0) c=src[i]+src[i+w1]>>1;
        if ((d&0x20040080)!=0) d=src[i+1]+src[i+1+w1]>>1;
        tmp[i]=c&0x1fe3f87f;
        tmp[i+1]=d&0x1fe3f87f;
      }
      src=tmp+(x>>3);
    } else src=(long*)src2+(y3>>1)*w1+(x>>3);
    dest=(long*)dest2+y2*(w+w);
    for (x2=0; x2<b; x2+=1) {
      d=(src[0]+src[1]>>1)*5-(src[-1]+src[2]>>1)+0x0200801>>2;
      if ((d&0x20040080)!=0) d=src[0]+src[1]>>1;
      cd=src[0]+d+0x00401002>>3&0x07e0f81f;
      de=src[1]+d+0x00401002>>3&0x07e0f81f;
      c =src[0]+0x00200801>>2&0x07e0f81f;
      d =d     +0x00200801>>2&0x07e0f81f;
      dest[0]=(c+(c>>16)&0xffff)+(cd+(cd<<16)&0xffff0000);
      dest[1]=(d+(d>>16)&0xffff)+(de+(de<<16)&0xffff0000);
      dest+=2; src+=1;
    }
  }
}
*/

void vcpy_my_16(void *_dest2, void *_src2, int _x, int _y, int _bytes, int _lines, int _width);
void vcpy_my_8(void *_dest2, void *_src2, int _x, int _y, int _bytes, int _lines, int _width);

/*
void vcpy_my_16(void *_dest2, void *_src2, int _x, int _y, int _bytes, int _lines, int _width) {
  static int x2, y2, y3;
  static long *src, *dest;
  static long *src1a, *src2a, *src3a, *src4a;
  static long c, cd, d, de, i;
  static long tmp[1024];
  static int w, w1, b;
  static void *dest2, *src2;
  static int x, y, bytes, lines, width;
  x=_x; y=_y; bytes=_bytes; lines=_lines; width=_width; dest2=_dest2; src2=_src2;
  //6737 6797 6785 6601
  w=width>>3; w1=w+1; b=bytes>>3;
  for (y2=0; y2<lines; y2++) {
    y3=y+y2;
    if (y3&1) {
      src=(long*)src2+(y3>>1)*w1;
      if (y3==1) for (i=0; i<w1; i++) {
        tmp[i]=src[i]+src[i+w1]>>1&0x1fe3f87f;
      } else {
        _asm {
          mov esi, w1
          shl esi, 2
          mov eax, src
          mov ebx, eax
          mov ecx, eax
          mov edx, eax
          sub eax, esi
          add edx, esi
          add ecx, esi
          add edx, esi
          mov [src1a], eax
          mov [src2a], ebx
          mov [src3a], ecx
          mov [src4a], edx
          mov esi, 0
          luup:
            mov edi, [src2a]
            mov eax, [esi*4+edi  ]
            mov ebx, [esi*4+edi+4]
            mov edi, [src3a]
            add eax, [esi*4+edi  ]
            add ebx, [esi*4+edi+4]
            shr eax, 1
            shr ebx, 1
            mov c, eax
            mov d, ebx
            lea eax, [eax*4+eax]
            lea ebx, [ebx*4+ebx]
            mov edi, [src1a]
            mov ecx, [esi*4+edi  ]
            mov edx, [esi*4+edi+4]
            mov edi, [src4a]
            add ecx, [esi*4+edi  ]
            add edx, [esi*4+edi+4]
            shr ecx, 1
            shr edx, 1
            sub eax, ecx
            sub ebx, edx
            add eax, 0x00401002
            add ebx, 0x00401002
            shr eax, 2
            shr ebx, 2
            test eax, 0x20040080
            jz aover
              mov eax, [c]
            aover:
            test ebx, 0x20040080
            jz bover
              mov ebx, [d]
            bover:
            and eax, 0x1fe3f87f
            and ebx, 0x1fe3f87f
            mov [esi*4+tmp  ], eax
            mov [esi*4+tmp+4], ebx
            add esi, 2
            cmp esi, [w1]
          jb luup
        };
      }
      src=tmp+(x>>3);
    } else src=(long*)src2+(y3>>1)*w1+(x>>3);
    dest=(long*)dest2+y2*(w+w);
//    for (x2=0; x2<b; x2++) {
    _asm {
      mov esi, [src]
      mov edi, [dest]
      mov eax, b
      shl eax, 2
      add eax, esi
      mov [x2], eax
      luup2:
        mov eax, [esi-4]
        mov ebx, [esi]
        mov ecx, [esi+4]
        mov edx, [esi+8]
        add eax, edx
        add ebx, ecx
        shr eax, 1
        shr ebx, 1
        mov ecx, ebx
        lea ebx, [ebx*4+ebx]
        sub ebx, eax
        add ebx, 0x0200801
        shr ebx, 2
        test ebx, 0x20040080
        jz dover
          mov ebx, ecx
        dover:
        mov [d], ebx
        mov eax, [esi]
        mov ecx, eax
        add eax, ebx
        add eax, 0x00401002
        add ecx, 0x00200801
        shr eax, 3
        shr ecx, 2
        and eax, 0x07e0f81f
        and ecx, 0x07e0f81f
        mov edx, eax
        shr eax, 16
        add eax, edx
        mov edx, ecx
        shl ecx, 16
        add ecx, edx
        and eax, 0xffff
        and ecx, 0xffff0000
        add eax, ecx
        mov [edi], eax
        mov eax, [esi+4]
        mov ecx, ebx
        add eax, ebx
        add eax, 0x00401002
        add ecx, 0x00200801
        shr eax, 3
        shr ecx, 2
        //and eax, 0x07e0f81f
        //and ecx, 0x07e0f81f
        //mov edx, eax
        //shr eax, 16
        //add eax, edx
        //mov edx, ecx
        //shl ecx, 16
        //add ecx, edx
        and eax, 0xffff
        and ecx, 0xffff0000
        add eax, ecx
        mov [edi+4], eax
        add esi, 4
        add edi, 8
        cmp esi, [x2]
      jb luup2
    };
//      d=(src[0]+src[1]>>1)*5-(src[-1]+src[2]>>1)+0x0200801>>2;
//      if ((d&0x20040080)!=0) d=src[0]+src[1]>>1;
//      cd=src[0]+d+0x00401002>>3&0x07e0f81f;
//      c =src[0]  +0x00200801>>2&0x07e0f81f;
//      dest[0]=(c+(c>>16)&0xffff)+(cd+(cd<<16)&0xffff0000);
//      de=d+src[1]+0x00401002>>3&0x07e0f81f;
//      d =d       +0x00200801>>2&0x07e0f81f;
//      dest[1]=(d+(d>>16)&0xffff)+(de+(de<<16)&0xffff0000);
//      dest+=2; src+=1;
//    }
  }
}
*/



int v_width() { return vesastuf._w; }
int v_height() { return vesastuf._h; }
int v_bpp() { return vesastuf._b; }
void *v_getlfb() { return vesastuf._buf; }
void memcpy(void *d, void *s, int c) { while (c--) *(*(char**)&d)++=*(*(char**)&s)++; }
void v_copy() {
  int w=vesastuf._w*(vesastuf._b+7>>3), d=(vesastuf._w*3-vesastuf._h*4>>3)*w, p;
  int y;

  vesasetbank(d>>16);
  for (y=0; y<vesastuf._h;) {
    p=d+0x10000&0xffff0000;
    if (d+w>=p) {
      vcpy_n_n(vesastuf._vbuf+(d&0xffff), vesastuf._buf, 0, y, p-d, 1, w);
      vesasetbank(p>>16);
      vcpy_n_n(vesastuf._vbuf, vesastuf._buf, p-d, y, w+d-p, 1, w);
      d+=w; y++;
    } else {
      int n;
      for (n=1; d+(n+1)*w<p && y+n<vesastuf._h; n++) ;
      vcpy_n_n(vesastuf._vbuf+(d&0xffff), vesastuf._buf, 0, y, w, n, w);
      d+=w*n; y+=n;
    }
  }
}
int v_open() {
  //136=320  151=400  13d=512  111=640  114=800  117=1024 */
  vesastuf._w=320; vesastuf._h=200; vesastuf._b=16;
  vesastuf._mde=0x136;
  vesasetmode(vesastuf._mde);
  vesastuf._buf=halloc(vesastuf._h*vesastuf._w*(vesastuf._b+7>>3)*4);
  fill32(vesastuf._buf, vesastuf._h*vesastuf._w*(vesastuf._b+7>>3), 0);
  vesastuf._vbuf=(char*)0xa0000;
  return 1;
}
int v_close() {
  if (vesastuf._mde) {
    vesasetmode(0x3); vesastuf._mde=0;
    return 1;
  } else return 0;
}

