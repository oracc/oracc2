#include <oraccsys.h>
#include "isp.h"

struct isp *
isp_init(void)
{
  struct isp defaults = {
    .oracc=oracc_builds() , .list_name="_all" ,
    .cemd="line" , .xhmd="html" , .pack="asis" , .aapi="rest" ,
    .zoom="0" , .page="1" , .size="1"
  };
  struct isp *ip = calloc(1, sizeof(struct isp));
  *ip = defaults;
  ip->p = pool_init();
  return ip;
}

void
isp_term(struct isp *ip)
{
  if (ip)
    {
      pool_term(ip->p);
      free(ip);
    }
}
