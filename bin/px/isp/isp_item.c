#include <oraccsys.h>
#include "isp.h"

int
isp_item_set(Isp *ip)
{
  char dbifn[strlen(ip->perm)+strlen("-itm0")];
  sprintf(dbifn, "%s-itm", ip->perm);
  Dbi_index *dp = dbx_init(ip->cache.sub, dbifn);
  char *k = (char*)dbx_key(dp, ip->item, NULL);
  if (!k)
    {
      ip->err = "key %s not found in item db\n";
      ip->errx = ip->item;
      return 1;
    }
  /* This implementation doesn't support creating an unzoomed pager from an item yet */
  char *s = k;
  while (*s && !isspace(*s))
    ++s;
  while (*s && isspace(*s))
    ++s;
  ip->zoom = s;
  while (*s && !isspace(*s))
    ++s;
  while (*s && isspace(*s))
    ++s;
  ip->page = k;
  return 0;
}
