#include <oraccsys.h>
#include "../px.h"
#include "isp.h"

int
isp_srch(Isp*ip)
{
  ip->from = "srch";
  ip->lloc.path = pool_alloc(strlen(ip->srchdata.tmp)+strlen("/list0"),ip->p);
  sprintf((char*)ip->lloc.path, "%s/list", ip->srchdata.tmp);
  if (access(ip->lloc.path, R_OK))
    {
      ip->err = (ccp)pool_copy((ucp)px_err("srch list %s not found",ip->lloc.path),ip->p);
      return 1;
    }
  else
    {
      isp_list_cemd(ip);
      ip->cache.list = ip->lloc.path;
    }
  return 0;
}
