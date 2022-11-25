
#include "modplyer.h"
#include <lib2\bugs.h>
#include <midas.h>
#include <midasdll.h>


static MIDASmodulePlayHandle playhandle;
static MIDASmodule module;
Modplayer *new_modplayer(char *filename) {
  MIDASstartup();
  if (!MIDASdetectSoundCard()) if (!MIDASconfig()) erreur("user abort");
  if (!MIDASinit()) erreur("midas init fail");
  MIDASstartBackgroundPlay(0);
  if ((module=MIDASloadModule(filename))==0) erreur("midas loadmodule fail");
  return (Modplayer*)1;
}
float modplayer_getpos(Modplayer *m) {
  float x, e;
  static float xb, eb;
  int i;
  x=(gmpHandle->position*64+gmpHandle->row)*6+gmpHandle->playCount;
  for (i=0; i<50; i++) {
    xb+=(x-xb)*0.05; e=x-xb;
    eb+=(e-eb)*0.02;
    if (-5.0<eb-e && eb-e<5.0) break;
  }
  return (xb+eb)*(1.0/6/64.0);
}
void modplayer_play(Modplayer *m) {
  playhandle=MIDASplayModule(module, 0);
}
void modplayer_stop(Modplayer *m) {
  MIDASstopModule(playhandle);
}
void modplayer_close(Modplayer *m) {
  MIDASfreeModule(module);
  MIDASstopBackgroundPlay();
  MIDASclose();
}


