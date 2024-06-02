#include <oraccsys.h>
#include "lx.h"

void
lx_metaforce(Lxfile *lxf)
{
  size_t i;
  for (i = 0; i < lxf->nitems; ++i)
    lxf->items[i].c = metaforce;
}
