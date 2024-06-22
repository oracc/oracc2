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

int
isp_cache_sort(Isp *ip)
{
#if 0
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
#endif

  if (isp_sort_sub(ip))
    return 1;

  return 0;  
}

static int
isp_sort_sub(Isp *ip)
{
  if (ip->tmp_dir || ip->glosdata.xis) /* This should also check for a www/list/LIST that has dotted IDs */
    {
      /* out should already have the xis or the search temp at the end */
      char dir[strlen(ip->cache.out) + strlen(ip->perm) + 3];
      sprintf(dir, "%s/%s", ip->cache.out, ip->perm);
      ip->cache.out = ip->cache.sort = (ccp)pool_copy((ucp)dir, ip->p);
      ip->cache.t_sort = (ccp)pool_alloc(strlen(ip->cache.sort)+3, ip->p);
      sprintf((char*)ip->cache.t_sort, "%s/t", ip->cache.sort);
      ip->cache.t_mol = (ccp)pool_alloc(strlen(ip->cache.t_sort)+10, ip->p);
      sprintf((char*)ip->cache.t_mol, "%s/zoom.mol", ip->cache.t_sort);
    }
  else
    {
      char dir[strlen(ip->cache.use) + strlen(ip->list_name) + strlen(ip->perm) + 3];
      sprintf(dir, "%s/%s/%s", ip->cache.use, ip->list_name, ip->perm);
      ip->cache.sort = (ccp)pool_copy((ucp)dir, ip->p);
      /* leave ip->cache.t_sort NULL */

      if (!ip->pub_output)
	{
	  char outd[strlen(ip->cache.project)+strlen(ip->list_name)+strlen(ip->perm)+3];
	  sprintf(outd, "%s/%s/%s", ip->cache.project, ip->list_name, ip->perm);
	  ip->cache.out = (ccp)pool_copy((ucp)outd, ip->p);
	}
    }
  
  struct stat sb;
  if (stat(ip->cache.sort, &sb) || !S_ISDIR(sb.st_mode))
    {
      if (ip->verbose)
	fprintf(stderr, "iss_data: creating %s\n", ip->cache.sort);
      if (mkdir(ip->cache.sort, 0775))
	{
	  ip->err = PX_ERROR_START "fatal: iss_data sort directory %s could not be created";
	  ip->errx = (ccp)pool_copy((ucp)ip->cache.sort, ip->p);
	}
    }

  if (ip->cache.out && strcmp(ip->cache.sort, ip->cache.out))
    {
      if (stat(ip->cache.out, &sb) || !S_ISDIR(sb.st_mode))
	{
	  if (ip->verbose)
	    fprintf(stderr, "iss_data: creating %s\n", ip->cache.out);
	  if (mkdir(ip->cache.out, 0775))
	    {
	      ip->err = PX_ERROR_START "fatal: iss_data sort directory %s could not be created";
	      ip->errx = (ccp)pool_copy((ucp)ip->cache.sort, ip->p);
	    }
	}
    }

  char tsvfn[strlen(ip->cache.sort)+strlen("/pag.tsv0")];
  sprintf(tsvfn, "%s/pag.tsv", ip->cache.sort);
  ip->cache.tsv = (ccp)pool_copy((ucp)tsvfn, ip->p);

  sprintf(tsvfn, "%s/max.tsv", ip->cache.sort);
  ip->cache.max = (ccp)pool_copy((ucp)tsvfn, ip->p);

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

  return 0;
}
