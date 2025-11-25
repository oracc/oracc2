#include <oraccsys.h>
#include <xml.h>
#include "bx.h"
#include "bib.h"

static FILE *bout;

static int
bib_field_cmp(Bibfield **a, Bibfield **b)
{
  /* bib fields may be NULL */
  if (!*a && !*b)
    return 0;
  else if (!*a)
    return 1;
  else if (!*b)
    return -1;

  return strcmp((*a)->name, (*b)->name);
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
      ep->fields[i]->line = bftp->sort;
    }
}

void
bib_bib_entry(Bx *bp, Bibentry *ep)
{
  fprintf(bout, "@%s{%s,\n", ep->type, xmlify((uccp)ep->bkey));
  int i;

  if (bp->sortfields)
    bib_reset_field_line(ep);
  
  qsort(ep->fields, f_top, sizeof(Bibfield*), (sort_cmp_func*)bib_field_cmp);

  for (i = 0; ep->fields[i]; ++i)
    fprintf(bout, " %s = { %s },\n", ep->fields[i]->name, ep->fields[i]->data);

  fputs("}\n\n", bout);
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
