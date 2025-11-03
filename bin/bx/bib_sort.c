#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static int ent_cmp(const void *a, const void *b)
{
  Bibentry *e1 = (*(Bibentry**)a);
  Bibentry *e2 = (*(Bibentry**)b);
  int i;
  for (i = 0; e1->names[i] && e2->names[i]; ++i)
    {
      int c = strcmp(e1->names[i]->nkey, e2->names[i]->nkey);
      if (c)
	return c;
    }
  
  if (e1->names[i])
    return 1;
  else if (e2->names[i])
    return -1;
  
  return e1->year - e2->year;
}

void
bib_sort(Bx *bp)
{
  bp->ents = (struct bibentry **)list2array_c(bp->entries, &bp->nents);
  qsort(bp->ents, bp->nents, sizeof(Bibentry *), (cmp_fnc_t)ent_cmp);
}
