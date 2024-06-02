#include <oraccsys.h>
#include "lx.h"

static Hash *h_lxf;

static void
ximportx(Lx *lp)
{
  const char *atfdata = NULL;
  Lxfile *x = hash_find(h_lxf, (ucp)lp->p);
  if (!x)
    {
      atfdata = lx_atf_data(lp->p, NULL);
      if (!(x = lx_load(atfdata)))
	{
	  fprintf(stderr, "%s: unable to open %s for ximport\n", prog, atfdata);
	  return;
	}
      hash_add(h_lxf, (ucp)lp->p, x);
    }
  else
    atfdata = x->fn;
  Lx *lpp = hash_find(x->seen, (ucp)lp->i);
  if (lpp)
    {
      if (lpp->x)
	{
	  if ('p' == *lpp->x) /* this was already a proxy in the proxy project */
	    lp->x = lpp->x;
	  else
	    {
	      char *p = malloc(strlen(lpp->x)+2);
	      strcpy(p, "p");
	      strcat(p, lpp->x);
	      lp->x = p;
	    }
	}
    }
  else
    fprintf(stderr, "%s: item %s not found in %s\n", prog, lp->i, atfdata);
}

void
lx_ximport(Lxfile *lxf)
{
  size_t i;
  h_lxf = hash_create(7);
  for (i = 0; i < lxf->nitems; ++i)
    {
      if (lxf->items[i].x && 'p' == *lxf->items[i].x)
	ximportx(&lxf->items[i]);
    }
#if 0
  hash_free(h_lxf, lx_free_lx);
#endif
}
