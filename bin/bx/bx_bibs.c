#include <oraccsys.h>
#include "bx.h"

const char **
bx_bibs_file(const char *bibf)
{
  const char **bfs = NULL;
  static unsigned char *fmem;
  if (bibf)
    {
      bfs = (const char **)loadfile_lines3((uccp)bibf, NULL, &fmem);
    }
  else
    {
      free(fmem);
      fmem = NULL;
    }
  return bfs;
}
