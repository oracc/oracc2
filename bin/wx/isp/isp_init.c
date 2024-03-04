#include <oraccsys.h>
#include "isp.h"

Isp *
isp_init(void)
{
  Isp defaults = {
    .oracc=oracc_builds() , .list_name="_all" , .dors="0" , .perm="123" ,
    .cemd="line" , .xhmd="html" , .pack="asis" , .aapi="rest" ,
    .zoom="0" , .page="1" , .psiz="25"
  };
  Isp *ip = calloc(1, sizeof(Isp));
  *ip = defaults;
  ip->p = pool_init();
  return ip;
}

void
isp_term(Isp *ip)
{
  if (ip)
    {
      pool_term(ip->p);
      free(ip);
    }
}
