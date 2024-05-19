#include <oraccsys.h>
#include <xmlify.h>
#include "ce.h"

const char *
ce_h(const char *h)
{
  char *x = strdup((ccp)xmlify((ucp)h));
  char *t = x;
  while (*t)
    {
      if ('_' == *t) *t++ = ' ';
      else if ('\t' == *t) *t++ = '/';
      else ++t;
    }
  return x;
}
