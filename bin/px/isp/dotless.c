#include <oraccsys.h>
#include "isp.h"

const char *
dotless(const char *i)
{
  const char *d;
  if ((d = strchr(i, '.')))
    {
      char r[strlen(i)+1];
      strncpy(r, i, d-i);
      r[d-i] = '\0';
      return strdup(r);
    }
  else
    return i;
}
