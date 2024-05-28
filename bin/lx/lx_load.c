#include <oraccsys.h>
#include "lx.h"

Lxfile *
lx_load(const char *fn)
{
  Lxfile *lxp = malloc(sizeof(Lxfile));
  lxp->fn = fn;
  lxp->lines = lx_loadfile((ccp)fn, &lxp->nlines, (char**)&lxp->fmem);
  lx_parse(lxp);
  return lxp;
}

void
lx_free(Lxfile *lxp)
{
  if (lxp)
    {
      if (lxp->items)
	free(lxp->items);
      if (lxp->lines)
	free(lxp->lines);
      if (lxp->fmem)
	free(lxp->fmem);
      free(lxp);
    }
}
