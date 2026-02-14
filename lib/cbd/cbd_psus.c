#include <oraccsys.h>
#include "cbd.h"

static Entry *
xcbd_find(Entry *ep, unsigned const char *cgp, Cbd *x)
{
  Cbd *c;
  for (c = list_first(csetp->lcbds); c; c = list_next(csetp->lcbds))
    if (x != c)
      continue;
    else if ((ep = hash_find(c->hentries, cgp)))
      return ep;
  return NULL;
}

static void
psu_index(Entry *part, Entry *psu)
{
  List *lp = hash_find(csetp->hpsus, part->cgp->tight);
  if (!lp)
    hash_add(csetp->hpsus, part->cgp->tight, (lp = list_create(LIST_SINGLE)));
  list_add(lp, psu);
}

/* validate @parts, linking each cgp to its Entry.
 */
void
cbd_psus(void)
{
  Parts *p;
  for (p = list_first(csetp->parts); p; p = list_next(csetp->parts))
    {
      Cbd *pcbd = p->owner->owner;
      Cgp *cp;
      Hash *seen = hash_create(0);
      for (cp = list_first(p->cgps); cp; cp = list_next(p->cgps))
	{
	  Entry *ep = hash_find(pcbd->hentries, cp->tight);
	  if (ep)
	    cp->owner = ep;
	  else
	    {
	      if ((ep = xcbd_find(ep,cp->tight,pcbd)))
		cp->owner = ep;
	      else
		{
		  mesg_verr(&p->l, "part %s not found in any glossary", cp->tight);
		  p->owner = NULL; /* flag value to indicate that no
				      future processing of @parts
				      should be done */
		}
	    }
	  if (cp->owner && !hash_find(seen, cp->owner->cgp->tight))
	    {
	      psu_index(cp->owner, p->owner); /* index the current part
						 as occurring in the
						 current PSU */
	      /* if a part occurs more than once in a PSU only index
		 it once to avoid faffing with uniqing later */
	      hash_add(seen, cp->owner->cgp->tight, "");
	    }
	}
      hash_free(seen, NULL);  
    }
}
