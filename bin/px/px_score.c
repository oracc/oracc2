#include <oraccsys.h>
#include "px.h"

int
px_score(Isp *ip)
{
  if (!ip->itemdata.item)
    ip->itemdata.item = ip->item;
  ip->itemdata.proj = isp_dbx_one_off(ip, ip->project, "02pub", "prx", ip->itemdata.item, NULL);
  char *tmp = expand(ip->itemdata.proj,ip->item,"sxh");
  if (!access(tmp, R_OK))
    {
      if (ip->itemdata.block)
        {
	  wpx_print_hdr();
	  execl("/home/oracc/bin/xfragx", "xfragx", 
		"-hs4",
		"-p", ip->itemdata.proj,
		"-i", ip->itemdata.item,
		tmp, ip->itemdata.block, NULL);
        }
#if 0
      else
        {
          sigfixer_html(tmp);
        }
#endif
    }
  else
    {
      ip->err = px_err("fatal: no such score %s", tmp);
#if 0
      tmp = or_find_pqx_file(project, 
                             components[0].replace 
                             ? components[0].replace 
                             : components[0].text, "xsf");
      if (!access(tmp, R_OK))
        {
          h_pqx_html_handler(project, components, 1, "score");    
        }
      do404();
#endif
    }
  return 1;
}

