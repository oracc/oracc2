#include <oraccsys.h>
#include "isp.h"

int
isp_item_load(Isp *ip)
{
  char dbifn[strlen(ip->perm)+strlen("-itm0")];
  sprintf(dbifn, "%s-itm", ip->perm);
  Dbi_index *dp = dbx_init(ip->cache.sub, dbifn);
  char *k = (char*)dbx_key(dp, ip->item, NULL);
  if (!k)
    {
      ip->err = PX_ERROR_START "key %s not found in item db\n";
      ip->errx = ip->item;
      return 1;
    }

  ip->itemdata.page = (ccp)pool_copy((ucp)k, ip->p);
  
  char *s = strchr(ip->itemdata.page, '\t');
  *s++ = '\0';

  ip->itemdata.zoom = s;
  s = strchr(s, '\t');
  *s++ = '\0';
  
  ip->itemdata.zpag = s;
  s = strchr(s, '\t');
  *s++ = '\0';

  if ('#' == *s)
    ip->itemdata.prev = NULL;
  else
    ip->itemdata.prev = s;
  s = strchr(s, '\t');
  *s++ = '\0';

  if ('#' == *s)
    ip->itemdata.next = NULL;
  else
    ip->itemdata.next = s;
  
  return 0;
}

int
isp_item_set(Isp *ip)
{
  if (isp_item_load(ip))
    return 1;
  
  /* This implementation doesn't support creating an unzoomed pager from an item yet */
  ip->zoom = ip->itemdata.zoom;
  ip->page = ip->itemdata.zpag;

  if (!(ip->itemdata.xmdxsl = isp_xmd_outline(ip)))
    return 1;

  return 0;
}
