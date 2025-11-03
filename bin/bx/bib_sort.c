#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static int ent_cmp(const void *a, const void *b)
{
  Bibentry *e1 = (*(Bibentry**)a);
  Bibentry *e2 = (*(Bibentry**)b);
  int c = strcmp(e1->key, e2->key);
  fprintf(stderr, "ent_cmp: %s vs %s => %d\n", e1->key, e2->key, c);
}

void
bib_sort(Bx *bp)
{
  bp->ents = list2array_c(bp->entries, &bp->nents);
  qsort(bp->ents, bp->nents, sizeof(Bibentry *), (cmp_fnc_t)ent_cmp);
	
}
