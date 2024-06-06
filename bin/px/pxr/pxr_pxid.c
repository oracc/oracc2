#include <oraccsys.h>
#include "../isp/isp.h"
#include "../pxdefs.h"
#include "../pui/pui.h"
#include "../wpx_lib.h"
#include "pxr.h"

/* Phase 1: default setting */
int
pxr_item_defaults(Isp *ip)
{
  if (!ip->what)
    ip->what = "text";

  if (!ip->part)
    {
      if (!strcmp(ip->what, "text"))
	ip->part = "full";
      else
	ip->part = "self";
    }

  if (!ip->form)
    ip->form = "html";

  return 0;
}

/* stub */
int pxr_pqx(Isp *ip) { return 0; }

/* Phase 2: determine support and required inputs
 *
 * There are three possible needs:
 *
 *	meta
 *	tlit
 *	xlat
 *
 * For xlat a LANG may be given which is taken as a request--if the
 * xlat does not exist in the given lang the first available xlat is
 * used.
 */
int
pxr_item_pathneeds(Isp *ip)
{
  if (!strcmp(ip->what, "text") && !strcmp(ip->form, "html"))
    {
      ip->data = "dtxt";
      ip->itemdata.item = ip->item;
      isp_item_xtf(ip); /* there's no list context so isp_item_set is not right */
    }
  else
    {
      if (!strcmp(ip->what, "meta"))
	ip->data = "dcat";
      else
	ip->data = "dtxt";
      pxr_pqx(ip);
    }
  return ip->err ? 1 : 0;
}

/* Phase 3: return outputs */
int
pxr_item_outputs(Isp *ip)
{
  wpx_print_hdr();
  if (!strcmp(ip->part, "full"))
    return pui_output(ip, stdout, pui_filetext("p4full.xml"));
  else if (!strcmp(ip->part, "plus"))
    return pui_output(ip, stdout, pui_filetext("p4plus.xml"));
  else
    ; /* futured */
  return 0;
}
