#include <oraccsys.h>
#include "ns.h"

void
nsb_sys(uchar *t)
{
  nxp->sys = memo_new(nxp->m_sys);
  nxp->sys->name = t;
  ++nxp->nsys;
  printf("nsb_sys: sys %s is sys %d\n", nxp->sys->name, nxp->nsys);
}
