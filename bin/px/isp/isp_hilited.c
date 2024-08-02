#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

const char *
isp_hilited(Isp *ip)
{
  char *dbifn = "hilite";
  Dbi_index *dp;
  const char *dir = ip->tmp_dir ? ip->tmp_dir : ip->cache.sub;
  if (dir)
    {
      const char *h = NULL;
      if (!dbx_access(dir, dbifn))
	{
	  if ((dp = dbx_init(dir, dbifn)))
	    {
	      h = (ccp)dbx_key(dp, ip->itemdata.item, NULL);
	      if (h)
		h = (ccp)pool_copy((ucp)h, ip->p);
	      dbx_term(dp);
	    }
	}
      else
	{
	  /* For now hilite is optional for all item lists; need to
	     require it for lists that are line/word/grapheme IDs */
#if 0
	  ip->err = (ccp)pool_copy((ucp)px_err("fatal: hilite db %s/%s could not be opened",
					       dir, dbifn), ip->p);
#endif
	}
      if (h)
	return (ccp)h;
      else
	return NULL;
    }
  return NULL;
}
