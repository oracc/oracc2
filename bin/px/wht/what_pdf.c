#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_pdf(Isp *ip)
{
  expand_base(ip->itemdata.htmd);
  const char *htmd  = expand(ip->itemdata.proj, ip->itemdata.item, NULL);
  char bin[_MAX_PATH];
  sprintf(bin, "%s/bin/webpdf.sh", oracc());
  execl("/bin/sh", "sh", bin, ip->itemdata.proj, ip->itemdata.item, htmd, NULL);
  ip->err = "failed to exec webpdf.sh";
  return 1;
}
