#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_score(Isp *ip)
{
  if (ip->itemdata.block)
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
	}
      else
	{
	  what_not(ip, "score block");
	  return PX_DONE;
	}      
    }
  else
    {
      if (what_html(ip))
	return PX_ERROR;
      else
	return PX_DONE;
    }
  return PX_DONE;
}

