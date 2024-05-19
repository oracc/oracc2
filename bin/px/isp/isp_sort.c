#include <oraccsys.h>
#include "isp.h"

/* This is a wrapper for the pg2/sortinfo subsystem that can also be
   called as a standalone program, ispsortx */
int
isp_cache_sort(Isp *ip)
{
  char buf[strlen(ip->cache.list)+strlen("-1230.mol")];

  /* In P4 'special' outline mode is only used when the list name is outlined.lst */
  if (ip->list_name && !strcmp(ip->list_name, "outlined.lst") && ip->special_cfg.select)
    ip->curr_cfg = &ip->special_cfg;
  else
    ip->curr_cfg = &ip->default_cfg;

  strcpy(buf, ip->cache.list);
  strcpy(strrchr(buf,'/')+1, "sort-");
  strcat(buf, ip->perm);
  ip->cache.sort = (ccp)pool_copy((uccp)buf, ip->p);
  strcat(buf, ".mol");
  ip->cache.mol = (ccp)pool_copy((uccp)buf, ip->p);
  if (ip->force)
    return ispsort(ip);
  else
    {
      if (!access(ip->cache.sort, F_OK))
	{
	  if (!access(ip->cache.sort, R_OK))
	    return 0;
	  else
	    {
	      ip->err = "sort file exists but is unreadable";
	      return 1;
	    }
	}
      else
	return ispsort(ip);
    }
}
