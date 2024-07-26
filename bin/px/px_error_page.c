#include <oraccsys.h>
#include "px.h"

/* This handler emits HTML error pages for some error conditions; if
   there is no page for an error it returns 1, otherwise it returns
   0 */

int
px_error_page(Isp*ip)
{
  if (!ip->project || ip->pxerr == PX_ER_BAD_PROJECT)
    return pui_output(ip, stdout, pui_filetext("p4noproject.xml"));

  if (ip->project && ip->item)
    return pui_output(ip, stdout, pui_filetext("p4noitem.xml"));

  return 1;
}
