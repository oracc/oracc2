#include <oraccsys.h>
#include "lx.h"

void
lx_check(const char *fn)
{
  mesg_init();
  Lxfile *lxf = lx_load(fn);
  mesg_print(stderr);
  if (show)
    lx_show(lxf, stdout);
  lx_free(lxf);
  mesg_term();
}
