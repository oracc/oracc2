#include <oraccsys.h>
#include "lx.h"

void
lx_finish(Lxfile *lxf)
{
  if (sort)
    lx_sort(lxf);

  if (metaforce)
    lx_metaforce(lxf);
}
