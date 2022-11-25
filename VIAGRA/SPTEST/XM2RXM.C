#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "key.h"
#include "copro.h"
#include "rxm.h"







int dflags = df16bit | dfStereo;
int vol = 12*8;
int amp = 0;
int mixrate = 44100;
int iwflag = 1;
int mastervol = 0;
int maxpat = 0;




//convert mode
#define cBin 1
#define cRxm 2
int rxmconvert = cBin;





typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short word;

#define Xtra  3  //fÅr awe und spline interpolation

#define xmLinear    1
#define xmConverted 2

#define sfLoop      0x08
#define sfPingpong  0x10
#define sf16bit     0x04

#define efOn        0x01
#define efSustain   0x02
#define efLoop      0x04

#define ffNotesmp      1
#define ffVolenv       2
#define ffPanenv       4
#define ffVibrato      8

#define Appregio       0 //. Appregio
#define Portaup        1 //  Porta up
#define Portadn        2 //  Porta down
#define Fportaup       3 //  Fine porta up
#define Fportadn       4 //  Fine prota down
#define Efportaup      5 //  Extra fine porta up
#define Efportadn      6 //  Extra fine porta down
#define Toneporta      7 //  Tone porta
#define Setglisscont   8 //   Set glissando control
#define Vibrato        9 //  Vibrato
#define Setvibspeed   10 //   Set vibrato speed
#define Setvibcont    11 //   Set vibrato control
#define Setvol        12 //. Set volume
#define VolslideV     13 //  Volume slide up/down
#define Volslide      14 //  Volume slide up/down
#define Fvolslide     15 //  Fine volume slide up/down
#define Fvolslideup   16 //  Fine volume slide up
#define Fvolslidedn   17 //  Fine volume slide down
#define Tremolo       18 //  Tremolo
        //            19 //   Set tremolo speed
#define Settrecont    20 //   Set tremolo control
#define Notecut       21 //  Note cut
#define Tremor        22 //  Tremor
#define Setgvol       23 //  Set global volume
#define Gvolslide     24 //  Global volume slide
#define Setpan        25 //. Set panning
#define PanslideV     26 //  Panning slide left/right
#define Panslide      27 //  Panning slide left/right
#define Posjump       28 //. Position jump
#define Patbreak      29 //  Pattern break
#define Patdelay      30 //  Pattern delay
#define Setloop       31 //  Set loop begin/loop
#define Settempo      32 //  Set tempo
#define Setbpm        33 //  Set bpm
#define Sampleofs     34 //. Sample offset
#define Notedelay     35 //  Note delay
#define Keyoff        36 //  Key off
#define Setenvpos     37 //  Set envelope position
#define Retrig        38 //  Retrig
#define Multiretrig   39 //  Multi retrig






char vibconv[] = {0, 3, 1, 2};

//envelope point
struct tpoint {
  word              x, y;
};

typedef struct tpoint tpointar[12];

//xm
struct txmheader1 {
  byte              id[17];        //ID text: 'Extended Module: '
  byte              mname[20];     //Module name, padded with zeroes
  byte              _1a;           //$1a
  byte              tname[20];     //Tracker name
  word              version;       //Version number (0x0104)
};

struct txmheader2 {
// int               hsize;         //Header size
  word              songlen;       //Song length (in patten order table)
  word              restart;       //Restart position
  word              channels;      //Number of channels (2, 4, 6, 8, 10, ..., 32)
  word              patterns;      //Number of patterns (max 256)
  word              instruments;   //Number of instruments (max 128)
  word              flags;         //Flags: bit 0: 0 = Amiga, 1 = Linear
  word              tempo;         //Default tempo
  word              BPM;           //Default BPM
  byte              ptable[256];   //Pattern order table
};


struct txmpatternh {
  //int               hsize;         //Pattern header length
  byte              packing;       //Packing type (always 0)
  word              rows;          //Number of rows in pattern (1..256)
  word              datasize;      //Packed patterndata size
};

struct txminstr {
  //int               size;          //Instrument size << header that is >>
  byte              name[22];      //Instrument name
  byte              type;          //Instrument type (always 0)
  word              samples;       //Number of samples in instrument
  int               samplehsize;   //Sample header size
  byte              notesmp[96];   //Sample number for all notes
  struct tpoint     volpoint[12];  //Points for volume envelope
  struct tpoint     panpoint[12];  //Points for panning envelope
  byte              volpoints;     //Number of volume points
  byte              panpoints;     //Number of panning points
  byte              volsustain;    //Volume sustain point
  byte              volloops;      //Volume loop start point
  byte              volloope;      //Volume loop end point
  byte              pansustain;    //Panning sustain point
  byte              panloops;      //Panning loop start point
  byte              panloope;      //Panning loop end point
  byte              voltype;       //Volume type: bit 0: On; 1: Sustain; 2: Loop
  byte              pantype;       //Panning type: bit 0: On; 1: Sustain; 2: Loop
  byte              vibtype;       //Vibrato type
  byte              vibsweep;      //Vibrato sweep
  byte              vibdepth;      //Vibrato depth
  byte              vibrate;       //Vibrato rate
  word              volfadeout;    //Volume fadeout
};

struct txmsampleh {
  int               length;        //Sample length
  int               loops;         //Sample loop start
  int               loopl;         //Sample loop length
  byte              vol;           //Volume
  sbyte             finetune;      //Finetune (signed byte -128..+127)
  byte              type;          //Type
  byte              pan;           //Panning (0-255)
  sbyte             relnote;       //Relative note number (signed byte)
  byte              freespace;     //Reserved
  byte              name[22];      //Sample name
};







//module



struct theader {
  int               songlen;       //Song length (in patten order table)
  int               restart;       //Restart position
  int               channels;      //Number of channels (2, 4, 6, 8, 10, ..., 32)
  int               patterns;      //Number of patterns (max 256)
  int               instruments;   //Number of instruments (max 128)
  int               tempo;         //Default tempo
  int               BPM;           //Default BPM
  byte              flags;         //Flags: bit 0: 0 = Amiga, 1 = Linear
  byte              ptable[256];   //Pattern order table
};

struct tpatternh {
  int               size;
  int               rows;
};

struct tnote {
  char              note;
  char              instr;
  char              vol;
  char              eff;
  char              param;
};

//instrument
struct tinstrh {
  char              samples;
  char              follows;
};

typedef byte tnotesample[96];

struct tenvelope {
  byte              lastpoint;     //Number of envelope points-1
  byte              sustain;       //Sustain point       (-1 if none)
  byte              loops;         //Loop start point
  byte              loope;         //Loop end point      (-1 if none)
  tpointar          point;         //Points for envelope
};

struct tvibrato {
  byte              type;          //Vibrato type
  byte              sweep;         //Vibrato sweep
  byte              depth;         //Vibrato depth
  byte              rate;          //Vibrato rate
};

struct tsampleh {
  void *            dataptr;       //Sample data pointer
  int               loops;         //Sample loop start
  int               loope;         //Sample loop end (!!!)
  byte              type;          //Type
  sbyte             finetune;      //Finetune (signed byte -64..+63)
  sbyte             relnote;       //Relative note number (signed byte)
  byte              vol;           //Volume
  byte              pan;           //Panning (0-255)
  byte              resample;      //Resample (GUS, AWE)
  int               offset;        //Sample offset (GUS, AWE)
};






file f;
struct theader header;

char used[48];
char use16bit;

struct tsearch search;






void *allocmem(int size) {
  void * mem;
  mem = (void *) malloc(size);
  if (!mem) m_err();
  return (mem);
};

void m_err() {
  printf("Out of memory!");
  exit(1);
};

void f_err() {
  printf("Error reading file!");
  exit(1);
};





char *streammem = NULL;
int streamsize = 0;
int streampos = 0;

void s_checksize(int size) {
  if (size > streamsize) {
   streamsize = size + 65536;
   streammem = (void *) realloc(streammem, streamsize);
   if (streammem == NULL) m_err();
  };
}

void s_write(void *buf, int size) {
  s_checksize(streampos + size);
  memcpy(&streammem[streampos], buf, size);
  streampos += size;
}

int s_getpos() {
  return(streampos);
}

void s_seek(int d) {
  streampos += d;
}

void s_fill(int pos, void *buf, int size) {
  s_checksize(pos + size);
  memcpy(&streammem[pos], buf, size);
}

void s_free() {
  free(streammem);
  streammem = NULL;
  streamsize = 0;
  streampos = 0;
}






//sample container
struct tsamplelink {
  void *sample;
  int len;
  int spos;
  struct tsamplelink *next;
};

struct tsamplelink *head = NULL;

void addsample(void *sample, int len, int spos) {
  struct tsamplelink *slink, *current;

  slink = (struct tsamplelink *) allocmem(sizeof(struct tsamplelink));
  slink->sample = sample;
  slink->len = len;
  slink->spos = spos;
  slink->next = NULL;

  if (!head) {
    head = slink;
  } else {
    current = head;
    while (current->next) current = current->next;
    current->next = slink;
  }
}

void freesamples() {
  struct tsamplelink *del, *current;

  current = head;
  while (current) {
    del = current;
    current = current->next;

    free(del->sample);
    free(del);
  }
  head = NULL;
}





void xmheader1load() {
  //xm
  struct txmheader1 xmheader1;
  char name[21];

  //xm header1
  f_read(&f, &xmheader1, sizeof(xmheader1));
  if (f.error) f_err();
  if (memcmp(xmheader1.id, "Extended Module: ", 17)) {
   printf("File is no XM\n");
   exit(3);
  };

  memcpy(name, xmheader1.mname, 20);
  name[20] = '\0';
  printf("Song name   : %s\n", name);

};





void xmheader2load() {
  //xm
  int hsize;
  struct txmheader2 *xmheader2;


  //xm header2
  f_read(&f, &hsize, sizeof(hsize));
  hsize -= sizeof(hsize);
  xmheader2 = allocmem(hsize);
  f_read(&f, xmheader2, hsize);
  if (f.error) f_err();

  //module header
  header.songlen     = xmheader2->songlen;
  header.restart     = xmheader2->restart;
  header.channels    = xmheader2->channels;
  header.patterns    = xmheader2->patterns;
  header.instruments = xmheader2->instruments;
  header.tempo       = xmheader2->tempo;
  header.BPM         = xmheader2->BPM;

  header.flags = xmheader2->flags & xmLinear;
  if (rxmconvert == cPlay || rxmconvert == cWav) {
   header.flags |= xmConverted;
  }

  memcpy(header.ptable, xmheader2->ptable, 256);

  if (maxpat && header.songlen > maxpat) header.songlen = maxpat;
  if (header.restart >= header.songlen) header.restart = header.songlen-1;

  s_seek(sizeof(struct theader) -256+header.songlen);

  free(xmheader2);

  //print
  printf("Song length : %d\n", header.songlen);
  printf("Restart     : %d\n", header.restart);
  printf("Channels    : %d\n", header.channels);
  printf("Patterns    : %d\n", header.patterns);
  printf("Instruments : %d\n", header.instruments);
  printf("Freq. table : %s\n", (header.flags & 1) ? "Linear" : "Amiga");
  printf("Tempo       : %d\n", header.tempo);
  printf("BPM         : %d\n", header.BPM);
}






char tosigned(char param) {
  return( (param & 0xF0) ? (param >> 4) : -(param & 0x0F) );
}





struct tr {
  int e, p;
};





void convnote(struct tnote note) {
  char effekt;
  char param;

  struct tr r[8];
  int c = 0;

  int z;

  c = 0;

  //Note delay
  if ((note.eff == 0x0E) & ((note.param & 0xF0) == 0xD0)) {
   r[c].e = Notedelay;
   r[c++].p = note.param & 0x0F;
  }
  //Tone porta
  if (note.eff == 3) {
   r[c].e = Toneporta;
   r[c++].p = note.param;
  }
  if (note.eff == 5) {
   r[c].e = Toneporta;
   r[c++].p = 0;
  }
  if (note.vol >= 0xF0) {
   r[c].e = Toneporta;
   r[c++].p = note.vol << 4;
  }

  // note & instrument
  if ((note.note || note.instr) && (note.note < 97)) {
   r[c].e = 48;
   r[c++].p = 0;
  }
  if (note.note == 97) {
   note.note = 0;
   r[c].e = Keyoff;
   r[c++].p = 0;
  }

  param = note.vol & 0x0F;
  switch(note.vol >> 4) {

   case 0x1: //Set volume
             r[c].e = Setvol;
             r[c++].p = param;
             break;
   case 0x2:
   case 0x3:
   case 0x4: r[c].e = Setvol;
             r[c++].p = note.vol-0x10;
             break;
   case 0x5: r[c].e = Setvol;
             r[c++].p = 0x40;
             break;
   case 0x6: //Volume slide down
             r[c].e = VolslideV;
             r[c++].p = -param;
             break;
   case 0x7: //Volume slide up
             r[c].e = VolslideV;
             r[c++].p = param;
             break;
   case 0x8: //Fine volume slide down
             r[c].e = Fvolslide;
             r[c++].p = -param;
             break;
   case 0x9: //Fine volume slide up
             r[c].e = Fvolslide;
             r[c++].p = param;
             break;
   case 0xA: //Set vibrato speed
             if (param) {
               r[c].e = Setvibspeed;
               r[c++].p = param;
             }
             break;
   case 0xB: //Vibrato
             r[c].e = Vibrato;
             r[c++].p = param;
             break;
   case 0xC: //Set panning
             r[c].e = Setpan;
             r[c++].p = param * 0x11;
             break;
   case 0xD: //Panning slide left
             r[c].e = PanslideV;
             r[c++].p = -param;
             break;
   case 0xE: //Panning slide right
             r[c].e = PanslideV;
             r[c++].p = param;
             break;
  }

  effekt = note.eff;
  param = note.param;
  if (note.eff == 0x0E) {
   effekt = 0xE0 + (param >> 4);
   param &= 0x0F;
  }

  switch(effekt) {
   case 0x00: //Appregio
              if (param) {
                r[c].e = Appregio;
                r[c++].p = param;
              }
              break;
   case 0x01: //Porta up
              r[c].e = Portaup;
              r[c++].p = param;
              break;
   case 0x02: //Porta down
              r[c].e = Portadn;
              r[c++].p = param;
              break;
   case 0x04: //Vibrato (Speed, Depth)
              r[c].e = Vibrato;
              r[c++].p = param;
              break;
   case 0x05: //Tone porta+Volume slide (slide speed)
              r[c].e = Volslide;
              r[c++].p = tosigned(param);
              break;
   case 0x06: //Vibrato+Volume slide (slide speed)
              r[c].e = Vibrato;
              r[c++].p = 0;
              r[c].e = Volslide;
              r[c++].p = tosigned(param);
              break;
   case 0x07: //Tremolo (Speed, Depth)
              r[c].e = Tremolo;
              r[c++].p = param;// & 0x0F;
              break;
   case 0x08: //Set panning
              r[c].e = Setpan;
              r[c++].p = param;
              break;
   case 0x09: //Sample offset (param speichern und ausfÅhren nur wenn Note)
              if (note.note) {
                r[c].e = Sampleofs;
                r[c++].p = param;
              }
              break;
   case 0x0A: //Volume slide
              r[c].e = Volslide;
              r[c++].p = tosigned(param);
              break;
   case 0x0B: //Position jump
              r[c].e = Posjump;
              r[c++].p = param;
              break;
   case 0x0C: //Set volume
              r[c].e = Setvol;
              r[c++].p = (param <= 0x40) ? param : 0x40;
              break;
   case 0x0D: //Pattern break
              r[c].e = Patbreak;
              r[c++].p = (param >> 4)*10 + (param & 0x0F);
              break;
   case 0x0F: //Set tempo/BPM
              if (param < 32) {
                //tempo
                r[c].e = Settempo;
                r[c++].p = param;
              } else {
                //BPM
                r[c].e = Setbpm;
                r[c++].p = param;
              }
              break;
   case 0x10: //Set global volume
              r[c].e = Setgvol;
              r[c++].p = (param <=64) ? param : 64;
              break;
   case 0x11: //Global volume slide
              r[c].e = Gvolslide;
              r[c++].p = tosigned(param);
              break;
   case 0x14: //Key off
              r[c].e = Keyoff;
              r[c++].p = 0;
              break;
   case 0x15: //Set envelope position
              r[c].e = Setenvpos;
              r[c++].p = param;
              break;
   case 0x19: //Panning slide
              r[c].e = Panslide;
              r[c++].p = tosigned(param);
              break;
   case 0x1B: //Multi retrig note
              r[c].e = Multiretrig;
              r[c++].p = param;
              break;
   case 0x1D: //Tremor
              r[c].e = Tremor;
              r[c++].p = param;
              break;
   case 0x21: //Extra fine porta up(=1) / down(=2)
              if ((param & 0xF0) == 0x10) {
                r[c].e = Efportaup;
                r[c++].p = param & 0x0F;
              }
              if ((param & 0xF0) == 0x20) {
                r[c].e = Efportadn;
                r[c++].p = param & 0x0F;
              }
              break;
   case 0xE1: //Fine porta up
              r[c].e = Fportaup;
              r[c++].p = param;
              break;
   case 0xE2: //Fine porta down
              r[c].e = Fportadn;
              r[c++].p = param;
              break;
   case 0xE3: //Set gliss control
              r[c].e = Setglisscont;
              r[c++].p = param;
              break;
   case 0xE4: //Set vibrato control
              r[c].e = Setvibcont;
              r[c++].p = (param & 4) | vibconv[param & 3];
              break;
   case 0xE5: //Set finetune
              break;
   case 0xE6: //Set loop begin/loop
              r[c].e = Setloop;
              r[c++].p = param;
              break;
   case 0xE7: //Set tremolo control
              r[c].e = Settrecont;
              r[c++].p = (param & 4) | vibconv[param & 3];;
              break;
   case 0xE9: //Retrig note (param = 0: retrig bei tick 0)
              r[c].e = Retrig;
              r[c++].p = param;
              break;
   case 0xEA: //Fine volume slide up
              r[c].e = Fvolslideup;
              r[c++].p = param;
              break;
   case 0xEB: //Fine volume slide down
              r[c].e = Fvolslidedn;
              r[c++].p = param;
              break;
   case 0xEC: //Note cut
              r[c].e = Notecut;
              r[c++].p = param;
              break;
   case 0xEE: //Pattern delay
              r[c].e = Patdelay;
              r[c++].p = param;
              break;
  };

  //anzahl effekte

  s_write(&c, 1);
  for(z=0;z < c; z++) {
   effekt = r[z].e;

   if (effekt == 48) {
     //note & instrument
     note.note |= 0x80;
     s_write(&note.note, 1);
     s_write(&note.instr, 1);
   } else {
     used[effekt] = 1;
     s_write(&effekt, 1);
     s_write(&r[z].p, 1);
   };
  };
};








void xmpatternload() {
  int    i, pz;
  //xm
  int    hsize;
  int    xmpatterns;
  struct txmpatternh *xmpatternh;
  char   *pack;
  int    packpos;
  //module
  int    patterns;  //actually used patterns
  int    streampos;
  struct tpatternh patternh;
  char   note[5];
  char   b;


  xmpatterns = header.patterns;
  patterns = 0;
  for(i=0; i < header.songlen; i++) {
   if (header.ptable[i] >= patterns) patterns = header.ptable[i]+1;
  }
  if (xmpatterns > patterns) header.patterns = patterns;

  //print
// printf("Unused pat. : %d\n", xmpatterns-patterns);


  for(pz=0; pz < xmpatterns; pz++) {
   //xm pattern header
   f_read(&f, &hsize, sizeof(hsize));
   hsize -= sizeof(hsize);

//printf("hsize : %2d  ", hsize);
   xmpatternh = allocmem(hsize);
   f_read(&f, xmpatternh, hsize);

   //xm pattern data
//printf("datasize : %5d  ", xmpatternh->datasize);

   if (xmpatternh->datasize) {
     pack = allocmem(xmpatternh->datasize);
     f_read(&f, pack, xmpatternh->datasize);
     if (pz < patterns) {
       //unpack pattern
       streampos = s_getpos();
       s_seek(sizeof(struct tpatternh));

       packpos = 0;
       while(packpos < xmpatternh->datasize) {
         if (((b = pack[packpos]) & 0x80) != 0) packpos++; else b = 0x1F;
         for(i=0; i < 5; i++) {
           note[i] = (b & 1 == 1) ? pack[packpos++] : 0;
           b >>= 1;
         };
         convnote(*(struct tnote *) &note);
         //printnote(note);
       };

       patternh.rows = xmpatternh->rows;
       patternh.size = s_getpos() - streampos;
//printf("new size : %5d", patternh.size);
       s_fill(streampos, &patternh, sizeof(struct tpatternh));

     }
     free(pack);

   } else {
     if (pz < patterns) {
       patternh.rows = xmpatternh->rows;
       patternh.size = sizeof(struct tpatternh);
//printf("new size : %5d", patternh.size);
       s_write(&patternh, sizeof(struct tpatternh));
     }
   }

   free(xmpatternh);

//printf("\n");

//     printf("Pattern     : %d\n", pz);
//     printf("Rows        : %d\n", pattern->rows);
//     if ((datapos+5) % (xm.header.channels * 5) == 0) printf("\n");

  if (f.error) f_err();
  }
}







void xminstrload() {
  int z, iz, sz;
  //xm
  int hsize;
  struct txminstr *xminstr;
  struct txmsampleh *xmsampleh;
  //module
  int instruments = 0;
  int streampos;
  struct tinstrh instrh;
  struct tsampleh sampleh[16];
  byte   smpused[16];
  struct tenvelope envelope;
  struct tvibrato vibrato;
  //samples
  int len;
  sbyte *bdata;
  byte b;
  word *wdata;
  word w;

  char s[23];

  //instruments
  for(iz = 0; iz < header.instruments; iz++) {

   //xm instrument
   f_read(&f, &hsize, sizeof(hsize));
   hsize -= sizeof(hsize);
   xminstr = allocmem(hsize);
   f_read(&f, xminstr, hsize);

//print
/*
memcpy(s, xminstr->name, 22);
s[22] = 0;
for(z = 0; z < 22; z++) if (s[z] < ' ') s[z] = ' ';
printf("Instr. %02X   : %s\n", iz, s);
*/
   instrh.samples = xminstr->samples;
   instrh.follows = 0;

   memset(sampleh, 0, sizeof(sampleh));
   memset(smpused, 0, sizeof(smpused));

   for(sz = 0; sz < instrh.samples; sz++) {
     xmsampleh = allocmem(xminstr->samplehsize);
     //xm sample header
     f_read(&f, xmsampleh, xminstr->samplehsize);

     b = 0;
     if (xmsampleh->type & 16) b |= sf16bit;
     if (xmsampleh->type &  1) b |= sfLoop;
     if (xmsampleh->type &  2) b |= sfLoop | sfPingpong;
     sampleh[sz].type = b;

     //loop length = 0: no loop
     if (xmsampleh->loopl == 0) sampleh[sz].type &= ~(sfLoop | sfPingpong);

     if (sampleh[sz].type & sfLoop) {
       //loop
       sampleh[sz].loops = xmsampleh->loops;
       sampleh[sz].loope = xmsampleh->loops+xmsampleh->loopl;
     } else {
       //no loop
       sampleh[sz].loops = 0;
       sampleh[sz].loope = xmsampleh->length;
     };

     sampleh[sz].offset   = xmsampleh->length; //offset = length
     sampleh[sz].vol      = xmsampleh->vol;
     sampleh[sz].finetune = xmsampleh->finetune /2;
     sampleh[sz].relnote  = xmsampleh->relnote;
     sampleh[sz].pan      = xmsampleh->pan;

//print
//memcpy(s, xmsampleh->name, 22);
//s[22] = 0;
//for(z = 0; z < 22; z++) if (s[z] < ' ') s[z] = ' ';
//printf("Sample  %1X   : %s\n", sz, s);

     if (xmsampleh->length) {
       for(z = 0; z < 96; z++) if (xminstr->notesmp[z] == sz) smpused[sz] = 1;
     }
     free(xmsampleh);
   }//sample header loop (0-15)

   for(sz = instrh.samples -1; sz >= 0 ;sz--) {
     if (!smpused[sz]) {
       for(z = 0; z < 96; z++) {
         b = xminstr->notesmp[z];
         if (b > sz) xminstr->notesmp[z]--;
         if (b == sz) xminstr->notesmp[z] = 15;
       }
       instrh.samples--;
     }
   }

   //module instrument
   if (instrh.samples) {

     //add empty instruments if iz > instruments
     while (iz > instruments++) s_write("\0\0", 2);

     //alloc mem for instrument header and keep pos
     streampos = s_getpos();
     s_seek(sizeof(struct tinstrh));

     //sample number for all notes
     b = 0;
     for (z = 0; z < 96; z++) if (xminstr->notesmp[z]) b = 1;
     if (b) {
       s_write(xminstr->notesmp, sizeof(tnotesample));
       instrh.follows |= ffNotesmp;
     }

     //volume envelope
     if ((xminstr->voltype & efOn) && (xminstr->volpoints)) {
       envelope.lastpoint = xminstr->volpoints-1;
       envelope.sustain = (xminstr->voltype & efSustain)
                           ? (xminstr->volsustain) : -1;
       if (xminstr->voltype & efLoop) {
         envelope.loops = xminstr->volloops;
         envelope.loope = xminstr->volloope;
       } else {
         envelope.loops = 0;
         envelope.loope = -1;
       }
       memcpy(envelope.point, xminstr->volpoint, sizeof(envelope.point));
       //x-werte im envelope sind differenzen
       for(z = 11; z > 0; z--) envelope.point[z].x -=envelope.point[z-1].x;
       for(z = 11; z >=0; z--) envelope.point[z].y *= 4;

       s_write(&envelope, sizeof(struct tenvelope)-(11-envelope.lastpoint)*sizeof(struct tpoint));
       s_write(&xminstr->volfadeout, 2);
       instrh.follows |= ffVolenv;
     }
     //panning envelope
     if ((xminstr->pantype & efOn) && (xminstr->panpoints)) {
       envelope.lastpoint = xminstr->panpoints-1;
       envelope.sustain = (xminstr->pantype & efSustain)
                           ? (xminstr->pansustain) : -1;
       if (xminstr->pantype & efLoop) {
         envelope.loops = xminstr->panloops;
         envelope.loope = xminstr->panloope;
       } else {
         envelope.loops = 0;
         envelope.loope = -1;
       }
       memcpy(envelope.point, xminstr->panpoint, sizeof(envelope.point));
       //x-werte im envelope sind differenzen
       for(z = 11; z > 0; z--) envelope.point[z].x -=envelope.point[z-1].x;
       for(z = 11; z >=0; z--) envelope.point[z].y *= 4;

       s_write(&envelope, sizeof(struct tenvelope)-(11-envelope.lastpoint)*sizeof(struct tpoint));
       instrh.follows |= ffPanenv;
     }
     //vibrato
     if (xminstr->vibdepth) {
       vibrato.type   = xminstr->vibtype;
       vibrato.sweep  = xminstr->vibsweep;
       vibrato.depth  = xminstr->vibdepth;
       vibrato.rate   = xminstr->vibrate;

       s_write(&vibrato, sizeof(struct tvibrato));
       instrh.follows |= ffVibrato;
     }

     s_fill(streampos, &instrh, sizeof(struct tinstrh));

   }//number of samples > 0

   free(xminstr);

   //samples
   for (sz = 0; sz < 16; sz++) {
     //xm sample data
     if (smpused[sz]) {
       //sample used
       if (!(sampleh[sz].type & sf16bit)) {
         //8 bit
         len = (sampleh[sz].loope + Xtra + 1) & ~1;
         //read data
         bdata = allocmem(len);
         f_read(&f, bdata, sampleh[sz].loope);
         f_seek(&f, f_getpos(&f) + sampleh[sz].offset - sampleh[sz].loope); //offset is original length

         //delta -> real values
         b = 0;
         for (z = 0; z < sampleh[sz].loope; z++) {
           b = (bdata[z] += b);
         }

       } else {
         //16 bit
         len = sampleh[sz].loope + Xtra*2;
         //read data
         wdata = allocmem(len);
         bdata = (sbyte *) wdata;
         f_read(&f, wdata, sampleh[sz].loope);
         f_seek(&f, f_getpos(&f) + sampleh[sz].offset - sampleh[sz].loope); //offset is original length

         //loop-points in samples (not bytes)
         sampleh[sz].loops >>= 1;
         sampleh[sz].loope >>= 1;

         //clear extra bytes
         for (z = 0; z < Xtra; z++) wdata[sampleh[sz].loope + z] = 0;

         //delta -> real values
         w = 0;
         for(z = 0; z < sampleh[sz].loope + Xtra; z++) {
           w = (wdata[z] += w);
         }

       }

       if (!(sampleh[sz].type & sf16bit)) {
         //8 bit

         //set extra bytes
         for (; z < len; z++) bdata[z] = b;

         if ((sampleh[sz].type & (sfLoop | sfPingpong)) == sfLoop) {
           for(z = 0; z < Xtra; z++)
             bdata[sampleh[sz].loope + z] = bdata[sampleh[sz].loops + z];
         }

         if (rxmconvert == cBin || rxmconvert == cRxm) {
           //real -> delta values
           for(z = sampleh[sz].loope + Xtra -1; z >= 1; z--) {
             bdata[z] -= bdata[z-1];
           }
         }

       } else {
         //16 bit
         use16bit = 1;

         if ((sampleh[sz].type & (sfLoop | sfPingpong)) == sfLoop) {
           for(z = 0; z < Xtra; z++)
             wdata[sampleh[sz].loope + z] = wdata[sampleh[sz].loops + z];
         }

         if (rxmconvert == cBin || rxmconvert == cRxm) {
           //real -> delta values
           for(z = sampleh[sz].loope + Xtra -1; z >= 1; z--) {
             wdata[z] -= wdata[z-1];
           }
         }
       }

       addsample(bdata, len, s_getpos());
       sampleh[sz].dataptr = bdata;
       sampleh[sz].offset = 0;
       s_write(&sampleh[sz], sizeof(struct tsampleh));
     } else {
       //sample not used
       f_seek(&f, f_getpos(&f) + sampleh[sz].offset); //offset is length
     }

   } //sample loop (0-15)

  if (f.error) f_err();
  } //instrument loop (0-127)

  header.instruments = instruments;
  s_fill(0, &header, sizeof(struct theader) - 256 + header.songlen);
};







int writedata(file *wf, int write_length) {
  struct tsamplelink *current;
  int ofs, rel;
  int datalen;

  datalen = (s_getpos() + 1) & ~1;
  //generate sample offsets
  current = head;
  ofs = datalen;
  while(current) {
    rel = ofs - current->spos;
    s_fill(current->spos, &rel, 4);
    ofs += current->len;
    current = current->next;
  }

  //write length
  if (write_length) f_write(wf, &ofs, 4);

  //write xm data
  f_write(wf, streammem, datalen);

  //write samples
  current = head;
  while(current) {
    f_write(wf, current->sample, current->len);
    current = current->next;
  }
  return (ofs - datalen);
}





void writefx(file *wf, int t, char *name) {
  if (t) f_write(wf, ";", 1);
  f_write(wf, name, strlen(name));
  f_write(wf, " = 1\15\12", 6);
}





void writebin(char *rxmname) {
  file wf;
  char *p;
  int len;

  p = (char *) strchr(rxmname, '.');
  if (p) *p = '\0';
  strcat(rxmname, ".bin");

  f_open(&wf, rxmname, ofCreate);
  len = writedata(&wf, 0);
  f_close(&wf);

  f_open(&wf, "fx.inc", ofCreate);
  writefx(&wf, used[0] != 0, "noAppregio");
  writefx(&wf, used[7] != 0, "noToneporta");
  writefx(&wf, used[9] != 0, "noVibrato");
  writefx(&wf, used[18] != 0, "noTremolo");
  writefx(&wf, used[21] != 0, "noNotecut");
  writefx(&wf, used[22] != 0, "noTremor");
  writefx(&wf, used[24] != 0, "noGvolslide");
  writefx(&wf, used[28] != 0, "noPosjump");
  writefx(&wf, used[29] != 0, "noPatbreak");
  writefx(&wf, used[30] != 0, "noPatdelay");
  writefx(&wf, used[31] != 0, "noSetloop");
  writefx(&wf, used[32] != 0, "noSettempo");
  writefx(&wf, used[34] != 0, "noSampleofs");
  writefx(&wf, used[35] != 0, "noNotedelay");
  writefx(&wf, used[36] != 0, "noKeyoff");
  writefx(&wf, used[37] != 0, "noSetenvpos");
  writefx(&wf, used[38] != 0, "noRetrig");
  writefx(&wf, used[39] != 0, "noMultiretrig");
  writefx(&wf, use16bit != 0, "no16bit");
  writefx(&wf, len > 262160, "noResample");

  writefx(&wf, 0, "no2tabs");
  writefx(&wf, header.flags & 1, "noLinear");
  writefx(&wf, !(header.flags & 1), "noAmiga");

  f_close(&wf);
}





void writerxm(char *rxmname) {
  file wf;
  char *p;

  p = (char *) strchr(rxmname, '.');
  if (p) *p = '\0';
  strcat(rxmname, ".rxm");

  f_open(&wf, rxmname, ofCreate);
  writedata(&wf, 1);
  f_close(&wf);
}




int parse(char *s, int mul) {
  int r = 0;

  while (*s && (*s < '0' || *s > '9')) s++;
  while (*s >= '0' && *s <= '9') {
    r *= mul;
    r += *s - '0';
    s++;
  }
  return r;
}





int checkenv(tdinfo *dinfo) {
  int i;
  i = dinfo->base >= 0x200 && dinfo->base <= 0x290;
  i &= dinfo->irq >= 2 && dinfo->irq <= 15;
  i &= dinfo->dma1 <= 7;
  i &= dinfo->dma2 <= 7;
  return i;
}





int gusenv(tdinfo *dinfo, int flags, int iwflag) {
  char *s;

  memset(dinfo, 0, sizeof(tdinfo));
  s = getenv("ULTRASND");
  dinfo->base = parse(s, 16);
  s = strchr(s, ', ');
  if (!s) return 0;
  dinfo->dma1 = parse(s, 10);
  s = strchr(s+1, ', ');
  if (!s) return 0;
  dinfo->dma2 = parse(s, 10);
  s = strchr(s+1, ', ');
  if (!s) return 0;
  dinfo->irq = parse(s, 10);

  if (iwflag >= 2 || (iwflag == 1 && getenv("INTERWAVE"))) dinfo->flags |= gus_iw;
  dinfo->flags |= flags;

  return checkenv(dinfo);
}





int sbenv(tdinfo *dinfo, int rate, int flags) {
  char *s;
  char *v;

  memset(dinfo, 0, sizeof(tdinfo));
  s = getenv("BLASTER");
  v = strchr(s, 'A');
  if (v) dinfo->base = parse(v, 16);
  v = strchr(s, 'I');
  if (v) dinfo->irq = parse(v, 10);
  v = strchr(s, 'D');
  if (!v) return 0;
  dinfo->dma1 = parse(v, 10);
  v = strchr(s, 'H');
  if (v) dinfo->dma2 = parse(v, 10);
  dinfo->rate = rate;
  dinfo->flags = flags;
  return checkenv(dinfo);
}







void parseerr(char c) {
  printf("Missing value for parameter -%c\n", c);
  exit(1);
}






void main(int argn, char **argv) {
  char xmname[80];
  char *param;
  int doserr, i;
  char c;


  printf("\nxm to rxm converter and player by digisnap\n\n");

  strcpy(xmname, "*.xm");
  i = 1;
  while (i < argn) {
   param = argv[i];
   if (strchr(param, '.')) strcpy(xmname, param);
   else {
     if (param[0] == '-') {
       do {
         param++;
         switch(*param) {
          case 'v' : i++;
                     if (i >= argn) parseerr(*param);
                     vol = atoi(argv[i])*8;
                     if (vol > 256) vol = 256;
                     break;
          case 'r' : i++;
                     if (i >= argn) parseerr(*param);
                     mixrate = atoi(argv[i]);
                     break;
          case 'P' : i++;
                     if (i >= argn) parseerr(*param);
                     maxpat = atoi(argv[i]);
                     break;
          case 'R' : dflags |= dfReverse;
                     break;
          case 'c' : rxmconvert = cWav;
                     break;
          case 'f' : dflags |= wav_lin;
                     rxmconvert = cWav;
                     break;
          case '3' : dflags |= wav_32bit;
                     rxmconvert = cWav;
                     break;
          case 'p' : if (amp == 0) amp = 100;
                     rxmconvert = cWav;
                     break;
          case 'A' : i++;
                     if (i >= argn) parseerr(*param);
                     amp = atoi(argv[i]);
                     rxmconvert = cWav;
                     break;
          case 'u' : iwflag = 0;                   //gus mode
                     break;
          case 'i' : iwflag = 2;                   //iw mode
                     break;
          case 'm' : dflags &= ~dfStereo;          //mono 8 bit
          case '8' : dflags &= ~df16bit;           //8 bit
                     break;
          case '1' : dflags |= sb_dsp1;            //DSP1xx
                     break;
          case 'a' : dflags |= sb_awe;
                     break;
          case 'M' : i++;
                     if (i >= argn) parseerr(*param);
                     mastervol = atoi(argv[i]);
                     if (vol > 255) mastervol = 255;
                     break;
          case 'b' : rxmconvert = cBin;
                     break;
          case 'B' : rxmconvert = cRxm;
                     break;
          default  : if (*param > 32) {
                       printf("Unknown parameter -%c\n", *param);
                       exit(1);
                     }
         }
       } while (*param != 0);
     } else {
       strcpy(xmname, param);
       strcat(xmname, ".xm");
     }
   }
   i++;
  }

  doserr = f_findfirst(xmname, faArchive, &search);

  while (!doserr) {

   for(i = strlen(xmname); i > 0; i--) if (xmname[i] == '\\') break;
   if (i > 0) i++;
   xmname[i] = '\0';
   strcat(xmname, search.name);

   f_open(&f, xmname, ofRead);
   if (f.error) f_err();

   //init
   memset(used, 0, sizeof(used));
   use16bit = 0;

   //load
   xmheader1load();
   xmheader2load();
   xmpatternload();
   xminstrload();
   f_close(&f);

   c = 27;
   if (rxmconvert == cBin) {
     writebin(xmname);
   } else if (rxmconvert == cRxm) {
     writerxm(xmname);
   }

   if (c == 27) break;

   s_free();
   freesamples();
   doserr = f_findnext();
  }
}



