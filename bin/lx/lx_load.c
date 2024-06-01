#include <oraccsys.h>
#include "lx.h"

Lxfile *
lx_load(const char *fn)
{
  Lxfile *lxp = calloc(1, sizeof(Lxfile));
  lxp->fn = fn;
  lxp->lines = lx_loadfile((ccp)fn, &lxp->nlines, (char**)&lxp->fmem);
  if (lxp->nlines)
    {
      lxp->seen = hash_create(1024);
      lx_parse(lxp);
    }
  else
    {
      free(lxp->fmem);
      free(lxp->lines);
      free(lxp);
      lxp = NULL;
    }
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
      if (lxp->seen)
	hash_free(lxp->seen, NULL);
      free(lxp);
    }
}
