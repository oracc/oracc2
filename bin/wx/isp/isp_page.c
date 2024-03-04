#include <oraccsys.h>
#include "isp.h"

static unsigned long
get_ft_start(const char *s)
{
  const char *ft = strchr(s, '/');
  ft = strchr(ft+1, '/');
  ft = strchr(ft+1, '/');
  return strtoul(ft+1, NULL, 10);
}

static unsigned long
get_ft_end(const char *s)
{
  const char *ft = strchr(s, '/');
  ft = strchr(ft+1, '/');
  ft = strchr(ft+1, '/');
  ft = strchr(ft+1, '/');
  return strtoul(ft+1, NULL, 10);
}

int
isp_cache_page(Isp *ip)
{
  char buf[strlen(ip->cache.sub)+strlen(ip->zoom)+strlen(ip->page)+strlen("-123-z-p.div0")];
  sprintf(buf, "%s/%s-z%s-p%s.div", ip->cache.sub, ip->perm, ip->zoom, ip->page);
  ip->cache.page = (ccp)pool_copy((uccp)buf, ip->p);
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
  char *mline = nth_line(mbuf, pg, multiplier>1);

  if (multiplier > 1)
    {
      char *mmline = nth_line(NULL, pg+multiplier, 0);
      if (mmline)
	{
	  unsigned long ft_start = get_ft_start(mline);
	  unsigned long ft_end = get_ft_end(mmline);
	  
	}
      else
	{
	  ip->err = "page out of range";
	  return 1;
	}
    }
  else
    {
      unsigned long ft_start = get_ft_start(mline);
      unsigned long ft_end = get_ft_end(mline);
      
    }
  return 0;
}

