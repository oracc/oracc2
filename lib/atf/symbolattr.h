#ifndef SYMBOLATTR_
#define SYMBOLATTR_

typedef struct symbolattr
{
  char *symbol;
  char *qualified_id;
  char *pname;
} Symbolattr;

extern void symbolattr_init(void);
extern struct symbolattr *symbolattr_get(const char *txtid, const char *sym);
extern void symbolattr_map(const char *txtid, const char *from, const char *to);
extern void symbolattr_put(const char *txtid, const char *sym, const char *idp, const char *pname);
extern void symbolattr_term(void);

#endif/*SYMBOLATTR_*/
