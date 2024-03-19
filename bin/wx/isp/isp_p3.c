#include <oraccsys.h>
#include "isp.h"

int
isp_p3(Isp *ip, FILE *outfp)
{
  fprintf(outfp, "%s:::%s", ip->cache.page, ip->cache.zout);
  return 0;
}
