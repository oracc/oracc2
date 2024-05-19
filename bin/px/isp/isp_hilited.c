#include <oraccsys.h>
#include "isp.h"

char * const *
isp_hilited(Isp *ip)
{
  char *dbifn = "hilite";
  Dbi_index *dp;
  const char *h = ip->hili;
  const char *dir = ip->tmp_dir ? ip->tmp_dir : ip->cache.sub;
  if ((dp = dbx_init(dir, dbifn)))
    {
      h = (char*)dbx_key(dp, ip->itemdata.item, NULL);
      dbx_term(dp);
    }
  else
    {
      ip->err = (ccp)pool_copy((ucp)px_err("fatal: hilite db %s/%s could not be opened",
					   dir, dbifn), ip->p);
    }
  if (h)
    return (char *const *)vec_from_str((char*)h, NULL, NULL);
  else
    return NULL;
}
