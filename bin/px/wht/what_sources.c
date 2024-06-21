#include <oraccsys.h>
#include "../px.h"
#include "what.h"

static void what_sources_setup(Isp *ip);

/* We just check that there is an appropriate XTL file and that it has entries.
 * If so, we return 0 so it can be processed by isp_list;
 * If not, we emit an error page and return 2 to signal 'goto ok'
 */

int
what_sources(Isp *ip)
{
  /* 
   * isp->list_name to the .xtl file
   * isp->lloc.path to the path to the xtl file
   * isp->lloc.method to xtl -- this short circuits the isp_list_method call in main
   */
  what_sources_setup(ip);
  ip->from = "list";
  ip->list_name = (ccp)pool_alloc(strlen(ip->itemdata.item)+5, ip->p);
  sprintf((char*)ip->list_name, "%s.xtl", ip->itemdata.item);
  if (!access(ip->lloc.path, R_OK))
    {
      return 0;
    }
  else
    {
      /* Don't error here; let the process produce the p4pager and
	 report the issue in the content */
      ip->nowhat = 1;
      return 0;
    }
  return 1;  
}

void
what_sources_setup(Isp *ip)
{
  char *xtl = expand(ip->itemdata.proj, ip->itemdata.item, "xtl");
  ip->lloc.path = (ccp)pool_copy((ucp)xtl, ip->p);
  ip->lloc.type = "xtl";
  ip->lloc.method = "xtl";
}
