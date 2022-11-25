typedef struct _Inifile Inifile;
typedef struct _Inientry Inientry;
struct _Inifile { Inientry *main; };
struct _Inientry { char *name, *val; Inientry *next; };
Inifile *lueini(char *fname) {
  FILE *fp=fopen(fname, "r");
  static char buf[257];
  char *cp, *cp2, *n0, *n1, *v0, *v1;
  Inifile *cfg=zgetmem(sizeof(Inifile));
  while (fgets(buf, 256, fp)) {
    cp=strchr(buf, '['); if (cp) *cp='\0';
    cp=strchr(buf, ';'); if (cp) *cp='\0';
    cp=strchr(buf, '\r'); if (cp) *cp='\0';
    cp=strchr(buf, '\n'); if (cp) *cp='\0';
    cp=strchr(buf, '#'); if (cp) *cp='\0';
    for (n0=buf; *n0 && isspace(*n0); n0++) ;
    for (n1=n0; *n1 && !isspace(*n1) && *n1!='='; n1++) ;
    for (v0=n1; *v0 && (isspace(*v0) || *v0=='='); v0++) ;
    for (v1=v0; *v1 && !isspace(*v1); v1++) ;
    if (*n0) {
      Inientry *ent=zgetmem(sizeof(Inientry));
      ent->next=cfg->main; cfg->main=ent;
      ent->name=zgetmem(n1-n0+7&~3);
      ent->val=zgetmem(v1-v0+7&~3);
      for (cp=n0, cp2=ent->name; cp<n1; cp++) *cp2++=tolower(*cp); *cp2='\0';
      for (cp=v0, cp2=ent->val; cp<v1; cp++) *cp2++=tolower(*cp); *cp2='\0';
    }
  }
  fclose(fp);
  return cfg;
}
char *ini_string(Inifile *cfg, char *name, char *def) {
  Inientry *ent;
  for (ent=cfg->main; ent; ent=ent->next) if (strcmp(ent->name, name)==0) return ent->val;
  return def;
}
int ini_int(Inifile *cfg, char *name, int def) {
  char *e; int i=strtol(ini_string(cfg, name, "a"), &e, 0);
  if (*e) return def; else return i;
}
float ini_float(Inifile *cfg, char *name, float def) {
  char *e; float f=strtod(ini_string(cfg, name, "a"), &e);
  if (*e) return def; else return f;
}

