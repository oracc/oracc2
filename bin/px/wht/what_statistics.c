#include <oraccsys.h>
#include "../pxdefs.h"
#include "../wpx_lib.h"
#include "what.h"

const char *stats_html = "/lib/scripts/p4-stats-HTML.xsl";

int
what_statistics(Isp *ip)
{
  char xsl[strlen(oracc())+strlen(stats_html)+1];
  sprintf(xsl, "%s/%s", oracc(), stats_html);
  char data[strlen(ip->project) + strlen(oracc()) + strlen("/xml//project-data.xml") + 1];
  sprintf(data, "%s/xml/%s/project-data.xml", oracc(), ip->project);
  wpx_print_hdr(ip);
  execl("/usr/bin/xsltproc", "xsltproc", stats_html, data, NULL);
  ip->err = "fatal: what_statistics failed";
  return PX_ERROR;
}
