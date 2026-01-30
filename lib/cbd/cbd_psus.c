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

/* validate @parts, linking each cgp to its Entry.
 */
void
cbd_psus(void)
{
  Parts *p;
  for (p = list_first(csetp->lcbds); p; p = list_next(csetp->lcbds))
    {
      Cbd *pcbd = p->owner->owner;
      Cgp *cp;
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
		mesg_verr(&p->l, "part %s not found in any glossary", cp->tight);
	    }
	}
    }
}
