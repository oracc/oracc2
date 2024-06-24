#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

void
isp_htmd(Isp *ip)
{
  if (!ip->itemdata.htmd)
    {
      const char *p4cache = getenv("ORACC_P4_CACHE");
      
      if (p4cache)
	{
	  ip->itemdata.htmd = (ccp)pool_alloc(strlen(p4cache)+5, ip->p);
	  sprintf((char*)ip->itemdata.htmd, "%s/htm", p4cache);
	}
      else
	ip->itemdata.htmd = "/home/oracc/www/p4.d/htm";
    }
}
