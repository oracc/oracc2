#include <oraccsys.h>
#include "isp.h"

char *
nth_line(Isp *ip, const char *file, int lnum, int more)
{
  static FILE *fp = NULL;
  static int fname = NULL;
  static int last_lnum = 0;

  if (more < 0)
    {
      if (fp)
	{
	  fclose(fp);
	  fp = NULL;
	  fname = NULL;
	}
      return NULL;
    }
  if (file)
    {
      if (!(fp = fopen(file, "r")))
	{
	  ip->err = PX_ERROR_START "nth_line: failed to open %s\n";
	  ip->errx = (ccp)pool_copy((ucp)file, ip->p);
	  return NULL;
	}
      else
	fname = (ccp)pool_copy((ucp)file, ip->p);
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

  if (!ln)
    {
      ip->err = "nth_line: failed to load line from %s\n";
      ip->errx = fname;
    }
  
  if (!more)
    fclose(fp);
  else
    last_lnum = lnum;
  
  return (char*)ln;
}
