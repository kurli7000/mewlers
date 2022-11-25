#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <notexists.h>
#include "user.h"
#include "obuja.h"
#include "functs.h"
#include "font.h"

/*Matrix *teematrixi(Matrix *m, float i, float j, float k) {
  float f, r;
  f=fsqrt(i*i+j*j+k*k); r=fcos(f); f=fsin(f)/f; i*=f; j*=f; k*=f;
  v_create(&m->i, r*r+i*i-j*j-k*k, 2*(i*j+k*r), 2*(k*i-j*r));
  v_create(&m->j, 2*(i*j-k*r), r*r-i*i+j*j-k*k, 2*(j*k+i*r));
  v_create(&m->k, 2*(k*i+j*r), 2*(j*k-i*r), r*r-i*i-j*j+k*k);
  v_create(&m->l, 0, 0, 0);
  return m;
}*/



int main() {
  int i;
  float t, p1, p2, p3, p4;
  int u, v;
  static int sintbl[4096];
  static char tmp[1048576], *tp;
  Texture *t1, *t2; Material *m1, *m2;
  Mesh *o, *o2;
  Frame *f;
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
  for (i=0; i<4096; i++) sintbl[i]=fsin(i*(3.14159265358979323846264/2048.0))*31767;
  mark();
  for (tp=tmp, u=0; u<4096; u+=4) for (v=0; v<4096; v+=4) {
    i=sintbl[u&4095]+sintbl[v&4095];
    *tp++=64+(sintbl[i+(sintbl[u*47&4095]+sintbl[v*47&4095]>>3)
            +(sintbl[u*7&4095]+sintbl[v*7&4095]>>3)>>4&4095]>>9)&0x7f;
    //*tp++=(u^v)>>5;
  }
  t1=save_txt(txt_load(new_txt(), tmp, 1024, 1024, 8, 10, 8));
  m1=save_mat(mat_set_txt1(new_mat(), t1));
  release();
  mark();
  for (tp=tmp, u=0; u<4096; u+=4) for (v=0; v<4096; v+=4) {
    i=sintbl[u&4095]+sintbl[v&4095];
    *tp++=64+(sintbl[i+(sintbl[u*47&4095]+sintbl[v*47&4095]>>3)
            +(sintbl[u*7&4095]+sintbl[v*7&4095]>>3)>>4&4095]>>9)>>2&0x7f;
    //*tp++=(u^v)>>5;
  }
  t2=save_txt(txt_load(new_txt(), tmp, 1024, 1024, 8, 10, 8));
  m2=save_mat(mat_set_txt1(new_mat(), t2));
  release();
  mark();
  o=save_mesh(mesh_createicoso(new_mesh(), m1, 0.5));
  o2=save_mesh(mesh_createicoso(new_mesh(), m2, -40));
  release();

  mark();
  f=new_frame();

  //printf("usz %i uamask %x uomask %x\n", t1->td->usz, t1->td->uamask, t1->td->uomask);
  //printf("vsz %i vamask %x vomask %x\n", t1->td->vsz, t1->td->vamask, t1->td->vomask);
  //printf("sh1 %i sh2 %i sh3 %i fb %x\n", t1->td->sh1, t1->td->sh2, t1->td->sh3, t1->td->fbits);


  for (t=0; !kbhit(); t-=0.003-t*0.0003) {
    static char dbuf[65536];

    mark();
    {
      Matrix m;
      Vector a, b, c, d, v, up;
      v_create(&a, -3, -0.5, 0);
      v_create(&d, fsin(t*0.1129)*17, fsin(t*0.12365)*16, fcos(t*0.07764)*17);
      p4=144-t*3.1;
          v_create(&d, fsin((t+p4*0.5)*0.154334)*18+fsin((t+p4*0.7)*0.565734)*5,
                       fcos((t+p4*0.6)*0.113434)*18+fsin((t+p4*0.6)*0.345334)*5,
                       fsin((t+p4*0.8)*0.141134)*18+fsin((t+p4*1.2)*0.154356)*5);
      p4=148-t*3.1;
          v_create(&a, fsin((t+p4*0.5)*0.154334)*18+fsin((t+p4*0.7)*0.565734)*5,
                       fcos((t+p4*0.6)*0.113434)*18+fsin((t+p4*0.6)*0.345334)*5,
                       fsin((t+p4*0.8)*0.141134)*18+fsin((t+p4*1.2)*0.154356)*5);
      p4=149-t*3.1;
          v_create(&b, fsin((t+p4*0.5)*0.154334)*18+fsin((t+p4*0.7)*0.565734)*5,
                       fcos((t+p4*0.6)*0.113434)*18+fsin((t+p4*0.6)*0.345334)*5,
                       fsin((t+p4*0.8)*0.141134)*18+fsin((t+p4*1.2)*0.154356)*5);
      frame_open(f, dbuf, 320, 0, 0, 320, 200,
        m_lookat_rel(&m, v_add(&v, &d, v_create(&v, 0,0.7,0)), &a, v_sub(&b, &b, &d), v_create(&c, 0.05, 0.2, -0.05)), 0.6);
      for (p1=(int)(p4)-5; p1<p4+5; p1+=1) {
        p2=p1+1; p3=p1+4;
        v_create(&a, fsin((t+p1*0.5)*0.154334)*18+fsin((t+p1*0.7)*0.565734)*5,
                     fcos((t+p1*0.6)*0.113434)*18+fsin((t+p1*0.6)*0.345334)*5,
                     fsin((t+p1*0.8)*0.141134)*18+fsin((t+p1*1.2)*0.154356)*5);
        v_create(&b, fsin((t+p2*0.5)*0.154334)*18+fsin((t+p2*0.7)*0.565734)*5,
                     fcos((t+p2*0.6)*0.113434)*18+fsin((t+p2*0.6)*0.345334)*5,
                     fsin((t+p2*0.8)*0.141134)*18+fsin((t+p2*1.2)*0.154356)*5);
        v_sub(&up, v_create(&up,
                     fsin((t+p3*0.5)*0.154334)*18+fsin((t+p3*0.7)*0.565734)*5,
                     fcos((t+p3*0.6)*0.113434)*18+fsin((t+p3*0.6)*0.345334)*5,
                     fsin((t+p3*0.8)*0.141134)*18+fsin((t+p3*1.2)*0.154356)*5), &a);
        mesh_draw(f, o, m_lookat(&m, &a, &b, &up));
      }
      mesh_draw(f, o2, m_ident(&m));
    }
    outp(0x3c8, 0); outp(0x3c9, 63); outp(0x3c9, 63); outp(0x3c9, 0);
    frame_close(f);

    gfxprintf(dbuf, fnt, 20, 20, "mem use: %ik + %ik",
      getsaveused()>>10, gettmpused()>>10);
    release();

    outp(0x3c8, 0); outp(0x3c9, 0); outp(0x3c9, 0); outp(0x3c9, 63);
    while (~inp(0x3da)&8); while (inp(0x3da)&1);
    outp(0x3c8, 0); outp(0x3c9, 0); outp(0x3c9, 63); outp(0x3c9, 0);
    for (i=0; i<16000; i+=4) {
      //((int*)0xa0000  )[i]=((int*)dbuf  )[i]; ((int*)dbuf  )[i]=0x81818181;
      //((int*)0xa0000+1)[i]=((int*)dbuf+1)[i]; ((int*)dbuf+1)[i]=0x81818181;
      //((int*)0xa0000+2)[i]=((int*)dbuf+2)[i]; ((int*)dbuf+2)[i]=0x81818181;
      //((int*)0xa0000+3)[i]=((int*)dbuf+3)[i]; ((int*)dbuf+3)[i]=0x81818181;
      ((int*)0xa0000  )[i]=((int*)dbuf  )[i]; ((int*)dbuf  )[i]=0x00000000;
      ((int*)0xa0000+1)[i]=((int*)dbuf+1)[i]; ((int*)dbuf+1)[i]=0x00000000;
      ((int*)0xa0000+2)[i]=((int*)dbuf+2)[i]; ((int*)dbuf+2)[i]=0x00000000;
      ((int*)0xa0000+3)[i]=((int*)dbuf+3)[i]; ((int*)dbuf+3)[i]=0x00000000;
    }
    outp(0x3c8, 0); outp(0x3c9, 63); outp(0x3c9, 0); outp(0x3c9, 0);
  }
  _asm {
    mov eax,3h
    int 10h
  };
  return 0;
}





