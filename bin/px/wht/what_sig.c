#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_sig(Isp *ip)
{
  if (!sigmap_item(ip))
    {
      ip->data = "dglo";
      ip->from = "sig";
      ip->item = ip->glosdata.ent;
      wpx_print_hdr(ip);
      pui_output(ip, stdout, pui_filetext("p4article.xml"));
      return PX_DONE;
    }
  else
    return PX_ERROR;
}
