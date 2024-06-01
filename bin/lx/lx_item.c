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

      /* trim leading spaces */
      while (isspace(*l))
	++l;

      /* trim trailing spaces */
      char *end = l+strlen(l);      
      while (isspace(end[-1]))
	*--end = '\0';

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
	    lpp->c = lpp->p;
	  else
	  lpp->c = lc;
	}
      else if (check)
	{
	  mesg_vwarning(lxp->fn, i, "dropping duplicate item %s", li);
	}
    }
  else
    mesg_warning(lxp->fn, i, "bad lx_item request");
}
