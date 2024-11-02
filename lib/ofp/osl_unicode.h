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
extern Osl_uentry **osl_sequence(const char *seq);

#endif/*OSL_UNICODE_H_*/
