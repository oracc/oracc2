#include <oraccsys.h>
#include "../px.h"
#include "isp.h"

int
isp_srch(Isp*ip)
{
  ip->from = "srch";
  ip->lloc.path = pool_alloc(strlen(ip->srchdata.tmp)+strlen("/list0"),ip->p);
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

  char *bar = (ccp)pool_alloc(strlen(ip->srchdata.tmp)+strlen("/search.bar0"), ip->p);
  sprintf((char*)bar, "%s/search.bar", ip->srchdata.tmp);
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
  
 wrapup:
  return ip->err ? 1 : 0;
}
