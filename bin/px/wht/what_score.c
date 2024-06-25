#include <oraccsys.h>
#include "../px.h"
#include "what.h"

static int what_sources(Isp *ip);
static void what_sources_setup(Isp *ip);

int
what_score(Isp *ip)
{
  if (!ip->form)
    {
      if (ip->itemdata.block)
	ip->form = "block";
      else
	ip->form = "html";
    }

  if (!strcmp(ip->form, "block"))
    {
      char *tmp = expand(ip->itemdata.proj,ip->item,"sxh");
      if (!access(tmp, R_OK))
	{
	  wpx_print_hdr();
	  execl("/home/oracc/bin/xfragx", "xfragx", 
		"-hs4",
		"-p", ip->itemdata.proj,
		"-i", ip->itemdata.item,
		tmp, ip->itemdata.block, NULL);
	  return PX_ERROR;
	}
      else
	{
	  what_not(ip, "score block");
	  return PX_DONE;
	}      
    }
  else if (!strcmp(ip->form, "sources"))
    return what_sources(ip);
  else
    return what_html(ip);
}

/* We just check that there is an appropriate XTL file and that it has entries.
 * If so, we return 0 so it can be processed by isp_list;
 * If not, we emit an error page and return 2 to signal 'goto ok'
 */
static int
what_sources(Isp *ip)
{
  what_sources_setup(ip);
  ip->what = "pager";
  ip->from = "list";
  ip->item = NULL;
  ip->list_name = (ccp)pool_alloc(strlen(ip->itemdata.item)+5, ip->p);
  sprintf((char*)ip->list_name, "%s.xtl", ip->itemdata.item);
  if (!access(ip->lloc.path, R_OK))
    {
      return PX_CONTINUE;
    }
  else
    {
      /* Don't error here; let the process produce the p4pager and
	 report the issue in the content */
      ip->nowhat = 1;
      return PX_CONTINUE;
    }
  return PX_ERROR;  
}

/* Set:
 *
 * isp->list_name to the .xtl file
 * isp->lloc.path to the path to the xtl file
 * isp->lloc.method to xtl -- this short circuits the isp_list_method call in main
 *
 */
static void
what_sources_setup(Isp *ip)
{
  char *xtl = expand(ip->itemdata.proj, ip->itemdata.item, "xtl");
  ip->lloc.path = (ccp)pool_copy((ucp)xtl, ip->p);
  ip->lloc.type = "xtl";
  ip->lloc.method = "xtl";
}
