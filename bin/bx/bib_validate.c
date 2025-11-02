#include <oraccsys.h>
#include <ctype.h>
#include "bx.h"
#include "bib.h"

/* Validation routines for incoming .bib fields; some of these are
 * designed to have side effects
 */

/* side effect: add alternate refs to entries hash
 */
void
bvl_ids(Mloc *mp, Bx *bp, struct bib_fld_tab *bfp, Bibentry *ep)
{
  
}
void
bvl_page(Mloc *mp, Bx *bp, struct bib_fld_tab *bfp, Bibentry *ep)
{
  
}

static void
bvl_name_dump(Bibentry *ep, int i)
{
  Name *np = ep->names[i];
  fprintf(stderr, "\tnp[%d]: orig=%s; last=%s; rest=%s; init=%s\n",
	  i, np->orig, np->last, np->rest, np->init);
}

/* side effect: split and parse into individual names then store as
 * Name structs in the Bibentry
 */
void
bvl_name(Mloc *mp, Bx *bp, struct bib_fld_tab *bfp, Bibentry *ep)
{
  const char *s = ep->fields[bfp->t];
  char *t;
  int nnames = 1;
  const char *s2 = s;
  while ((s2 = strstr(s2, " and ")))
    {
      ++nnames;
      s2 += 5;
    }
  ep->names = memo_new_array(bp->m_name_ptr, nnames+1);
  int i;
  char *dup = strdup(ep->fields[bfp->t]);
  if (verbose)
    fprintf(stderr, "starting field=%s\n", dup);
  for (i = 0, t = dup; i < nnames; ++i)
    {
      ep->names[i] = memo_new(bp->m_name);
      ep->names[i]->orig = t;
      t = dup;
      while (isspace(*t))
	++t;
      if ((t = strstr(t, " and ")))
	{
	  *t = '\0';
	  t += 5;
	  while (isspace(*t))
	    ++t;	  
	}
      bnm_split(mp, bp, ep, ep->names[i]);
      if (verbose)
	bvl_name_dump(ep, i);
    }
  free(dup);
}

void
bvl_year(Mloc *mp, Bx *bp, struct bib_fld_tab *bfp, Bibentry *ep)
{
}
