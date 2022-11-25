#ifndef __MXMPLAY_H
#define __MXMPLAY_H

//extern unsigned int _psp;

#ifdef __cplusplus
extern "C"
{
#endif

int xmpInit(void *mxmmem, unsigned long psp, void *mxmdata, unsigned long maxtimerrate);
  #pragma aux xmpInit parm [esi] [eax] [ebx] [ecx] value [eax] modify [eax ebx ecx edx esi edi]
void xmpPlay(unsigned int startord);
  #pragma aux xmpPlay parm [eax] modify [eax ebx ecx edx esi edi]
void xmpStop();
  #pragma aux xmpStop modify [eax ebx ecx edx esi edi]
void xmpSetVolume(unsigned char vol);
  #pragma aux xmpSetVolume parm [eax]
unsigned char xmpGetSync();
  #pragma aux xmpGetSync value [al]
unsigned short xmpGetPos();
  #pragma aux xmpGetPos value [ax]
long xmpGetTimer();
  #pragma aux xmpGetTimer value [eax]


typedef struct {
  long hdMXMSig;
  long hdNOrders;
  long hdOrdLoopStart;
  long hdNChannels;
  long hdNPatterns;
  long hdNInstruments;
  char hdIniTempo;
  char hdIniBPM;
  short hdOptions;
  long hdSampStart;
  long hdSampMem8;
  long hdSampMem16;
  long hdPitchMin;
  long hdPitchMax;
  char hdPanPos[32];
  char hdOrderTable[256];
  long hdInstrTable[128];
  long hdPatternTable[256];
} Mxmheader;


#define channelsize 256
typedef struct {
  char globalvol;
  char uservol;
  char syncval;
  char curtick;
  char curtempo;
  char tick0;
  long currow;
  long patptr;
  long patlen;
  long curord;
  long jumptoord;
  long jumptorow;
  char patdelay;
  char procnot;
  char procins;
  char procvol;
  char proccmd;
  char procdat;
  char notedelayed;
  long tmOldTimer;
  short joo1;
  long tmOldSSESP;
  short joo2;
  long tmIntCount;
  long tmTimerRate;
  long tmTicker;
  long tmInRoutine;
  long stimerlen;
  long stimerpos;
  short datasegsel;
  long maxtimerrate;
  long gusport;
  long guschannels;
  char portatmp;
  Mxmheader head;
  short guslinvol[257];
  char chandata[channelsize*32];
  char tmStack[1024];
  char vibtabs[1024];
  char jarnodata[1024];
} Xmpgds;







#ifdef __cplusplus
};
#endif

#endif
