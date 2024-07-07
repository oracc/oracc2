#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

int
set_item_max(Isp *ip)
{
  const char *sdir = ((ip->item && ip->cache.t_sort) ? ip->cache.t_sort : ip->cache.sort);
  /* Watch me: can cache.txtindex ever be set when we are actually in a search/glos, etc? */
  const char *maxdir = (ip->cache.txtindex ? ip->cache.txtindex : sdir);

  Dbi_index *dp = dbx_init(maxdir, "max");
  if (dp)
    {
      const char *k = dbx_key(dp, ip->zoom, NULL);
      if (k)
	{
	  ip->zmax = atoi(k);
	  dbx_term(dp);
	  return 0;
	}
      else
	{
	  ip->err = "fatal: item %s not in max db";
	  ip->errx = ip->zoom;
	  dbx_term(dp);
	  return 1;
	}
    }
  else
    {
      ip->err = "fatal: no max db found in %s";
      ip->errx = maxdir;
      return 1;
    }
}

int
create_page_div(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/ispdiv.sh");
  list_add(args, " ");
  list_add(args, (void*)ip->project);
  list_add(args, " ");
  if (ip->cache.tsv)
    list_add(args, (void*)ip->cache.tsv);
  else
    list_add(args, (void*)ip->cache.pgin);
  list_add(args, " ");
  if (ip->cache.pkey)
    list_add(args, (void*)ip->cache.pkey);
  else
    list_add(args, "-");
  list_add(args, " ");
  list_add(args, (void*)ip->cache.page);
  list_add(args, " ");
  list_add(args, (void*)ip->cemd);

  if (ip->dors && '1' == *ip->dors)
    list_add(args, " special");
  else
    list_add(args, " default");
  
  if (ip->glos)
    {
      list_add(args, " ");
      list_add(args, (void*)ip->glos);
    }

  unsigned char *syscmd = list_concat(args);

  if (ip->verbose)
    fprintf(stderr, "isp: create_page_div: %s\n", syscmd);

  if (system((ccp)syscmd))
    {
      ip->err = PX_ERROR_START "fatal: create_page_div failed system call:\n\n\t%s\n";
      ip->errx = (ccp)syscmd;
      return 1;
    }
  
  return 0;
}

int
isp_page_data(Isp *ip)
{
  /* In item display the item knows its own page/zoom/index but we
     don't precompute the page div because we haven't done
     compute_ranges */
  if (ip->item) /* && !strcmp(ip->what, "text"))*/
    return 0;

  set_item_max(ip);

  int len = snprintf(NULL, 0, "z%sp%s", ip->zoom, ip->page);
  ip->cache.pkey = (ccp)pool_alloc(len+1, ip->p);
  sprintf((char*)ip->cache.pkey,  "z%sp%s", ip->zoom, ip->page);
  return create_page_div(ip);
}
