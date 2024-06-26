#include <oraccsys.h>
#include "../pxdefs.h"
#include "../wpx_lib.h"
#include "what.h"

const char *stats_html = "/home/oracc/lib/scripts/p4-stats-HTML.xsl";

int
what_statistics(Isp *ip)
{
  char data[strlen(ip->project) + strlen("/home/oracc/xml//project-data.xml") + 1];
  sprintf(data, "/home/oracc/xml/%s/project-data.xml", ip->project);
  wpx_print_hdr();
  execl("/usr/bin/xsltproc", "xsltproc", stats_html, data, NULL);
  ip->err = "fatal: what_statistics failed";
  return PX_ERROR;
}
