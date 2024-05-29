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
      char *lp = NULL, *li = NULL, *lc = NULL;
      
      if (colon)
	{
	  lp = l;
	  *colon++ = '\0';
	  li = colon;
	}
      else
	li = l;
      if (at)
	{
	  *at++ = '\0';
	  lc = at;
	}
      if (!uniq || !hash_find(lxp->seen, (uccp)li))
	{
	  hash_add(lxp->seen, (uccp)li, "");
	  Lx *lpp = &lxp->items[lxp->nitems++];
	  if (qualify && !lp)
	    lpp->p = (char*)project;
	  else
	    lpp->p = lp;
	  lpp->i = li;
	  if (qualify && !lc)
	    lpp->c = (char*)project;
	  else
	  lpp->c = lc;
	}
      else if (verbose)
	{
	  mesg_vwarning(lxp->fn, i, "skipping duplicate item %s", li);
	}
    }
  else
    mesg_warning(lxp->fn, i, "bad lx_item request");
}
