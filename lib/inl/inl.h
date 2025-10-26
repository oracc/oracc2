#ifndef INL_H_
#define INL_H_

#include <scan.h>

typedef enum inl_nym { INL_NO , INL_YES , INL_MAY , INL_TOK } inl_nym;

typedef void (inl_handler)(Tree *tp, Scan *sp, char *text);

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

extern inl_handler inl_bib_h;
extern inl_handler inl_gdl_h;
extern void inl_init(void);
extern void inl_term(void);
extern char *inl_nodes(Node *np, char *s);
extern Tree *inl(char *s);
extern Memo *inl_scan_m;

#endif/*INL_H_*/
