#include <oraccsys.h>
#include "lx.h"

void
lx_output(Lxfile *lxp, const char *outfn)
{
  FILE *outfp = stdout;
  if (outfn && strcmp(outfn, "-"))
    {
      if (!(outfp = fopen(output, "w")))
	{
	  fprintf(stderr, "failed to open output %s: %s\n", output, strerror(errno));
	  exit(1);
	}
    }
  int i;
  for (i = 0; i < lxp->nitems; ++i)
    {
      if (lxp->items[i].p)
	{
	  fputs(lxp->items[i].p, outfp);
	  fputc(':', outfp);
	}
      fputs(lxp->items[i].i, outfp);
      if (lxp->items[i].c)
	{
	  fputc('@', outfp);
	  fputs(lxp->items[i].c, outfp);
	}
      if (lxp->items[i].x)
	fprintf(outfp, "\t%s", lxp->items[i].x);
      fputc('\n', outfp);
    }	
}
