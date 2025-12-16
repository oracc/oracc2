#ifndef INL_H_
#define INL_H_

#include <scan.h>

typedef enum inl_nym { INL_NO , INL_YES , INL_MAY , INL_TOK } inl_nym;

typedef void (inl_handler)(Scan *sp, Scanseg *ssp, char *text);
typedef int (*inl_self_func_p)(const char *name);

struct inltok
{
  const char *name;
  nscode ns;
  inl_nym arg;
  inl_nym txt;
  const char *term;
  const char *tag;
  const char *attr;
  inl_handler *h;
};
extern struct inltok *inltok(register const char *str, register size_t len);

struct inl_self_tab { const char *name; };

extern inl_self_func_p inl_self_func;
extern Memo *inl_scanseg_m;
extern Pool *inl_scan_p;
extern inl_handler inl_bib_h;
extern inl_handler inl_gdl_h;
extern void inl_self_set(inl_self_func_p f);
extern void inl_init(void);
extern void inl_term(void);
extern char *inl_nodes(Scan *sp, Node *np, char *s);
extern Tree *inl(Mloc *mp, char *s);

#endif/*INL_H_*/
