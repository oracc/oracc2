#include <oraccsys.h>
#include "lx.h"

static int lx_cmp(const void *a, const void *b)
{
  return strcmp(((Lx *)a)->i, ((Lx *)b)->i);
}

void
lx_sort(Lxfile *lxp)
{
  qsort(lxp->items, lxp->nitems, sizeof(Lx), lx_cmp);
}
