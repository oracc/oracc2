#ifndef OSL_UNICODE_H_
#define OSL_UNICODE_H_

#include <roco.h>

typedef struct Osl_unicode
{
  Hash *h;
  Roco *r;
  struct Osl_uentry *e;
} Osl_unicode;

typedef struct Osl_uentry
{
  const char *u;
  const char *o;
  unsigned const char *n;
  int s;
  int seq; /* 0 = codepoint; 1 = sequence */
} Osl_uentry;

extern Osl_unicode *osl_unicode(void);
extern unsigned char *osl_ou_seq_name(Osl_uentry **oup, int noup, Pool *p);
extern Osl_uentry **osl_sequence(Osl_unicode *op, const char *seq, int *noup);
extern unsigned char *osl_seq_name(Osl_unicode *op, const char *seq, Pool *p);
Osl_uentry *osl_autocreate(const char *u);

#endif/*OSL_UNICODE_H_*/
