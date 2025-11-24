#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static FILE *bout;

void
bib_bib_entry(Bx *bp, Bibentry *ep)
{
  fprintf(bout, "@%s{%s\n", ep->type, xmlify((uccp)ep->bkey), dis, same);
  int i;
  for (i = 0; i < f_top; ++i)
    {
      if (ep->fields[i])
	fprintf(bout, " %s = { %s },\n", fldnames[i], ep->fields[i]->data);
    }
  fputs("}", bout);
}

void
bib_bib(Bx *bp, List *b, FILE *fp)
{
  bout = fp;
  Bib *bibp;
  for (bibp = list_first(b); bibp; bibp = list_next(b))
    {
      Bibentry *ep;
      for (ep = list_first(bibp->entries); ep; ep = list_next(bibp->entries))
	bib_bib_entry(bp, ep);
    }
}
