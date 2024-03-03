#include <oraccsys.h>
#include "isp.h"

/* This is a wrapper for the pg2/sortinfo subsystem that can also be
   called as a standalone program, ispsortx */
int
isp_cache_sort(Isp *ip)
{
  /* could reasonably have an Isp member 'curr_sort' which is the sort
     to use based on default/special and PERM settings */
  char buf[strlen(ip->cache.list)+strlen("-1230")];
  strcpy(buf, ip->cache.sort);
  strcpy(strrchr(buf,'/')+1, "sort-");
  strcat(buf, ip->perm);
  ip->cache.sort = (ccp)pool_copy((uccp)buf, ip->p);
  return ispsort(ip, NULL, NULL, ip->sort.default_sort);
}
