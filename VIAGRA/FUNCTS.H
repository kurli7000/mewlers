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
void fistq(void*, float);
#pragma aux fistq="fistp qword ptr[eax]"\
  parm[eax][8087] modify exact[8087 eax];

int mask_adc(unsigned int *a, unsigned int b);
#pragma aux mask_adc="adc [ebx],eax" "sbb eax,eax" parm[ebx][eax] value[eax] modify exact[eax];
int mask_ne(int, int);
#pragma aux mask_ne="cmp eax,edx" "sbb ebx,ebx" "cmp edx,eax" "sbb ebx,0" parm[eax][edx] value[ebx] modify exact[ebx];
int mask_b(int, int);
#pragma aux mask_b="cmp eax,edx" "sbb ebx,ebx" parm[eax][edx] value[ebx] modify exact[ebx];
#define mask_a(a, b) (mask_b((b), (a)))
#define mask_l(a, b) (mask_b((a)+2147483648, (b)+2147483648))
#define mask_g(a, b) (mask_a((a)+2147483648, (b)+2147483648))
#define mask_e(a, b) (~mask_ne((a), (b)))
#define mask_nb(a, b) (~mask_b((a), (b)))
#define mask_na(a, b) (~mask_a((a), (b)))
#define mask_nl(a, b) (~mask_l((a), (b)))
#define mask_ng(a, b) (~mask_g((a), (b)))
#define mask_btw(a, b, c) (mask_b((a)-(b), (c)-(b)))
#define mask_nbtw(a, b, c) (mask_a((a)-(b), (c)-(b)))


