#include <ctype.h>
#include "scan.h"

unsigned char *
skip_white(Scan *sp, unsigned char *ftext)
{
  while (isspace(*ftext))
    if ('\n' == *ftext++)
      ++sp->mp->line;
  return ftext;
}
