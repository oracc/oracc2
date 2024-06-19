#include <oraccsys.h>
#include "../isp/isp.h"
#include "what.h"

int
what(Isp *ip)
{
  int ret = 0;
  if (!strcmp(ip->what, "sig"))
    ret = what_sig(ip);
  {
    if (!ip->itemdata.item)
      ip->itemdata.item = ip->item;
    ip->itemdata.proj = isp_dbx_one_off(ip, ip->project, "02pub", "prx", ip->itemdata.item, NULL);

    if (!strcmp(ip->what, "score"))
      ret = what_score(ip);
    else if (!strcmp(ip->what, "sources"))
      ret = what_sources(ip);
  }
  return ret;
}
