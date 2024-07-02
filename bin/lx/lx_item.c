#include <oraccsys.h>
#include "lx.h"

/**Load one file into an items array, dropping duplicate items
 */
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

      /* work on only the first column; additional columns are echoed
	 through */
      char *tab = strchr(lxp->lines[i], '\t');
      if (tab)
	*tab++ = '\0';

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
	  Lx *lpp = &lxp->items[lxp->nitems++];
	  lpp->proxy = proxy;
	  hash_add(lxp->seen, (uccp)li, lpp);
#if 1
	  if (!lp)
#else
          if (qualify && !lp)
#endif
	    lpp->p = (char*)project;
	  else
	    lpp->p = lp;
	  lpp->i = li;
	  if (!lc)
#if 1
	    if (metaforce)
	      lpp->c = (char*)metaforce;
	    else
	      lpp->c = lpp->p;
#else
	    {
	      if (proxy)
		lpp->c = lpp->p;
	      else if (qualify)
		lpp->c = (char*)project;
	    }
#endif
	  else
	    lpp->c = lc;
	  if (proxy)
	    {
	      if (tab)
	        {
		  if ('p' != *tab)
		    {
		      char *p = malloc(strlen(tab)+2);
		      strcpy(p, "p");
		      strcat(p, tab);
		      lpp->x = p;
		    }
		  else
		    lpp->x = tab;
		}
	      else
		lpp->x = "p";
	    }
	  else
	    lpp->x = tab;
	}
      else if (check)
	{
	  mesg_vwarning(lxp->fn, i, "dropping duplicate item %s", li);
	}
    }
  else
    mesg_warning(lxp->fn, i, "bad lx_item request");
}
