#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_tei(Isp *ip)
{
  ip->wrap = "xtf2tei";
  return what_wrapper(ip);
}
