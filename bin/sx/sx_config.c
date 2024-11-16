#include <oraccsys.h>
#include <xpd.h>
#include "signlist.h"

/* Read config variables from 00lib/config.xml */

struct sl_config sxconfig = 
{
  .signlist="csl",
  .domain="sl",
  .font="ofs-noto"
};

static Pool *xpdpool;
static struct xpd *xpdp;

void
sx_config(const char *project)
{
  if (project)
    {
      asl_config(project, &sxconfig);
    }
  else if (xpdp)
    {
      pool_term(xpdpool);
      xpd_term(xpdp);
      xpdp = NULL;
      xpdpool = NULL;
    }
}
