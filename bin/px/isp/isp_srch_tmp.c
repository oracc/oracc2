#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

int
isp_srch_tmp(Isp *ip)
{
  if (!strchr(ip->srchdata.tmp,'/'))
    {
      ip->srchdata.list = ip->srchdata.tmp;

      if (ip->glos)
	{
	  char buf[strlen(ip->cache.project)+strlen(ip->glos)+strlen(ip->srchdata.tmp)+3];
	  sprintf(buf, "%s/%s/%s", ip->cache.project, ip->glos, ip->srchdata.tmp);
	  ip->srchdata.tmp = (ccp)pool_copy((ucp)buf, ip->p);
	}
      else
	{
	  char buf[strlen(ip->cache.project)+strlen(ip->srchdata.tmp)+3];
	  sprintf(buf, "%s/%s", ip->cache.project, ip->srchdata.tmp);
	  ip->srchdata.tmp = (ccp)pool_copy((ucp)buf, ip->p);
	}
    }

  ip->tmp_dir = ip->cache.use = ip->cache.out = ip->srchdata.tmp;
  
  return 0;  
}
