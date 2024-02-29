#include <oraccsys.h>
#include "isp.h"

void
isp_list_type(struct isp *ip)
{
  char *p = NULL;
  
  if (!strncmp(ip->list_name, "is.", 3))
    ip->lloc.type = "tmp";
  else if ((p = strchr(ip->list_name, '.')))
    {
      if (p - ip->list_name == 3)
	ip->lloc.type = "xis";
    }
  else if ('t' == *ip->list_name && strlen(TIS_TEMPLATE) == strlen(ip->list_name))
    ip->lloc.type = "tis";
  else
    ip->lloc.type = "www";
}

void
isp_list_location(struct isp *ip)
{
  if (!strcmp(ip->lloc.type, "www"))
    {
      char path[strlen(ip->oracc)+strlen("/www/")
		+strlen(ip->project)+strlen("/lists/")
		+strlen(ip->list_name)];
      sprintf(path, "%s/www/%s/lists/%s", ip->oracc, ip->project, ip->list_name);
      if (access(path, R_OK))
	ip->err = "list file not found in project web lists directory";
      else
	ip->lloc.path = (ccp)pool_copy((uccp)path, ip->p);
    }
  else if (!strcmp(ip->lloc.type, "tmp"))
    {
      isp_tmp_dir(ip);
      char path[strlen(ip->tmp_dir)+strlen("/list")];
      sprintf(path, "%s/list", ip->tmp_dir);
      if (access(path, R_OK))
	ip->err = "list not found in tmpdir";
      else
	ip->lloc.path = (ccp)pool_copy((uccp)path, ip->p);
    }
  else if (!strcmp(ip->lloc.type, "xis"))
    {
      
    }
}
