#include <oraccsys.h>
#include "lx.h"

void
lx_output(Lxfile *lxp, FILE *fp)
{
  int i;
  for (i = 0; i < lxp->nitems; ++i)
    {
      if (lxp->items[i].p)
	{
	  fputs(lxp->items[i].p, fp);
	  fputc(':', fp);
	}
      fputs(lxp->items[i].i, fp);
      if (lxp->items[i].c)
	{
	  fputc('@', fp);
	  fputs(lxp->items[i].c, fp);
	}
      fputc('\n', fp);
    }	
}
