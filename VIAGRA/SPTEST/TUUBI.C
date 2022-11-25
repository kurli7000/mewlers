
void inter8x8_t(char *dest, int x1, int y1, gridfx *g) {
  int x, y, u, v, u2, v2, a, c, br;
  int u_xinc, u_yinc, u_xyinc;
  int v_xinc, v_yinc, v_xyinc;
  int c_xinc, c_yinc, c_xyinc;
  int u2_xinc, u2_yinc, u2_xyinc;
  int v2_xinc, v2_yinc, v2_xyinc;
  char *d=dest;

  u_xinc=(g[1].u-g[0].u)>>4;
  u_yinc=(g[41].u-g[0].u)>>3;
  v_xinc=(g[1].v-g[0].v)>>4;
  v_yinc=(g[41].v-g[0].v)>>3;
  c_xinc=(g[1].c-g[0].c)>>4;
  c_yinc=(g[41].c-g[0].c)>>3;

  u2_xinc=(g[1].u2-g[0].u2)>>4;
  u2_yinc=(g[41].u2-g[0].u2)>>3;
  v2_xinc=(g[1].v2-g[0].v2)>>4;
  v2_yinc=(g[41].v2-g[0].v2)>>3;

  u_xyinc=g[42].u-g[41].u-g[1].u+g[0].u>>7;
  v_xyinc=g[42].v-g[41].v-g[1].v+g[0].v>>7;
  c_xyinc=g[42].c-g[41].c-g[1].c+g[0].c>>7;
  u2_xyinc=g[42].u2-g[41].u2-g[1].u2+g[0].u2>>7;
  v2_xyinc=g[42].v2-g[41].v2-g[1].v2+g[0].v2>>7;

  u=g[0].u;
  v=g[0].v;
  c=g[0].c;
  u2=g[0].u2;
  v2=g[0].v2;

  for (y=0; y<8; y++) {
    for (x=0; x<8; x++) {
      br=(texture[(v>>mapsy&wrapy)+(u>>mapsx&wrapx)]<<15)+(c>>1);
      a=texture2[(v2>>mapsy&wrapy)+(u2>>mapsx&wrapx)]*br>>valoshift;

      //a=(texture[(v>>mapsy&wrapy)+(u>>mapsx&wrapx)]*c>>valoshift)+
         //texture2[(v2>>mapsy&wrapy)+(u2>>mapsx&wrapx)]>>1;

      u+=u_xinc; v+=v_xinc; c+=c_xinc;
      u2+=u2_xinc; v2+=v2_xinc;

      br=(texture[(v>>mapsy&wrapy)+(u>>mapsx&wrapx)]<<15)+(c>>1);
      *d++=a+texture2[(v2>>mapsy&wrapy)+(u2>>mapsx&wrapx)]*br>>(valoshift+1);

      //*d++=a+((texture[(v>>mapsy&wrapy)+(u>>mapsx&wrapx)]*c>>valoshift)+
               //texture2[(v2>>mapsy&wrapy)+(u2>>mapsx&wrapx)])>>2;

      u+=u_xinc; v+=v_xinc; c+=c_xinc;
      u2+=u2_xinc; v2+=v2_xinc;
    }
    u+=u_yinc-16*u_xinc;
    v+=v_yinc-16*v_xinc;
    c+=c_yinc-16*c_xinc;
    u2+=u2_yinc-16*u2_xinc;
    v2+=v2_yinc-16*v2_xinc;

    u_xinc+=u_xyinc;
    v_xinc+=v_xyinc;
    c_xinc+=c_xyinc;
    u2_xinc+=u2_xyinc;
    v2_xinc+=v2_xyinc;
    d+=640-8;
  }
}

void ammuu_sade_t(gridfx *g, int x, int y) {
  float px, py, pz, t;
  float px2, py2, pz2;
  float ax, bx, cx;
  float test;

  suunta[0]=camera[2][0]-camera[1][0]*(y-200)*pery-camera[0][0]*(x-320)*perx;
  suunta[1]=camera[2][1]-camera[1][1]*(y-200)*pery-camera[0][1]*(x-320)*perx;
  suunta[2]=camera[2][2]-camera[1][2]*(y-200)*pery-camera[0][2]*(x-320)*perx;

  ax=suunta[0]*suunta[0]*opu[0]+
     suunta[1]*suunta[1]*opu[1]+
     suunta[2]*suunta[2]*opu[2];
  bx=suunta[0]*posx*opu[0]+
     suunta[1]*posy*opu[1]+
     suunta[2]*posz*opu[2];
  cx=posx*posx*opu[0]+
     posy*posy*opu[1]+
     posz*posz*opu[2]-opu[3];

  test=bx*bx-ax*cx;
  if (test<0) {
    g[0].c=ulkona;
  } else {
    t=(-fsqrt(test)-bx)/ax;
    px=posx+t*suunta[0];
    py=posy+t*suunta[1];
    pz=posz+t*suunta[2];
    px2=px*mapm[0][0]+py*mapm[1][0]+pz*mapm[2][0];
    py2=px*mapm[0][1]+py*mapm[1][1]+pz*mapm[2][1];
    pz2=px*mapm[0][2]+py*mapm[1][2]+pz*mapm[2][2];

    //uusix=vanhax*i.x+vanhay*j.x+vanhaz*k.x;
    //uusiy=vanhax*i.y+vanhay*j.y+vanhaz*k.y;
    //uusiz=vanhax*i.z+vanhay*j.z vanhaz*k.z;

    g[0].u=(int)(fpatan(px2, py2)/pi*16777216.0)*128; // valot
    g[0].v=(int)(fpatan(pz2, fsqrt(px2*px2+py2*py2))/pi*128.0*16777216.0)<<1;

    g[0].u2=(int)(fpatan(px, py)/pi*16777216.0)*128; // tunneli
    g[0].v2=(int)(pz*16777216.0)*16;

    g[0].c=65536*128-t*16*65536;
    if (g[0].c<65536) g[0].c=65536;

  }
}



void subdiv1_t(int x, int y, gridfx *d, gridfx *s1, gridfx *s2) {
  if (s1->c==ulkona && s2->c==ulkona) d->c=ulkona; else
    if (s1->c!=ulkona && s2->c!=ulkona &&
       (unsigned)(s1->u-s2->u+0x40000000)<0x80000000 &&
       (unsigned)(s1->u2-s2->u2+0x40000000)<0x80000000) {
      d->u=s1->u+(s2->u-s1->u>>1);
      d->v=s1->v+(s2->v-s1->v>>1);
      d->u2=s1->u2+(s2->u2-s1->u2>>1);
      d->v2=s1->v2+(s2->v2-s1->v2>>1);
      d->c=s1->c+(s2->c-s1->c>>1);
    } else ammuu_sade_t(d, x, y);
}
void subdiv8x8_t(char *dest, int x1, int y1, gridfx *g) {
  int x, y, br;
  static gridfx gtmp[17*17], *gp;

  fill32(gtmp, sizeof(gtmp)>>2, 0);
  gtmp[0]=g[0]; gtmp[16]=g[1]; gtmp[16*17]=g[41]; gtmp[16*17+16]=g[42];

  subdiv1_t(x1+ 8, y1+0, gtmp+ 8+ 0*17, gtmp+ 0+ 0*17, gtmp+16+ 0*17);
  subdiv1_t(x1+ 4, y1+0, gtmp+ 4+ 0*17, gtmp+ 0+ 0*17, gtmp+ 8+ 0*17);
  subdiv1_t(x1+12, y1+0, gtmp+12+ 0*17, gtmp+ 8+ 0*17, gtmp+16+ 0*17);
  subdiv1_t(x1+ 2, y1+0, gtmp+ 2+ 0*17, gtmp+ 0+ 0*17, gtmp+ 4+ 0*17);
  subdiv1_t(x1+ 6, y1+0, gtmp+ 6+ 0*17, gtmp+ 4+ 0*17, gtmp+ 8+ 0*17);
  subdiv1_t(x1+10, y1+0, gtmp+10+ 0*17, gtmp+ 8+ 0*17, gtmp+12+ 0*17);
  subdiv1_t(x1+14, y1+0, gtmp+14+ 0*17, gtmp+12+ 0*17, gtmp+16+ 0*17);
  subdiv1_t(x1+ 1, y1+0, gtmp+ 1+ 0*17, gtmp+ 0+ 0*17, gtmp+ 2+ 0*17);
  subdiv1_t(x1+ 3, y1+0, gtmp+ 3+ 0*17, gtmp+ 2+ 0*17, gtmp+ 4+ 0*17);
  subdiv1_t(x1+ 5, y1+0, gtmp+ 5+ 0*17, gtmp+ 4+ 0*17, gtmp+ 6+ 0*17);
  subdiv1_t(x1+ 7, y1+0, gtmp+ 7+ 0*17, gtmp+ 6+ 0*17, gtmp+ 8+ 0*17);
  subdiv1_t(x1+ 9, y1+0, gtmp+ 9+ 0*17, gtmp+ 8+ 0*17, gtmp+10+ 0*17);
  subdiv1_t(x1+11, y1+0, gtmp+11+ 0*17, gtmp+10+ 0*17, gtmp+12+ 0*17);
  subdiv1_t(x1+13, y1+0, gtmp+13+ 0*17, gtmp+12+ 0*17, gtmp+14+ 0*17);
  subdiv1_t(x1+15, y1+0, gtmp+15+ 0*17, gtmp+14+ 0*17, gtmp+16+ 0*17);
  subdiv1_t(x1+ 8, y1+16, gtmp+ 8+16*17, gtmp+ 0+16*17, gtmp+16+16*17);
  subdiv1_t(x1+ 4, y1+16, gtmp+ 4+16*17, gtmp+ 0+16*17, gtmp+ 8+16*17);
  subdiv1_t(x1+12, y1+16, gtmp+12+16*17, gtmp+ 8+16*17, gtmp+16+16*17);
  subdiv1_t(x1+ 2, y1+16, gtmp+ 2+16*17, gtmp+ 0+16*17, gtmp+ 4+16*17);
  subdiv1_t(x1+ 6, y1+16, gtmp+ 6+16*17, gtmp+ 4+16*17, gtmp+ 8+16*17);
  subdiv1_t(x1+10, y1+16, gtmp+10+16*17, gtmp+ 8+16*17, gtmp+12+16*17);
  subdiv1_t(x1+14, y1+16, gtmp+14+16*17, gtmp+12+16*17, gtmp+16+16*17);
  subdiv1_t(x1+ 1, y1+16, gtmp+ 1+16*17, gtmp+ 0+16*17, gtmp+ 2+16*17);
  subdiv1_t(x1+ 3, y1+16, gtmp+ 3+16*17, gtmp+ 2+16*17, gtmp+ 4+16*17);
  subdiv1_t(x1+ 5, y1+16, gtmp+ 5+16*17, gtmp+ 4+16*17, gtmp+ 6+16*17);
  subdiv1_t(x1+ 7, y1+16, gtmp+ 7+16*17, gtmp+ 6+16*17, gtmp+ 8+16*17);
  subdiv1_t(x1+ 9, y1+16, gtmp+ 9+16*17, gtmp+ 8+16*17, gtmp+10+16*17);
  subdiv1_t(x1+11, y1+16, gtmp+11+16*17, gtmp+10+16*17, gtmp+12+16*17);
  subdiv1_t(x1+13, y1+16, gtmp+13+16*17, gtmp+12+16*17, gtmp+14+16*17);
  subdiv1_t(x1+15, y1+16, gtmp+15+16*17, gtmp+14+16*17, gtmp+16+16*17);
  for (x=0; x<16; x++) {
    subdiv1_t(x1+x, y1+ 8, gtmp+x+ 8*17, gtmp+x+ 0*17, gtmp+x+16*17);
    subdiv1_t(x1+x, y1+ 4, gtmp+x+ 4*17, gtmp+x+ 0*17, gtmp+x+ 8*17);
    subdiv1_t(x1+x, y1+12, gtmp+x+12*17, gtmp+x+ 8*17, gtmp+x+16*17);
    subdiv1_t(x1+x, y1+ 2, gtmp+x+ 2*17, gtmp+x+ 0*17, gtmp+x+ 4*17);
    subdiv1_t(x1+x, y1+ 6, gtmp+x+ 6*17, gtmp+x+ 4*17, gtmp+x+ 8*17);
    subdiv1_t(x1+x, y1+10, gtmp+x+10*17, gtmp+x+ 8*17, gtmp+x+12*17);
    subdiv1_t(x1+x, y1+14, gtmp+x+14*17, gtmp+x+12*17, gtmp+x+16*17);
    subdiv1_t(x1+x, y1+ 1, gtmp+x+ 1*17, gtmp+x+ 0*17, gtmp+x+ 2*17);
    subdiv1_t(x1+x, y1+ 3, gtmp+x+ 3*17, gtmp+x+ 2*17, gtmp+x+ 4*17);
    subdiv1_t(x1+x, y1+ 5, gtmp+x+ 5*17, gtmp+x+ 4*17, gtmp+x+ 6*17);
    subdiv1_t(x1+x, y1+ 7, gtmp+x+ 7*17, gtmp+x+ 6*17, gtmp+x+ 8*17);
    subdiv1_t(x1+x, y1+ 9, gtmp+x+ 9*17, gtmp+x+ 8*17, gtmp+x+10*17);
    subdiv1_t(x1+x, y1+11, gtmp+x+11*17, gtmp+x+10*17, gtmp+x+12*17);
    subdiv1_t(x1+x, y1+13, gtmp+x+13*17, gtmp+x+12*17, gtmp+x+14*17);
    subdiv1_t(x1+x, y1+15, gtmp+x+15*17, gtmp+x+14*17, gtmp+x+16*17);
  }

  for (gp=gtmp, y=0; y<16; y++, gp++) for (x=0; x<16; x++, gp++) {
    if (gp->c==ulkona) gp->c=0;
    else {
      br=(texture[(gp->v>>mapsy&wrapy)+(gp->u>>mapsx&wrapx)]<<15)+(gp->c>>1);
      gp->c=texture2[(gp->v2>>mapsy&wrapy)+(gp->u2>>mapsx&wrapx)]*br>>valoshift;
    }
  }
    //gp->c=gp->c==ulkona?0:(texture[(gp->v>>mapsy&wrapy)+(gp->u>>mapsx&wrapx)]*gp->c>>valoshift)+
                           //texture2[(gp->v2>>mapsy&wrapy)+(gp->u2>>mapsx&wrapx)]>>1;

  for (gp=gtmp, y=0; y<8; y++, gp+=18) {
    char *d=dest+y*640;
    for (x=0; x<8; x++, gp+=2) *d++=gp[0].c+gp[1].c+gp[17].c+gp[18].c+2>>2;
  }

}



dot raycokes_t(char *dest, float _posx, float _posy, float _posz,
               float tgtx, float tgty, float tgtz, float jep) {
  float px, py, pz, t;
  int x, y, u, v;
  float ax, bx, cx;
  int xypos;
  float r, test;
  gridfx *p;
  dot joops;
  float xxx, yyy, zzz;

  posx=_posx; posy=_posy; posz=_posz;
  teematriisi2(camera, tgtx-posx, tgty-posy, tgtz-posz);
  pery=1/300.0; perx=1/300.0;

  //if (texture==&mappi1)
  mapsy=16, mapsx=24, wrapy=0xff00, wrapx=0xff;

  //if (texture==&mappi2) mapsy=16, mapsx=23, wrapy=0x3f80, wrapx=0x7f;
  //if (texture==&mappi2) mapsy=15, mapsx=23, wrapy=0xff00, wrapx=0xff;
  //if (texture==&mappi3) mapsy=16, mapsx=25, wrapy=0x3f80, wrapx=0x7f;
  //if (texture==&mappi4) mapsy=16, mapsx=25, wrapy=0x3f80, wrapx=0x7f;


  xxx=camera[0][0]*(0-posx)+camera[0][1]*(0-posy)+camera[0][2]*(0-posz);
  yyy=camera[1][0]*(0-posx)+camera[1][1]*(0-posy)+camera[1][2]*(0-posz);
  zzz=camera[2][0]*(0-posx)+camera[2][1]*(0-posy)+camera[2][2]*(0-posz);

  joops.x=(xxx/zzz*100*1.8)*jep;
  joops.y=(yyy/zzz*100*1.8)*jep;
  if (joops.x<-159) joops.x=-159; if (joops.x>159) joops.x=159;
  if (joops.y<-99) joops.y=-99; if (joops.y>99) joops.y=99;
  dest=dest+joops.y*640+joops.x;

  for (y=0; y<26; y++) for (x=0; x<41; x++) ammuu_sade_t(pallero+y*41+x, x*16, y*16);

  for (y=0; y<25; y++)
    for (x=0; x<40; x++) {
      char *d2=dest+y*640*8+x*8;
      xypos=y*41+x; p=pallero+xypos;
      //if (p[0].c!=ulkona && p[1].c!=ulkona && p[41].c!=ulkona && p[42].c!=ulkona
         //&& (unsigned)(p[0].u-p[42].u+0x40000000)<0x80000000
         //&& (unsigned)(p[1].u-p[41].u+0x40000000)<0x80000000
         //&& (unsigned)(p[0].u2-p[42].u2+0x40000000)<0x80000000
         //&& (unsigned)(p[1].u2-p[41].u2+0x40000000)<0x80000000)
        inter8x8_t(d2, x<<4, y<<4, pallero+xypos);
      //else if (p[0].c==ulkona && p[1].c==ulkona && p[41].c==ulkona && p[42].c==ulkona)
        //fill8x8(d2);
      //else subdiv8x8_t(d2, x<<4, y<<4, pallero+xypos);
  }
  return joops;
}


