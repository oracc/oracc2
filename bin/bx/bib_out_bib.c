#include <oraccsys.h>
#include <xml.h>
#include "bx.h"
#include "bib.h"

static FILE *bout;

static int
bib_field_line_cmp(Bibfield **a, Bibfield **b)
{
  /* bib fields may be NULL */
  if (!*a && !*b)
    return 0;
  else if (!*a)
    return 1;
  else if (!*b)
    return -1;

  return (*a)->line == (*b)->line ? 0 : ((*a)->line < (*b)->line ? -1 : 1);
}

/* To sort the fields within an entry alphabetically by name we
 * rewrite the line number member of each present field. This means we
 * shouldn't use line in error messages after this.
 */
static void
bib_reset_field_line(Bibentry *ep)
{
  int i;
  for (i = 0; ep->fields[i]; ++i)
    {
      struct bib_fld_tab *bftp = bib_fld(ep->fields[i]->name, strlen(ep->fields[i]->name));
      ep->fields[i]->line = bftp->sort+1; /* ensure that ids can have sort == 0 */
    }
}

void
bib_bib_entry(Bx *bp, Bibentry *ep)
{
  if (ep->comments)
    {
      const char *c;
      for (c = list_first(ep->comments); c; c = list_next(ep->comments))
	fprintf(bout, "%s", c);
    }
  fprintf(bout, "@%s{%s,\n", ep->type, xmlify((uccp)ep->bkey));
  int i;

  if (bp->sortfields)
    bib_reset_field_line(ep);
  
  qsort(ep->fields, f_top, sizeof(Bibfield*), (sort_cmp_func*)bib_field_line_cmp);

  for (i = 0; ep->fields[i]; ++i)
    if (ep->fields[i]->line >= 0 && ep->fields[i]->data)
      fprintf(bout, " %s = { %s },\n", ep->fields[i]->name, ep->fields[i]->data);

  fputs("}", bout);
}

void
bib_bib(Bx *bp, List *b, FILE *fp)
{
  bout = fp;
  if (bp->entries)
    {
      int i;
      if (bp->sort)
	{
	  for (i = 0; bp->ents[i]; ++i)
	    bib_bib_entry(bp, bp->ents[i]);
	}
      else
	{
	  Bibentry *ep;
	  for (ep = list_first(bp->entries); ep; ep = list_next(bp->entries))
	    bib_bib_entry(bp, ep);
	}
      Bib *bibp = list_first(bp->bibs);
      if (bibp && bibp->eof_comments)
	{
	  const char *c;
	  for (c = list_first(bibp->eof_comments); c; c = list_next(bibp->eof_comments))
	    fprintf(bout, "%s", c);
	}
      else if (verbose > 1)
	fprintf(stderr, "bib_bib: no eof_comments found; bibp == %lu\n", (uintptr_t)bibp);
    }
  else
    {
      Bib *bibp;
      for (bibp = list_first(b); bibp; bibp = list_next(b))
	{
	  Bibentry *ep;
	  for (ep = list_first(bibp->entries); ep; ep = list_next(bibp->entries))
	    bib_bib_entry(bp, ep);
	}
    }
}
