#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_sig(Isp *ip)
{
  fprintf(stderr, "px: project=`%s'; literal sig=`%s'\n", ip->project, ip->sig);
  execl("/home/oracc/bin/wsigx", "wsigx", ip->project, ip->sig, NULL);
  ip->err = "failed to exec wsigx";
  return 1;
}
