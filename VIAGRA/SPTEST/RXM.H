#ifndef RXM_H
#define RXM_H

#define df16bit   1       //play 16 bit
#define dfStereo  2       //play stereo
#define dfReverse 4       //reverse stereo
#define dfWave    8       //wave output
#define dfNosound 16      //no sound
#define sb_dsp1   32      //play in DSP 1.xx mode
#define sb_awe    64      //use AWE32 if present
#define gus_iw    128     //play in extended IW mode
#define wav_lin   256     //wav linear
#define wav_32bit 512

#define xmStop  2
#define xmFade  3

typedef struct tdinfo {
  int  base;
  char irq;
  char dma1;
  char dma2;
  int  rate;
  int  flags;
  char ver;
  char vol;
  char mastervol;
  int  mem;
  int  dmabuf;
  void *vmt;
} tdinfo;

typedef struct txmstat {
  int pos;
  int row;
  int samples;
  int clipsamples;
} txmstat;

#ifdef __cplusplus
extern "C" {
#endif

//unsigned int _psp;

void i8_init(void);
  #pragma aux i8_init "*" modify [eax ebx ecx edx esi edi]

void i8_done(void);
  #pragma aux i8_done "*" modify [eax ebx ecx edx esi edi]
//--
int getdmabuf(int);
  #pragma aux getdmabuf "*" parm [eax] value [eax] modify [eax ebx edx]

void cleardmabuf(void);
  #pragma aux cleardmabuf "*" modify [eax ecx edi]

void freedmabuf(void);
  #pragma aux freedmabuf "*" modify [eax edx]

int maxtick();
  #pragma aux maxtick "*" value [eax] modify [eax ebx ecx edx esi edi]

int wavtick(void *);
  #pragma aux wavtick "*" parm [ebx] value [eax] modify [eax ebx ecx edx esi edi]

void xmstat(txmstat *);
  #pragma aux xmstat "*" parm [esi] modify [eax esi edi]


void gameinit(int channels); //ex. 4 channels
  #pragma aux gameinit "*" parm [eax] modify [eax edi]

            //first instrument is 1, note C-4 is 48, max. volume is 255
void playsound(int instument, int note, int volume);
  #pragma aux playsound "*" parm [ebx] [eax] [edx] modify [eax ebx edx edi]

//void gameinit(int);
//  #pragma aux gameinit "*" parm [eax] modify [eax edi]

//void playsound(int, int);
//  #pragma aux playsound "*" parm [eax] [edx] modify [eax ebx edx edi]
//--
void rxminit(void);
  #pragma aux rxminit "*" modify [eax ecx edi]

int n_test(tdinfo *);
  #pragma aux n_test "*" parm [edi] modify [eax ebx ecx edx esi edi]

int u_test(tdinfo *);
  #pragma aux u_test "*" parm [edi] modify [eax ebx ecx edx esi edi]

int sb_test(tdinfo *);
  #pragma aux sb_test "*" parm [edi] modify [eax ebx ecx edx esi edi]

int a_test(tdinfo *);
  #pragma aux a_test "*" parm [edi] modify [eax ebx ecx edx esi edi]

int w_test(tdinfo *);
  #pragma aux w_test "*" parm [edi] modify [eax ebx ecx edx esi edi]


int rxmdevinit(tdinfo *, void *);
  #pragma aux rxmdevinit "*" parm [esi] [edi] modify [eax ebx ecx edx esi edi]

void rxmdevdone(void);
  #pragma aux rxmdevdone "*" modify [eax ebx ecx edx esi edi]

void rxmsetvol(int);
  #pragma aux rxmsetvol "*" parm [eax] modify [eax ebx ecx edx esi edi]

void rxmplay(void *rxmmem, int pos);
  #pragma aux rxmplay "*" parm [esi] [eax] modify [eax ebx ecx edx esi edi]

void rxmstop(int);
  #pragma aux rxmstop "*" parm [eax] modify [eax edi]

void rxmcontinue(void);
  #pragma aux rxmcontinue "*" modify [edi]

void rxmskip(int);
  #pragma aux rxmskip "*" parm [eax] modify [eax edi]

//void waveplay(void *wavemem, int len);
//  #pragma aux waveplay "*" parm [esi] [eax] modify [eax ebx ecx edx esi edi]

//int waveend(void);
//  #pragma aux waveend "*" modify [eax edi]

#ifdef __cplusplus
};
#endif

#endif //RXM_H
