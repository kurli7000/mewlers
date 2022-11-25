#ifndef KEY_H
#define KEY_H


#ifdef __cplusplus
extern "C"
{
#endif

int readchar();
#pragma aux readchar value [eax] modify [eax]

int keypressed();
#pragma aux keypressed value [eax] modify [eax]

#ifdef __cplusplus
};
#endif

#endif
