#include <oraccsys.h>
#include "../px.h"
#include "what.h"

/* We just check that there is an appropriate XTL file and that it has entries.
 * If so, we return 0 so it can be processed by isp_list;
 * If not, we emit an error page and return 2 to signal 'goto ok'
 */

int
what_sources(Isp *ip)
{
  return 1;
}
