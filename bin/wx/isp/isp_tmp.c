#include <oraccsys.h>
#include "isp.h"

void
isp_tmp_dir(struct isp *ip)
{
  if (!ip->tmp_dir)
    {
      char path[strlen(ip->oracc)+strlen(ISP_TMP_PAT)+2];  
      sprintf(path, "%s/%s", ip->oracc, ISP_TMP_PAT);
      ip->tmp_dir = (ccp)pool_copy((uccp)path, ip->p);
    }
}
