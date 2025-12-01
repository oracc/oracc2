#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static const char *xsltproc_path = "/usr/bin/xsltproc";
static const char *xslt_script = "/home/oracc/lib/scripts/bib-HTML.xsl";

const char *
bx_xsltproc(Bx *bp)
{
  const char *modeparm = (bp->html_mode == BX_HTML_DIV ? " -stringparam outmode div" : "");
  char projparm[strlen(" -stringparam 0") + strlen(bp->project?bp->project:"")];
  if (bp->project)
    {
      strcpy(projparm, " stringparam ");
      strcat(projparm, bp->project);
    }
  else
    *projparm = '\0';
  char outarg[strlen(" -o0")+strlen(bp->outfile ? bp->outfile : "")];
  if (bp->outfile && strcmp(bp->outfile, "-"))
    sprintf(outarg, " -o %s", bp->outfile);
  else
    *outarg = '\0';
  char xbuf[strlen(xsltproc_path)+strlen(xslt_script)+strlen(modeparm)+strlen(projparm)+strlen(" -0")];
  sprintf(xbuf, "%s%s%s%s %s -", xsltproc_path, outarg, projparm, modeparm, xslt_script);
  return (ccp)pool_copy((uccp)xbuf, bp->p);
}
