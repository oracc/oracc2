#include <oraccsys.h>
#include "isp.h"

void
isp_tmp_dir(Isp *ip)
{
  if (!ip->tmp_dir)
    {
      if ('s' == ip->list_name[0] && '.' == ip->list_name[1])
	{
	  char dir[strlen(ip->cache.sys)+strlen(ip->project)+strlen(ip->list_name)+3];
	  sprintf(dir, "%s/%s/%s", ip->cache.sys, ip->project, ip->list_name);
	  ip->tmp_dir = (ccp)pool_copy((uccp)dir, ip->p);
	  isp_srch_bar(ip);
	  isp_srch_count(ip);
	}
      else if ('i' == *ip->lloc.type)
	{
	  char dir[strlen(ip->oracc)+strlen(PX_TMP_PAT)];
	  sprintf(dir, "%s/%s", ip->oracc, PX_TMP_PAT);
	  char *slash = strrchr(dir, '/');
	  *slash = '\0';
	  ip->cache.sys = (ccp)pool_copy((uccp)dir, ip->p);
	  char path[strlen(ip->oracc)+strlen(PX_TMP_PAT)+2];  
	  sprintf(path, "%s/%s", ip->oracc, PX_TMP_PAT);
	  ip->tmp_dir = (ccp)pool_copy((uccp)path, ip->p);
	}
    }
}
