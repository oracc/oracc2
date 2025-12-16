#include <ctype128.h>
#include <string.h>
#include "scan.h"
extern Pool *inl_scan_p;
char *
scan_str_term(Scan *sp, const char *start, const char *match, char **endp)
{
  char *m = NULL;
  if ((m = strstr(start, match)))
    {
      *endp = m+strlen(match);
      int len = m - start;
      char *ret = (char*)pool_alloc(len+1, inl_scan_p);
      strncpy(ret, start, len);
      ret[len] = '\0';
      return ret;
    }
  else
    {
      mesg_verr(&sp->mp, "terminating match string '%s' not found", match);
      return NULL;
    }
}
