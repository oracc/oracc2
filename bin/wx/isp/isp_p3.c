#include <oraccsys.h>
#include "isp.h"

int
isp_p3(Isp *ip, FILE *outfp)
{
  fprintf(outfp, "%d:%s:::%s", ip->md1.zimx, ip->cache.page, ip->cache.zout);
  return 0;
}
