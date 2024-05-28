#include <oraccsys.h>
#include "lx.h"

void
lx_parse(Lxfile *lxp)
{
  int i, nitems;
  lxp->items = malloc(lxp->nlines * sizeof(Lx));
  for (i = nitems = 0; i < lxp->nlines; ++i)
    {
      if ('#' == *lxp->lines[i])
	; /* silently ignore comments */
      else if (*lxp->lines[i])
	lx_item(lxp, i);
      else
	mesg_warning(lxp->fn, i, "empty line not allowed in list file");
    }
}
