#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_proofing(Isp *ip)
{
  ip->wrap = "xtf2prf";
  return what_wrapper(ip);
}
