#include <oraccsys.h>
#include "cfy.h"

void
cfy_render(Cfy *c, const char *om)
{
  if (!strcmp(om, "xml"))
    cfy_out_xml(c);
  else if (!strcmp(om, "html"))
    cfy_out_html(c);
}
