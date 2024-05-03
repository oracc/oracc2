#include <oraccsys.h>
#include "isp.h"

/* This is a wrapper for the pg2/sortinfo subsystem that can also be
   called as a standalone program, ispsortx */
int
isp_cache_sort(Isp *ip)
{
  /* could reasonably have an Isp member 'curr_sort' which is the sort
     to use based on default/special and PERM settings */

  char buf[strlen(ip->cache.list)+strlen("-1230.mol")];
  strcpy(buf, ip->cache.list);
  strcpy(strrchr(buf,'/')+1, "sort-");
  strcat(buf, ip->perm);
  ip->cache.sort = (ccp)pool_copy((uccp)buf, ip->p);
  strcat(buf, ".mol");
  ip->cache.mol = (ccp)pool_copy((uccp)buf, ip->p);
  if (ip->force)
    return ispsort(ip, NULL, NULL, ip->sort.default_sort);
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
	return ispsort(ip, NULL, NULL, ip->sort.default_sort);
    }
}
