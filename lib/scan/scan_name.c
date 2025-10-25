#include <ctype.h>
#include "scan.h"

char *
scan_name(char *s)
{
  char *e = s;
  while (isalnum(*e))
    ++e;
  int len = e-s;
  unsigned char *p = pool_alloc(len + 1, scan_pool);
  strncpy((char*)p, s, len);
  p[len] = '\0';
  return e;
}
