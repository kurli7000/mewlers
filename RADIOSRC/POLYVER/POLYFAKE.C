#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <notexists.h>

char mappi[1048576];
int ukonvi(int u) {
  return (u>>0&0x00000fff)|(u<<0&0x00001000)|(u<<1&0x00004000)|(u<<2&0x00010000)
        |(u<<3&0x00040000)|(u<<4&0x00100000)|(u<<5&0x00400000)|(u<<6&0x01000000)
        |(u<<7&0x04000000)|(u<<8&0x10000000)|(u<<9&0x40000000);
}
int vkonvi(int v) {
  return (v>>0&0x00000fff)|(v<<1&0x00002000)|(v<<2&0x00008000)|(v<<3&0x00020000)
        |(v<<4&0x00080000)|(v<<5&0x00200000)|(v<<6&0x00800000)|(v<<7&0x02000000)
        |(v<<8&0x08000000)|(v<<9&0x20000000)|(v<<10&0x80000000);
}
int uaddi(int u) { return ukonvi(u)|0xaaaaa000; }
int vaddi(int v) { return vkonvi(v)|0x55555000; }
int uv2i(int u, int v) {
  return (u<<0&0x00001)|(v<<1&0x00002)|(u<<1&0x00004)|(v<<2&0x00008)
        |(u<<2&0x00010)|(v<<3&0x00020)|(u<<3&0x00040)|(v<<4&0x00080)
        |(u<<4&0x00100)|(v<<5&0x00200)|(u<<5&0x00400)|(v<<6&0x00800)
        |(u<<6&0x01000)|(v<<7&0x02000)|(u<<7&0x04000)|(v<<8&0x08000)
        |(u<<8&0x10000)|(v<<9&0x20000)|(u<<9&0x40000)|(v<<10&0x80000);
}




int main() {
  int x,y,i,j,u0,v0,ux,uy,vx,vy,ux1,ux2,ux3,ux4,ux5,ux6,ux7,vx1,vx2,vx3,vx4,vx5,vx6,vx7;
  float t;
  register int u,v,c,*q;
  _asm {
    mov eax,13h
    int 10h
  };

  for (i=0; i<256; i++) {
    outp(0x3c8,i);
    outp(0x3c9,32-31*fcos(i*0.05));
    outp(0x3c9,32-31*fcos(i*0.07656745));
    outp(0x3c9,32-31*fcos(i*0.13567575));
  }
  for (u=0; u<1024; u++) for (v=0; v<1024; v++) {
    t=fsin(u*(M_PI*2  /512)+2)*12;
    i=120+fsin( u   *(M_PI    /512)+1)*28  +fsin(v*(M_PI    /512)+2)*28
         +fsin((u+v)*(M_PI    /512)+3)*28
         +fsin( u   *(M_PI*7  /512)+4)*10  +fsin(v*(M_PI*7  /512)+6)*10
         +fsin((u+v)*(M_PI*7  /512)+5)*10
         +fsin( u   *(M_PI*97 /512)+9)*2   +fsin(v*(M_PI*97 /512)+8)*2
         +fsin((u+v)*(M_PI*97 /512)+7)*2 +rand()%17;
    mappi[uv2i(u,v)]=i;
  }
  for (t=0; !kbhit(); t+=0.0084515413) {
    for (i=1; i<40; i++) {
      ux= fcos(t       )*2096.0*fexp(-fcos(t*0.8)*3.0);
      uy= fsin(t       )*2096.0*fexp(-fcos(t*0.8)*3.0)*1.2;
      vx=-fsin(t+M_PI/6)*2096.0*fexp(-fcos(t*0.8)*3.0);
      vy= fcos(t+M_PI/6)*2096.0*fexp(-fcos(t*0.8)*3.0)*1.2;
      u=ukonvi((i-20)*8*ux-100*uy); v=vkonvi((i-20)*8*vx-100*vy);
      ux7=uaddi(ux*7); vx7=vaddi(vx*7);
      ux6=uaddi(ux*6); vx6=vaddi(vx*6);
      ux5=uaddi(ux*5); vx5=vaddi(vx*5);
      ux4=uaddi(ux*4); vx4=vaddi(vx*4);
      ux3=uaddi(ux*3); vx3=vaddi(vx*3);
      ux2=uaddi(ux*2); vx2=vaddi(vx*2);
      ux1=uaddi(ux*1); vx1=vaddi(vx*1);
      vy=vaddi(vy); uy=uaddi(uy);
      for (q=(int*)0xa0000+i*2; q<(int*)0xa0000+16000; q+=80) {
        u&=0x55555fff; v&=0xaaaaafff;
/*             c =mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0x000000ff ; u+=ux; v+=vx;
             c+=mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0x0000ff00 ; u+=ux; v+=vx;
             c+=mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0x00ff0000 ; u+=ux; v+=vx;
        q[0]=c+(mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0xff000000); u+=ux; v+=vx;
             c =mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0x000000ff ; u+=ux; v+=vx;
             c+=mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0x0000ff00 ; u+=ux; v+=vx;
             c+=mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0x00ff0000 ; u+=ux; v+=vx;
        q[1]=c+(mappi[(unsigned)((u&=0x555557ff)+(v&=0xaaaaa7ff))>>12]&0xff000000);*/
        q[0]=(mappi[(unsigned)((u               )+(v               ))>>12]           )
            +(mappi[(unsigned)((u+ux1&0x55555fff)+(v+vx1&0xaaaaafff))>>12]<<8        )
            +(mappi[(unsigned)((u+ux2&0x55555fff)+(v+vx2&0xaaaaafff))>>12]<<16       )
            +(mappi[(unsigned)((u+ux3&0x55555fff)+(v+vx3&0xaaaaafff))>>12]<<24       );
        q[1]=(mappi[(unsigned)((u+ux4&0x55555fff)+(v+vx4&0xaaaaafff))>>12]           )
            +(mappi[(unsigned)((u+ux5&0x55555fff)+(v+vx5&0xaaaaafff))>>12]<<8        )
            +(mappi[(unsigned)((u+ux6&0x55555fff)+(v+vx6&0xaaaaafff))>>12]<<16       )
            +(mappi[(unsigned)((u+ux7&0x55555fff)+(v+vx7&0xaaaaafff))>>12]<<24       );
        u+=uy; v+=vy;
      }
    }
    outp(0x3c8,0); outp(0x3c9,0); outp(0x3c9,63); outp(0x3c9,0);
    while (inp(0x3da)&8); while (~inp(0x3da)&8);
    outp(0x3c8,0); outp(0x3c9,63); outp(0x3c9,0); outp(0x3c9,0);
  }

  _asm {
    mov eax,3h
    int 10h
  };
  return 0;
}
