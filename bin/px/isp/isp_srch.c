#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

/* Fix some things up to compensate for not running page builder when
   srch result count == 0 */
int
isp_srch_fixup(Isp *ip)
{
  ip->data = "dtxt";
  return 0;
}

int
isp_srch_bar(Isp *ip)
{
  const char *tmp = ip->srchdata.tmp ? ip->srchdata.tmp : ip->tmp_dir;
  if (tmp)
    {
      char *bar = (char *)pool_alloc(strlen(tmp)+strlen("/search.bar0"), ip->p);
      sprintf((char*)bar, "%s/search.bar", tmp);
      if (access(bar, R_OK))
	{
	  ip->err = (ccp)pool_copy((ucp)px_err("unknown or inaccessible search.bar %s", bar), ip->p);
	  goto wrapup;
	}
      
      char *bartext = (char*)slurp("qx-px", bar, NULL);
      if (!bartext)
	{
	  ip->err = (ccp)pool_copy((ucp)px_err("failed to read search.bar %s", bar), ip->p);
	  goto wrapup;
	}
      else if ('\n' == bartext[strlen(bartext)-1])
	bartext[strlen(bartext)-1] = '\0';
      
      ip->srch = bartext;
    }
  else
    {
      ip->err = "no temp dir to look for search.bar in";
    }
      
 wrapup:
  return ip->err ? 1 : 0;
}

int
isp_srch_count(Isp *ip)
{
  const char *tmp = ip->srchdata.tmp ? ip->srchdata.tmp : ip->tmp_dir;
  if (tmp)
    {
      char *bar = (char *)pool_alloc(strlen(tmp)+strlen("/count0"), ip->p);
      sprintf((char*)bar, "%s/count", tmp);
      if (access(bar, R_OK))
	{
	  ip->err = (ccp)pool_copy((ucp)px_err("unknown or inaccessible count %s", bar), ip->p);
	  goto wrapup;
	}
      
      char *bartext = (char*)slurp("qx-px", bar, NULL);
      if (!bartext)
	{
	  ip->err = (ccp)pool_copy((ucp)px_err("failed to read count %s", bar), ip->p);
	  goto wrapup;
	}
      else
	{
	  ip->srchdata.count = strtoul(bartext, NULL, 10);
	}
    }
  else
    {
      ip->err = "no temp dir to look for count in";
    }
      
 wrapup:
  return ip->err ? 1 : 0;
}

int
isp_srch(Isp*ip)
{
  ip->from = "srch";

  if (!strchr(ip->srchdata.tmp,'/'))
    {
      if (ip->glos)
	{
	  char buf[strlen(ip->cache.project)+strlen(ip->glos)+strlen(ip->srchdata.tmp)+3];
	  sprintf(buf, "%s/%s/%s", ip->cache.project, ip->glos, ip->srchdata.tmp);
	  ip->srchdata.tmp = (ccp)pool_copy(buf, ip->p);
	}
      else
	{
	  char buf[strlen(ip->cache.project)+strlen(ip->srchdata.tmp)+3];
	  sprintf(buf, "%s/%s", ip->cache.project, ip->srchdata.tmp);
	  ip->srchdata.tmp = (ccp)pool_copy(buf, ip->p);
	}
    }
  ip->cache.use = ip->cache.out = ip->srchdata.tmp;

  if (isp_srch_count(ip))
    return 1;
  
  if (isp_srch_bar(ip))
    return 1;

  if (ip->srchdata.count)
    {
      ip->lloc.path = (ccp)pool_alloc(strlen(ip->srchdata.tmp)+strlen("/list0"),ip->p);
      sprintf((char*)ip->lloc.path, "%s/list", ip->srchdata.tmp);
      if (access(ip->lloc.path, R_OK))
	{
	  ip->err = (ccp)pool_copy((ucp)px_err("srch list %s not found",ip->lloc.path),ip->p);
	  return 1;
	}
      else
	{
	  isp_list_cemd(ip);
	  ip->cache.list = ip->lloc.path;
	}
    }
  
  return ip->err ? 1 : 0;
}
