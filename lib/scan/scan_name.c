#include <ctype.h>
#include "scan.h"

char *
scan_name(char *s, char **endp)
{
  char *e = s;
  while (isalnum(*e))
    ++e;
  int len = e-s;
  unsigned char *p = pool_alloc(len + 1, scan_pool);
  strncpy((char*)p, s, len);
  p[len] = '\0';
  if (endp)
    *endp = e;
  return e;
}
