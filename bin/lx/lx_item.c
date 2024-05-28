#include <oraccsys.h>
#include "lx.h"

void
lx_item(Lxfile *lxp, int i)
{
  if (!lxp)
    return;
  
  if (i < lxp->nlines)
    {
      char *l = lxp->lines[i];
      char *colon = strchr(l, ':'), *at = strchr(l,'@');
      Lx *lp = &lxp->items[lxp->nitems++];
      if (colon)
	{
	  lp->p = l;
	  *colon++ = '\0';
	  lp->i = colon;
	}
      else
	lp->i = l;
      if (at)
	{
	  *at++ = '\0';
	  lp->c = at;
	}
    }
  else
    mesg_warning(lxp->fn, i, "bad lx_item request");
}
