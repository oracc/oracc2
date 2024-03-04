#include <oraccsys.h>
#include "isp.h"

int
isp_cache_page(Isp *ip)
{
  char buf[strlen(ip->cache.sub)+strlen(ip->zoom)+strlen(ip->page)+strlen("-123-z-p.div0")];
  sprintf(buf, "%s/%s-z%s-p%s.div", ip->cache.sub, ip->perm, ip->zoom, ip->page);
  ip->cache.page = pool_copy(buf, ip->p);
  if (ip->verbose)
    fprintf(stderr, "isp: isp_cache_page: need %s\n", buf);
  if (!access(ip->cache.page, F_OK))
    {
      if (!access(ip->cache.page, R_OK))
	return 0;
      else
	{
	  ip->err = "cache page exists but is unreadable";
	  return 1;
	}
    }
  char mbuf[strlen(ip->cache.sort)+strlen(ip->zoom)+strlen(".pmp0")];
  if (atoi(ip->zoom))
    sprintf(mbuf, "%s-z%s.pmp", ip->cache.sort, ip->zoom);
  else
    sprintf(mbuf, "%s.pmp", ip->cache.sort);
  /* compute the start line in the .pmp */
  int multiplier = 1;
  if (!strcmp(ip->psiz, "50"))
    multiplier = 2;
  else if (!strcmp(ip->psiz,"100"))
    multiplier = 4;

  int pg = atoi(ip->page) * multiplier;  
  char *mline = nth_line(mbuf, pg);
  char *pgx = NULL;

  if (multiplier > 1)
    {
      char *mmline = nth_line(buf, pg+multiplier);
      if (mmline)
	{
	  long ft_start = get_ft_start(mline);
	  long ft_end = get_ft_end(mmline);
	  
	}
      else
	{
	  ip->err = "page out of range";
	  return 1;
	}
    }
  else
    {
      long ft_start = get_ft_start(mline);
      long ft_end = get_ft_end(mline);
      
    }
}

