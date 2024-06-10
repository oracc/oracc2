#include <oraccsys.h>
#include "isp.h"
#include "../pxdefs.h"

static int isp_sort_sub(Isp *ip);

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

  if (isp_sort_sub(ip))
    return 1;

  return 0;  
}

static int
isp_sort_sub(Isp *ip)
{
  char dir[strlen(ip->cache.use) + strlen(ip->list_name) + strlen(ip->perm) + 3];
  sprintf(dir, "%s/%s", ip->cache.use, ip->perm);
  ip->cache.sort = (ccp)pool_copy((ucp)dir, ip->p);
  if (!ip->pub_output)
    ip->cache.out = ip->cache.sort;
  struct stat sb;
  if (stat(dir, &sb) || !S_ISDIR(sb.st_mode))
    {
      if (ip->verbose)
	fprintf(stderr, "iss_data: creating %s\n", dir);
      if (mkdir(dir, 0775))
	{
	  ip->err = PX_ERROR_START "fatal: iss_data sort directory %s could not be created";
	  ip->errx = (ccp)pool_copy((ucp)dir, ip->p);
	}
    }

  char tsvfn[strlen(ip->cache.sort)+strlen("/pag.tsv0")];
  sprintf(tsvfn, "%s/pag.tsv", ip->cache.sort);
  ip->cache.tsv = (ccp)pool_copy((ucp)tsvfn, ip->p);

  char mold[strlen(ip->cache.use)+strlen(ip->list_name)+strlen("//zoom.mol0")];
  sprintf(mold, "%s/zoom.mol", ip->cache.sort);
  ip->cache.mol = (ccp)pool_copy((uccp)mold, ip->p);
  
  if (ip->force)
    return iss_sort(ip);
  else
    {
      if (!access(ip->cache.tsv, F_OK))
	{
	  if (!access(ip->cache.tsv, R_OK))
	    return 0;
	  else
	    {
	      ip->err = "sort tsv file exists but is unreadable";
	      return 1;
	    }
	}
      else if (iss_sort(ip))
	{
	  ip->err = "sort failed";
	  return 1;
	}
    }

  if (iso_master(ip))
    return 1;

  return 0;
}
