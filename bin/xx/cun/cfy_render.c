#include <oraccsys.h>
#include "cfy.h"

void
cfy_render(Cfy *c, FILE *fp, const char *om)
{
  if (!strcmp(om, "xml"))
    cfy_out_xml(c, fp);
}
