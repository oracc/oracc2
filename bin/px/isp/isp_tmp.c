#include <oraccsys.h>
#include "isp.h"

void
isp_tmp_dir(Isp *ip)
{
  if (!ip->tmp_dir)
    {
      char path[strlen(ip->oracc)+strlen(PX_TMP_PAT)+2];  
      sprintf(path, "%s/%s", ip->oracc, PX_TMP_PAT);
      ip->tmp_dir = (ccp)pool_copy((uccp)path, ip->p);
    }
}
