#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_wrapper(Isp *ip)
{
  char qpq[strlen(ip->itemdata.proj)+strlen(ip->itemdata.item)+2];
  sprintf(qpq, "%s:%s", ip->itemdata.proj, ip->itemdata.item);
  char bin[strlen(oracc())+strlen("/bin")+strlen(ip->wrap) + 2];
  sprintf(bin, "%s/bin/%s", oracc(), ip->wrap);
  execl(bin, ip->wrap, "-vw", qpq, NULL);
  ip->err = "failed to exec wrapper";
  return 1;
}
