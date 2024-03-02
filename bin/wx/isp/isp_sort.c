#include <oraccsys.h>
#include "isp.h"

/* This is a wrapper for the pg2/sortinfo subsystem that can also be
   called as a standalone program, ispsortx */
int
isp_cache_sort(Isp *ip)
{
  /* could reasonably have an Isp member 'curr_sort' which is the sort
     to use based on default/special and PERM settings */
  ip->cache.sort = (ccp)pool_copy((uccp)ip->cache.list, ip->p);
  strcpy(strrchr(ip->cache.sort,'/')+1, "sort");
  return ispsort(ip, NULL, NULL, ip->sort.default_sort);
}
