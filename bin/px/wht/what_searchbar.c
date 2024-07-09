#include <oraccsys.h>
#include "../pxdefs.h"
#include "../pui/pui.h"
#include "../wpx_lib.h"
#include "what.h"

int
what_searchbar(Isp *ip)
{
  ip->from = "sbar";
  wpx_print_hdr();
  if (pui_output(ip, stdout, pui_filetext("p4searchbar.xml")))
    return PX_ERROR;
  else
    return PX_DONE;
}
