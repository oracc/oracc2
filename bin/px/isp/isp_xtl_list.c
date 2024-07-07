#include <oraccsys.h>
#include "isp.h"

int
isp_xtl_list(Isp *ip)
{
  if (!ip->nowhat)
    {
      List *args = list_create(LIST_SINGLE);
      list_add(args, (void*)ip->oracc);
      list_add(args, (void*)"/bin/ispxtl.sh");
      list_add(args, " ");
      /* *.xtl lives in the same item directory as a composite, so use
	 *the itemdata project info to handle proxies correctly */
      list_add(args, (void*)ip->itemdata.proj);
      list_add(args, " ");
      list_add(args, (void*)ip->lloc.path);
      list_add(args, " ");
      list_add(args, (void*)ip->cache.list);

      unsigned char *syscmd = list_concat(args);

      if (ip->verbose)
	fprintf(stderr, "isp: isp_xtl_lst: %s\n", syscmd);

      if (system((ccp)syscmd))
	{
	  ip->err = PX_ERROR_START "fatal: isp_xtl_list failed system call:\n\n\t%s\n";
	  ip->errx = (ccp)syscmd;
	  return 1;
	}
    }  
  return 0;
}

int
isp_is_xtl(Isp *ip)
{
  const char *ext;
  if (ip->list_name && (ext = strstr(ip->list_name, ".xtl")) && '\0' == ext[4])
    {
#if 0
      /* this should be redundant because it is called in what_sources */
      extern void what_sources_setup(Isp *ip);
      what_sources_setup(ip);
#endif
      return 1;
    }
  return 0;
}
