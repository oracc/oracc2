#include <ctype.h>
#include "scan.h"

unsigned char *
scan_token(Scan *sp, unsigned char *tokp, unsigned char **endtokp, unsigned char *savep)
{
  unsigned char *endp = ++tokp;
  if (isalpha(*endp))
    {
      while (isalpha(*endp))
	++endp;
    }
  else if (endp[1])
    ++endp;
  *savep = *endp;
  *endp = '\0';
  *endtokp = endp;
  return tokp;
}
