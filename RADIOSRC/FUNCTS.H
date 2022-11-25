int ukonvi(int u);
int vkonvi(int v);
#define uaddi(u) (ukonvi(u)|0xaaaaa800)
#define vaddi(v) (vkonvi(v)|0x55555800)
int uv2i(int u, int v);
//int ifloor(float f) { int i=f; return i>f?i-1:i; }
extern float if_05; extern int if_tmp[2];
int ifloor(float);
#pragma aux ifloor="fsub if_05" "fistp qword ptr[if_tmp]" "mov eax,if_tmp"\
  parm[8087] value[eax] modify exact[8087 eax];
int inear(float);
#pragma aux inear="fistp qword ptr[if_tmp]" "mov eax,if_tmp"\
  parm[8087] value[eax] modify exact[8087 eax];
//void fistq(void*, float);
//#pragma aux fistq="fistp qword ptr[eax]"
  //parm[eax][8087] modify exact[8087 eax];


