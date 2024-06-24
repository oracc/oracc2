#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_cuneify(Isp *ip)
{
  ip->wrap = "xtf2cun";
  return what_wrapper(ip);
}
