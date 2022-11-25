#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <notexists.h>
#include "user.h"
#include "obuja.h"
#include "functs.h"
#include "font.h"

int kludge;
#define block for (mark(), kludge=1; kludge; release(), kludge=0)


int main() {
  int i;
  float t;
  int u, v;
  Texture *mappi1, *mappi2; Material *mat1, *mat2, *mat3;
  Mesh *obu1, *obu2;
  Frame *f;
  Space *sp1, *sp2;
  Fontti *fnt=getfont_tiny();

  _asm {
    mov eax,4f02h
    mov ebx,163h
    int 10h
  };
  //for (i=0; i<127; i++) { outb(0x3c8, i    ); outb(0x3c9, fpow(i/127.0, 0.7)*63); outb(0x3c9, 0); outb(0x3c9, fpow(i/127.0, 0.7)*63); }
  //for (i=0; i<127; i++) { outb(0x3c8, i+128); outb(0x3c9, fpow(i/127.0, 0.7)*63); outb(0x3c9, 32); outb(0x3c9, fpow(i/127.0, 0.7)*63); }
  for (i=0; i<128; i++) { outb(0x3c8, i    ); outb(0x3c9, fpow(i/127.0, 0.7)*63.3); outb(0x3c9, fpow(i/127.0, 0.7)*63.3); outb(0x3c9, fpow(i/127.0, 0.7)*63.3); }
  for (i=0; i<128; i++) { outb(0x3c8, i+128); outb(0x3c9, fpow(i/127.0, 0.7)*63.3); outb(0x3c9, fpow(i/127.0, 0.7)*63.3); outb(0x3c9, fpow(i/127.0, 0.7)*63.3); }
  block {
    int *sintbl=getmem(sizeof(*sintbl)*4096);
    for (i=0; i<4096; i++) sintbl[i]=fsin(i*(3.14159265358979323846264/2048.0))*31767;
    block {
      char *tmp=getmem(1048576), *tp;
      for (tp=tmp, u=0; u<4096; u+=4) for (v=0; v<4096; v+=4) {
        i=sintbl[u&4095]+sintbl[v&4095];
        *tp++=( 64+(sintbl[i+(sintbl[u*47&4095]+sintbl[v*47&4095]>>3)
                  +(sintbl[u*7&4095]+sintbl[v*7&4095]>>3)>>4&4095]>>9)>>1&0x7f);
        //*tp++=(u^v)>>5;
      }
      mappi1=save_txt(txt_load(new_txt(), tmp, 1024, 1024, 8, 10, 8));
    }
    block {
      char *tmp=getmem(1048576), *tp;
      for (tp=tmp, u=0; u<4096; u+=4) for (v=0; v<4096; v+=4) {
        i=sintbl[u&4095]+sintbl[v&4095];
        *tp++=256+(sintbl[i+(sintbl[u*47&4095]+sintbl[v*47&4095]>>3)
                 +(sintbl[u*7&4095]+sintbl[v*7&4095]>>3)>>4&4095]>>9)>>3&0x7f;
        //*tp++=((u^v)>>5)>>1;
      }
      mappi2=save_txt(txt_load(new_txt(), tmp, 1024, 1024, 8, 10, 8));
    }
  }
  block {
    mat1=save_mat(mat_set_txt1(new_mat(), mappi1));
    mat2=save_mat(mat_something(mat_set_txt1(new_mat(), mappi2)));
  }
  block {
    obu1=save_mesh(mesh_createcube(new_mesh(), mat1, 12, 8, 2));
    obu2=save_mesh(mesh_createicoso(new_mesh(), mat1, 10));
  }


  for (t=0; !kbhit(); t-=0.03) block {
    Vector a, b, c, d, v;
    char *dbuf=getmem(65536);

    v_create(&a, fsin(t*0.163474)*15+fsin(t*0.505734)*2,
                 fcos(t*0.242434)*15+fcos(t*0.374334)*2,
                 fcos(t*0.176064)*15+fsin(t*0.543356)*2);
    v_create(&b, fsin(t*0.154934)*15+fsin(t*0.125734)*2,
                 fsin(t*0.112836)*15+fsin(t*0.375234)*2,
                 fsin(t*0.154344)*15+fcos(t*0.194056)*2);
    v_create(&c, fcos(t*0.156332)*15+fsin(t*0.587734)*2,
                 fcos(t*0.164434)*15+fsin(t*0.143934)*2,
                 fsin(t*0.121131)*15+fsin(t*0.384356)*2);
    v_mul(&a, &a, 0.2);
    v_mul(&b, &b, 0.2);
    v_mul(&c, v_norm(&c), 18);

    f=new_frame();
    frame_open(f, dbuf, 320, 0, 0, 320, 200,
      m_lookat(new_m(), &c, &v_c0, &v_cj), 0.6);

    sp1=new_space();
    space_addsimplecut(sp1,
      obu1, m_lookat(new_m(), &b, &a, &v_cj), mat2,
      obu2, m_lookat(new_m(), &a, &c, &v_cj));
    space_draw(f, sp1);

    outp(0x3c8, 0); outp(0x3c9, 63); outp(0x3c9, 63); outp(0x3c9, 0);
    frame_close(f);

    gfxprintf(dbuf, fnt, 20, 20, "mem use: %ik + %ik",
      getsaveused()>>10, gettmpused()>>10);

    outp(0x3c8, 0); outp(0x3c9, 0); outp(0x3c9, 0); outp(0x3c9, 63);
    while (~inp(0x3da)&8); while (inp(0x3da)&1);
    outp(0x3c8, 0); outp(0x3c9, 0); outp(0x3c9, 63); outp(0x3c9, 0);
    for (i=0; i<16000; i+=4) {
      ((int*)0xa0000  )[i]=((int*)dbuf  )[i]; ((int*)dbuf  )[i]=0x01010101;
      ((int*)0xa0000+1)[i]=((int*)dbuf+1)[i]; ((int*)dbuf+1)[i]=0x01010101;
      ((int*)0xa0000+2)[i]=((int*)dbuf+2)[i]; ((int*)dbuf+2)[i]=0x01010101;
      ((int*)0xa0000+3)[i]=((int*)dbuf+3)[i]; ((int*)dbuf+3)[i]=0x01010101;
    }
    outp(0x3c8, 0); outp(0x3c9, 63); outp(0x3c9, 0); outp(0x3c9, 0);
  }
  _asm {
    mov eax,3h
    int 10h
  };
  return 0;
}



