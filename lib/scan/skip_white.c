#include <ctype.h>
#include "scan.h"

unsigned char *
skip_white(unsigned char *ftext, size_t *lnump)
{
  size_t local_lnum = *lnump;
  while (isspace(*ftext))
    if ('\n' == *ftext++)
      ++local_lnum;
  *lnump = local_lnum;
  return ftext;
}
