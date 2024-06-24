#include <oraccsys.h>
#include "../isp/isp.h"
#include "what.h"

int
what(Isp *ip)
{
  if (ip->item || ip->itemdata.item)
    {
      if (!ip->itemdata.item)
	ip->itemdata.item = ip->item;
      ip->itemdata.proj = isp_dbx_one_off(ip, ip->project, "02pub", "prx", ip->itemdata.item, NULL);
    }
  struct whatfuncstab *w = whatfuncs(ip->what, strlen(ip->what));
  if (w)
    return w->func(ip);
  else
    {
      ip->err = px_err("fatal: unknown 'what' request '%s'\n", ip->what);
      return PX_ERROR;
    }
}
