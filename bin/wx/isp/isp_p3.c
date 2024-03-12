#include <oraccsys.h>
#include "isp.h"

int
isp_p3(Isp *ip, FILE *outfp)
{
  fprintf(outfp, "%s", ip->cache.sub);
  return 0;
}
