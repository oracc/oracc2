#include <oraccsys.h>
#include "isp.h"

int
isp_cache_sys(Isp *ip)
{
  if (!ip->cache.sys)
    {
      if ('i' == *ip->lloc.type)
	{
	  char dir[strlen(ip->oracc)+strlen("/tmp/isp/0")];
	  sprintf(dir, "%s/tmp/isp", ip->oracc);
	  ip->cache.sys = (ccp)pool_copy((uccp)dir, ip->p);
	}
      else
	{
	  ip->cache.sys = getenv("ORACC_ISP_CACHE");
	  if (!ip->cache.sys)
	    {
	      char dir[strlen(ip->oracc)+strlen("/www/is.d0")];
	      sprintf(dir, "%s/www/is.d", ip->oracc);
	      ip->cache.sys = (ccp)pool_copy((uccp)dir, ip->p);
	    }
	}
      struct stat sb;
      if (stat(ip->cache.sys, &sb) || !S_ISDIR(sb.st_mode))
	ip->err = "cache directory not found";
      else if (access(ip->cache.sys, W_OK))
	ip->err = "cache directory not writeable";
    }
  return ip->err ? 1 : 0;
}

/* if there is no directory in the cache for this list create one */
int
isp_cache_sub(Isp *ip)
{
  char dir[strlen(ip->cache.sys)+strlen(ip->list_name)+2];
  sprintf(dir, "%s/%s", ip->cache.sys, ip->list_name);
  ip->cache.sub = (ccp)pool_copy((uccp)dir, ip->p);
  struct stat sb;
  if (stat(ip->cache.sub, &sb) || !S_ISDIR(sb.st_mode))
    {
      if (mkdir(ip->cache.sub, 0775))
	ip->err = "cache directory not found and could not be created";
  }
  else if (access(ip->cache.sub, W_OK))
    ip->err = "cache directory not writeable";
  return ip->err ? 1 : 0;
}

/* if there is no list file in the cache.sub and the list type is not
   isp, try to create the list based on the specs given in ip->lloc */
int
isp_cache_list(Isp *ip)
{
  char list[strlen(ip->cache.sub)+strlen("/list0")];
  sprintf(list, "%s/list", ip->cache.sub);
  ip->cache.list = (ccp)pool_copy((uccp)list, ip->p);
  if (access(list, R_OK))
    {
      if (!access(list, F_OK))
	ip->err = "cache list file exists but is not readable";
      else if ('i' == *ip->lloc.type)
	ip->err = "cache list file does not exist in tmp/isp";
      else
	isp_list_create(ip);
    }
  return ip->err ? 1 : 0;
}
