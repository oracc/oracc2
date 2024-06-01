#include <oraccsys.h>
#include "lx.h"

void
lx_finish(Lxfile *lxf)
{
  if (sort)
    lx_sort(lxf);
#if 0
  if (cat_force)
    lx_catforce(lxf);
#endif
}
