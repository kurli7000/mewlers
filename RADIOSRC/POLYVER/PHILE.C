#include <stdlib.h>
#include "phile.h"
#include <lib2\mem.h>
typedef struct _Phile Phile;
typedef struct _Diskphile Diskphile;

struct _Phile {
  void (*read)(Phile *ph, void *dest, int bytes);
  void (*write)(Phile *ph, void *src, int bytes);
  void (*seek)(Phile *ph, int pos);
  int (*getpos)(Phile *ph);
  void (*close)(Phile *ph);
};


#include <stdio.h>
struct _Diskphile { Phile;
  FILE *fp;
};
void df_read(Phile *ph, void *dest, int bytes) { fread(dest, bytes, 1, ((Diskphile*)ph)->fp); }
void df_write(Phile *ph, void *src, int bytes) { fwrite(src, bytes, 1, ((Diskphile*)ph)->fp); }
void df_seek(Phile *ph, int pos) { fseek(((Diskphile*)ph)->fp, pos, SEEK_SET); }
int df_getpos(Phile *ph) { long i; fgetpos(((Diskphile*)ph)->fp, &i); return i; }
void df_close(Phile *ph) { fclose(((Diskphile*)ph)->fp); }


void *phile_read(Phile *ph, void *dest, int bytes) { ph->read(ph, dest, bytes); return dest; }
Phile *phile_write(Phile *ph, void *src, int bytes) { ph->write(ph, src, bytes); return ph; }
Phile *phile_seek(Phile *ph, int pos) { ph->seek(ph, pos); return ph; }
Phile *phile_dseek(Phile *ph, int delta) { ph->seek(ph, ph->getpos(ph)+delta); return ph; }
int phile_getpos(Phile *ph) { return ph->getpos(ph); }
void phile_close(Phile *ph) { ph->close(ph); }


void df_check(Phile *ph) {
  _asm {
    mov eax, 3
    int 10h
  };
  printf("%x %x\n", df_read, ph->read);
  exit(1);
}


Phile *new_phile_disk(FILE *fp) {
  Diskphile *df=zgetmem(sizeof(Diskphile));
  df->read=df_read; df->write=df_write; df->seek=df_seek;
  df->close=df_close; df->getpos=df_getpos; df->fp=fp;
  return (Phile*)df;
}
Phile *new_phile_disk_ro(char *fname) {
  FILE *fp=fopen(fname, "rb");
  return (Phile*)new_phile_disk(fp);
}
Phile *new_phile_disk_create(char *fname) {
  FILE *fp=fopen(fname, "wb");
  return (Phile*)new_phile_disk(fp);
}
Phile *new_phile_disk_rw(char *fname) {
  FILE *fp=fopen(fname, "r+b");
  return (Phile*)new_phile_disk(fp);
}








