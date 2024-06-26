#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

int
isp_adhoc(Isp*ip)
{
  ip->from = "srch";

  ip->lloc.path = (ccp)pool_alloc(strlen(ip->srchdata.tmp)+strlen("/list0"),ip->p);
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
      char qxnew[strlen(ip->srchdata.tmp)+7];
      sprintf(qxnew, "%s/ad.new", ip->srchdata.tmp);
      if (!access(qxnew, R_OK))
	{
	  ip->srchdata.new = 1;
	  unlink(qxnew);
	}
    }
  
  return ip->err ? 1 : 0;
}
