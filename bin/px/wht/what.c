#include <oraccsys.h>
#include "../isp/isp.h"
#include "what.h"

int
what(Isp *ip)
{
  if (!ip->from)
    ip->from = "url";
  
  if (!ip->form)
    ip->form = "html";

  if (!ip->part)
    {
      if (!strcmp(ip->what, "text"))
	ip->part = "full";
      else
	ip->part = "self";
    }

  /* If the score block is a ref out from a witness to the score,
     reset ip->item to be the score not the witness before we get
     itemdata */
  if (ip->form && !strcmp(ip->form, "block") && ip->item && !strstr(ip->item, ip->itemdata.block))
    {
      char *b = (char*)pool_copy((ucp)ip->itemdata.block+3, ip->p);
      ip->item = b;
      b = strchr(b, '.');
      *b = '\0';
    }
  
  if (ip->item || ip->itemdata.item)
    {
      if (!ip->itemdata.item)
	ip->itemdata.item = ip->item;
      ip->itemdata.proj = isp_dbx_one_off(ip, ip->project,
					  "02pub", "prx",
					  ip->itemdata.item, NULL);
      isp_htmd(ip);
    }
  struct whatfuncstab *w = whatfuncs(ip->what, strlen(ip->what));
  if (w)
    return w->func(ip);
  else
    {
      ip->err = px_err("fatal: unknown 'what' request '%s'", ip->what);
      return PX_ERROR;
    }
}
