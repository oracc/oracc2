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
bvl_name_dump(Name *np, int i)
{
  fprintf(stderr, "  np[%d]: orig=%s; last=%s; rest=%s; init=%s; nkey=%s\n",
	  i, np->orig, np->last, np->rest, np->init, np->nkey);
}

/* side effect: split and parse into individual names then store as
 * Name structs in the Bibentry
 */
void
bvl_name(Mloc *mp, Bx *bp, struct bib_fld_tab *bfp, Bibentry *ep)
{
  const char *s = ep->fields[bfp->t];

  /* entries with empty author or editor are bad form but may occur */
  if (!s)
    return;
  
  char *t;
  int nnames = 1;
  const char *s2 = s;
  while ((s2 = strstr(s2, " and ")))
    {
      ++nnames;
      s2 += 5;
    }
  Name **names = memo_new_array(bp->m_name_ptr, nnames+1);
  int i;
  char *dup = strdup(ep->fields[bfp->t]);
  if (verbose)
    fprintf(stderr, "bib %s starting field=%s\n", ep->bkey, dup);
  for (i = 0, t = dup; i < nnames; ++i)
    {
      names[i] = memo_new(bp->m_name);
      names[i]->orig = t;
      while (isspace(*t))
	++t;
      if ((t = strstr(t, " and ")))
	{
	  *t = '\0';
	  t += 5;
	  while (isspace(*t))
	    ++t;
	}
      bnm_split(mp, bp, ep, names[i]);
      bnm_nkey(mp, bp, names[i]);
      if (verbose)
	bvl_name_dump(names[i], i);
    }
  free(dup);

  if (bfp->t == f_author)
    {
      ep->nnames = nnames;
      ep->names = names;
      if (nnames == 1)
	ep->allnames = ep->names[0]->nkey;
      else
	bnm_all_names(ep, f_author);
    }
  else if (bfp->t == f_editor)
    {
      ep->nenames = nnames;
      ep->enames = names;
    }
  else
    {
      fprintf(stderr, "bvl_name: name validation applied to non-author/editor field %s\n", bfp->name);
    }
}

void
bvl_year(Mloc *mp, Bx *bp, struct bib_fld_tab *bfp, Bibentry *ep)
{
  const char *y = ep->fields[bfp->t];

  if (!y)
    {
      ep->year = 0;
      return;
    }

  struct bib_year_tab *yp = bib_year(y, strlen(y));
  if (yp)
    {
      ep->year = yp->year;
    }
  else
    {
      int ndig = 0;
      int hyph = 0;
      int nbad = 0;
  
      while (*y)
	{
	  if (isdigit(*y))
	    ++ndig;
	  else if ('-' == *y)
	    ++hyph;
	  else
	    ++nbad;
	  ++y;
	}
  
      /* simple case, year is four digits */
      if (ndig == 4 && !hyph && !nbad)
	ep->year = atoi(ep->fields[bfp->t]);
      else if (hyph && !nbad)
	{
	  int y1 = atoi(ep->fields[bfp->t]);
	  ep->year = y1;
	  const char *hyph = strchr(ep->fields[bfp->t], '-');
	  if (hyph && '-' == hyph[1] && isdigit(hyph[2]))
	    {
	      if (hyph[6])
		mesg_verr(mp, "%s: junk after range in year\n", ep->fields[bfp->t]);
	      else
		{
		  int y2 = atoi(&hyph[2]);
		  if (y2 <= y1)
		    mesg_verr(mp, "%s: year range end <= range start\n", ep->fields[bfp->t]);
		}
	    }
	  else
	    mesg_verr(mp, "%s: malformed range in year\n", ep->fields[bfp->t]);	
	}
      else
	{
	  mesg_verr(mp, "%s: bad character(s) in year\n", ep->fields[bfp->t]);
	}
    }
}
