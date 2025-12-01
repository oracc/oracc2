#include <oraccsys.h>
#include <ctype.h>
#include "bx.h"
#include "bib.h"

/* Validation routines for incoming .bib fields; some of these are
 * designed to have side effects
 */

bibvalfnc bib_validators[f_top];

static void
bib_fld_set_val(const char *name, bibvalfnc v)
{
    struct bib_fld_tab *fp = bib_fld(name, strlen(name));
    bib_validators[fp->t] = v;
}

void
bvl_init(void)
{
  bib_fld_set_val("author", bvl_name);
  bib_fld_set_val("bookauthor", bvl_name);
  bib_fld_set_val("editor", bvl_name);
  bib_fld_set_val("translator", bvl_name);
  bib_fld_set_val("ids", bvl_ids);
  bib_fld_set_val("page", bvl_page);
  bib_fld_set_val("pages", bvl_page);
  bib_fld_set_val("year", bvl_year);
}

/* side effect: add alternate refs to entries hash
 */
void
bvl_ids(Mloc *mp, Bx *bp, enum bib_ftype ft, Bibentry *ep)
{
  
}
void
bvl_page(Mloc *mp, Bx *bp, enum bib_ftype ft, Bibentry *ep)
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
bvl_name(Mloc *mp, Bx *bp, enum bib_ftype ft, Bibentry *ep)
{
  /* ignore null author */
  if (-1 == ep->fields[ft]->line)
    return;
      
  Nameinfo *nip = memo_new(bp->m_nameinfo);
  nip->ftype = ft;
  switch (ft)
    {
    case f_author:
      ep->nm_author = nip;
      break;
    case f_editor:
      ep->nm_editor = nip;
      break;
    case f_bookauthor:
      ep->nm_bookauthor = nip;
      break;
    case f_translator:
      ep->nm_translator = nip;
      break;
    default:
      mesg_verr(mp, "name validation applied to non-name field %s\n", fldnames[ft]);
      break;
    }
  const char *s = ep->fields[ft]->data;
  mp->line = ep->fields[ft]->line;

  /* entries with empty author or editor are bad form but may occur; nothing more to set */
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
  
  nip->nnames = nnames;
  nip->names = memo_new_array(bp->m_name_ptr, nip->nnames+1);
  
  int i;
  char *dup = strdup(ep->fields[ft]->data);
  if (verbose)
    fprintf(stderr, "bib %s starting field=%s\n", ep->bkey, dup);
  for (i = 0, t = dup; i < nip->nnames; ++i)
    {
      nip->names[i] = memo_new(bp->m_name);
      nip->names[i]->orig = t;
      while (isspace(*t))
	++t;
      if ((t = strstr(t, " and ")))
	{
	  *t = '\0';
	  t += 5;
	  while (isspace(*t))
	    ++t;
	}
      bnm_split(mp, bp, ep, nip->names[i]);
      bnm_nkey(mp, bp, nip->names[i]);
      if (verbose)
	bvl_name_dump(nip->names[i], i);
    }
  free(dup);

  if (nip->nnames == 1)
    nip->allnames = nip->names[0]->nkey;
  else
    bnm_all_names(ep, nip);
}

void
bvl_year(Mloc *mp, Bx *bp, enum bib_ftype ft, Bibentry *ep)
{
  const char *y = ep->fields[ft]->data;

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
	ep->year = atoi(ep->fields[ft]->data);
      else if (hyph && !nbad)
	{
	  int y1 = atoi(ep->fields[ft]->data);
	  ep->year = y1;
	  char *s_hyph = strchr(ep->fields[ft]->data, '-');
	  if (s_hyph && (isdigit(s_hyph[1]) || ('-' == s_hyph[1] && isdigit(s_hyph[2]))))
	    {
	      const char *s_y2 = s_hyph;
	      while ('-' == *s_y2)
		++s_y2;
	      const char *end_y2 = s_y2;
	      while (isdigit(*end_y2))
		++end_y2;
	      if (*end_y2)
		mesg_verr(mp, "%s: junk after range in year at `%s'\n",
			  ep->fields[ft]->data, end_y2);
	      else
		{
		  if (end_y2 - s_y2 == 1)
		    {
		      int y2 = atoi(s_y2);
		      y1 = atoi(ep->fields[ft]->data+3);
		      if (y2 <= y1)
			mesg_verr(mp, "%s: year range end <= range start\n",
				  ep->fields[ft]->data);
		    }
		  else if (end_y2 - s_y2 == 2)
		    {
		      int y2 = atoi(s_y2);
		      y1 = atoi(ep->fields[ft]->data+2);
		      if (y2 <= y1)
			mesg_verr(mp, "%s: year range end <= range start\n",
				  ep->fields[ft]->data);
		    }
		  else if (end_y2 - s_y2 == 4)
		    {
		      int y2 = atoi(s_y2);
		      if (y2 <= y1)
			mesg_verr(mp, "%s: year range end <= range start\n",
				  ep->fields[ft]->data);
		    }
		  else
		    mesg_verr(mp, "%s: range end should be 1, 2 or 4 digits\n",
			      ep->fields[ft]->data);
		}
	    }
	  else
	    mesg_verr(mp, "%s: malformed range in year\n", ep->fields[ft]->data);	
	}
      else
	{
	  mesg_verr(mp, "%s: bad character(s) in year\n", ep->fields[ft]->data);
	}
    }
}
