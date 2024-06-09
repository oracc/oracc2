#include <oraccsys.h>
#include "isp.h"


const char *
isp_set_perm(const char *str)
{
  int n = 1;
  const char *comma = strchr(str, ',');
  if (comma)
    {
      ++n;
      if ((comma = strchr(++comma, ',')))
	++n;
    }
  if (n == 1)
    str = "1";
  else if (n == 2)
    str = "12";
  else
    str = "123";
  return str;
}

/* This is a wrapper for the pg2/sortinfo subsystem that can also be
   called as a standalone program, ispsortx */
int
isp_cache_sort(Isp *ip)
{
  char buf[strlen(ip->cache.list)+strlen("-1230.mol")];

  /* In P4 'special' outline mode is only used when the list name is outlined.lst */
  if (ip->list_name && !strcmp(ip->list_name, "outlined.lst") && ip->special_cfg.select)
    {
      ip->curr_cfg = &ip->special_cfg;
      ip->dors = "1";
    }
  else
    {
      ip->curr_cfg = &ip->default_cfg;
      ip->dors = "0";
    }

  if (!ip->perm || '#' == *ip->perm)
    ip->perm = isp_set_perm(ip->default_cfg.sort_labels);

  strcpy(buf, ip->cache.list);
  strcpy(strrchr(buf,'/')+1, "sort-");  
  strcat(buf, ip->perm);
  ip->cache.sort = (ccp)pool_copy((uccp)buf, ip->p);
  strcat(buf, ".mol");
  ip->cache.mol = (ccp)pool_copy((uccp)buf, ip->p);
  if (ip->force)
    return iss_sort(ip);
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
	return iss_sort(ip);
    }
}
