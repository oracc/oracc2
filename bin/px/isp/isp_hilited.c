#include <oraccsys.h>
#include "isp.h"

char * const *
isp_hilited(Isp *ip)
{
  char *dbifn = "hilite";
  Dbi_index *dp;
  const char *h = ip->hili;
  /* no need to do ip->tmp_dir ? ip->tmp_dir : ip->cache.sub here ? */
  if ((dp = dbx_init(ip->cache.sub, dbifn)))
    {
      h = (char*)dbx_key(dp, ip->itemdata.item, NULL);
      dbx_term(dp);
    }
  if (h)
    return (char *const *)vec_from_str((char*)h, NULL, NULL);
  else
    return NULL;
}
