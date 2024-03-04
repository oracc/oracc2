#include <oraccsys.h>
#include "isp.h"

char *
nth_line(const char *file, int lnum, int more)
{
  static FILE *fp = NULL;
  static int last_lnum = 0;
  if (file)
    {
      if (!(fp = fopen(file, "r")))
	return NULL;
    }
  else
    {
      if (last_lnum <= lnum)
	rewind(fp);
    }

  unsigned char *ln;
  int lncount = 0;
  while ((ln = loadoneline(fp, NULL)))
    if (++lncount == lnum)
      break;

  if (!more)
    fclose(fp);
  else
    last_lnum = lnum;
  
  return (char*)ln;
}
