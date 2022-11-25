typedef struct _Phile Phile;

void *phile_read(Phile *ph, void *dest, int bytes);
Phile *phile_write(Phile *ph, void *src, int bytes);
Phile *phile_seek(Phile *ph, int pos);
Phile *phile_dseek(Phile *ph, int delta);
int phile_getpos(Phile *ph);
void phile_close(Phile *ph);

#ifdef FILE
Phile *new_phile_disk(FILE *fp);
#endif
Phile *new_phile_disk_ro(char *fname);
Phile *new_phile_disk_create(char *fname);
Phile *new_phile_disk_rw(char *fname);
void df_check(Phile *ph);

