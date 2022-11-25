#ifndef __MODPLAYER_H__
#define __MODPLAYER_H__

typedef struct _Modplayer Modplayer;

Modplayer *new_modplayer(void *);
  /* parameter is FILE * actually but i dont want to force stdio */
float modplayer_getpos(Modplayer *);
void modplayer_play(Modplayer *);
void modplayer_stop(Modplayer *);
void modplayer_close(Modplayer *);

#endif
