#include <oraccsys.h>
#include <xpd.h>
#include "signlist.h"


static Pool *xpdpool;
static struct xpd *xpdp;

/* Read config variables from 00lib/config.xml */
void
asl_config(const char *project, struct sl_config *cp)
{
  if (project)
    {
      xpdpool = pool_init();
      xpdp = xpd_init(project, xpdpool);
      const char *val;
      cp->configged = 1;
      cp->project = project;
      if ((val = xpd_option(xpdp, "asl-domain")))
	cp->domain = val;
      if ((val = xpd_option(xpdp, "asl-script")))
	cp->script = cp->font = val;
      if ((val = xpd_option(xpdp, "asl-merge")))
	cp->merge = val;
      else if (cp->script)
	cp->merge = cp->script;
      if ((val = xpd_option(xpdp, "asl-nokeys")))
	cp->nokeys = !strcmp(val, "yes");
    }
  else if (xpdp)
    {
      pool_term(xpdpool);
      xpd_term(xpdp);
      xpdp = NULL;
      xpdpool = NULL;
    }
}
