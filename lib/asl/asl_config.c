#include <oraccsys.h>
#include <xpd.h>
#include "signlist.h"

/* Read config variables from 00lib/config.xml */

struct sl_config aslconfig = 
{
  .signlist="csl",
  .domain="sl",
  .font="noto"
};

static Pool *xpdpool;
static struct xpd *xpdp;

void
asl_config(const char *project)
{
  if (project)
    {
      xpdpool = pool_init();
      xpdp = xpd_init(project, xpdpool);
      const char *val;
      aslconfig.project = project;
      if ((val = xpd_option(xpdp, "asl-domain")))
	aslconfig.domain = val;
      if ((val = xpd_option(xpdp, "asl-script")))
	aslconfig.script = aslconfig.font = val;
      if ((val = xpd_option(xpdp, "asl-merge")))
	aslconfig.merge = val;
      else if (aslconfig.script)
	aslconfig.merge = aslconfig.script;
    }
  else if (xpdp)
    {
      pool_term(xpdpool);
      xpd_term(xpdp);
      xpdp = NULL;
      xpdpool = NULL;
    }
}
