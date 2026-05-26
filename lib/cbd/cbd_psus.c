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

static char *
sanitize_cgp(const char *cfg)
{
  char buf[strlen(cfg)], *bp = buf;
  const char *s = cfg, *x;
  *bp = '\0';
  while (*s)
    {
      while (' ' == *s)
	++s;
      if ('n' == s[0] && ' ' == s[1])
	s += 2;
      else if ((x = strstr(s, "//")))
	{
	  while (s < x)
	    *bp++ = *s++;
	  *bp = '\0';
	  s = strchr(s, ']');
	  while (*s && '\'' != *s)
	    *bp++ = *s++;
	  *bp = '\0';
	  while (*s && !isspace(*s))
	    ++s;
	}
      else
	{
	  while (*s)
	    *bp++ = *s++;
	  *bp = '\0';
	}
    }
  return strdup(buf);
}

/* validate @parts, linking each cgp to its Entry.
 *
 * Two things need better implementation:
 *
 * n in @parts, e.g., n ŋal[be]V/it--these are currently just ignored when checking
 *
 * //SENSE in @parts CGP; this is currently stripped out in checking but should use SENSE_GW
 *
 */
void
cbd_psus(void)
{
  static int psu_verbose = 1;
  Parts *p;
  for (p = list_first(csetp->parts); p; p = list_next(csetp->parts))
    {
      Cbd *pcbd = p->owner->owner;
      Cgp *cp;
      Hash *seen = hash_create(0);
      if (psu_verbose)
	fprintf(stderr, "cbd_psus: %s\n", p->owner->cgp->tight);
      for (cp = list_first(p->cgps); cp; cp = list_next(p->cgps))
	{
	  char *c = (char*)cp->tight, *m = NULL;
	  if (('n' == c[0] && ' ' == c[1])
	      || strstr(c, " n ")
	      || strstr(c, "//"))
	    c = m = sanitize_cgp(c);
	  Entry *ep = hash_find(pcbd->hentries, (uccp)c);
	  if (ep)
	    cp->owner = ep;
	  else
	    {
	      if ((ep = xcbd_find(ep,(uccp)c,pcbd)))
		cp->owner = ep;
	      else
		{
		  mesg_verr(&p->l, "part %s not found in any glossary", cp->tight);
		  p->owner = NULL; /* flag value to indicate that no
				      future processing of @parts
				      should be done */
		}
	    }
	  if (cp->owner && !hash_find(seen, (uccp)c))
	    {
	      psu_index(cp->owner, p->owner); /* index the current part
						 as occurring in the
						 current PSU */
	      /* if a part occurs more than once in a PSU only index
		 it once to avoid faffing with uniqing later */
	      hash_add(seen, (uccp)c, "");
	    }
	  if (m)
	    free(m);
	}
      hash_free(seen, NULL);  
    }
}
