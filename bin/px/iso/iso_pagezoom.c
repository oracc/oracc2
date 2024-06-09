#include <oraccsys.h>
#include "iso.h"

int
iso_page_zoom(Isp *ip)
{
  int need_zout = 1;
  if (!access(ip->cache.zout, F_OK))
    {
      if (access(ip->cache.zout, R_OK))
	{
	  ip->err = PX_ERROR_START "cache zoom %s exists but is unreadable\n";
	  ip->errx = ip->cache.zout;
	  return 1;
	}
      need_zout = 0;
    }

  if (need_zout || ip->force)
    if (iso_zoutline(ip))
      return 1;

  return 0;
}
