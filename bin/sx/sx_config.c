#include <oraccsys.h>
#include <xpd.h>
#include "sx.h"

/* Read config variables from 00lib/config.xml */

struct sx_config sxconfig = 
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
      xpdpool = pool_init();
      xpdp = xpd_init(project, xpdpool);
      const char *val;
      sxconfig.project = project;
      if ((val = xpd_option(xpdp, "asl-domain")))
	sxconfig.domain = val;
      if ((val = xpd_option(xpdp, "asl-script")))
	sxconfig.script = sxconfig.font = val;
      if ((val = xpd_option(xpdp, "asl-merge")))
	sxconfig.merge = val;
      else if (sxconfig.script)
	sxconfig.merge = sxconfig.script;
    }
  else if (xpdp)
    {
      pool_term(xpdpool);
      xpd_term(xpdp);
      xpdp = NULL;
      xpdpool = NULL;
    }
}
