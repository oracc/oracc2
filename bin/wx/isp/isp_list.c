#include <oraccsys.h>
#include "isp.h"

void
isp_list_type(struct isp *ip)
{
  char *p = NULL;
  
  if (!strncmp(ip->list_name, "is.", 3))
    ip->lloc.type = "isp";
  else if ((p = strchr(ip->list_name, '.')))
    {
      ip->lloc.type = "xis";
      *p = '\0';
      ip->lloc.lang = (ccp)pool_copy((uccp)ip->list_name, ip->p);
      *p = '.';
    }
  else if ('t' == *ip->list_name && strlen(TIS_TEMPLATE) == strlen(ip->list_name))
    ip->lloc.type = "tis";
  else
    ip->lloc.type = "www";
}

void
isp_list_location(struct isp *ip)
{
  if ('w' == *ip->lloc.type) /* www */
    {
      char path[strlen(ip->oracc)+strlen("/www/")
		+strlen(ip->project)+strlen("/lists/")
		+strlen(ip->list_name)];
      sprintf(path, "%s/www/%s/lists/%s", ip->oracc, ip->project, ip->list_name);
      if (access(path, R_OK))
	ip->err = "list file not found in project web lists directory";
      else
	{
	  ip->lloc.path = (ccp)pool_copy((uccp)path, ip->p);
	  ip->lloc.method = "file";
	}
    }
  else if ('i' == *ip->lloc.type) /* isp */
    {
      isp_tmp_dir(ip);
      char path[strlen(ip->tmp_dir)+strlen("/list")];
      sprintf(path, "%s/list", ip->tmp_dir);
      if (access(path, R_OK))
	ip->err = "list not found in tmpdir";
      else
	{
	  ip->lloc.path = (ccp)pool_copy((uccp)path, ip->p);
	  ip->lloc.method = "file";
	}
    }
  else if ('x' == *ip->lloc.type) /* xis */
    {
      char p[strlen(ip->oracc)+strlen("/pub/cbd/.tis")+strlen(ip->lloc.lang)+1];
      sprintf(p, "%s/pub/%s/cbd/%s.tis", ip->oracc, ip->project, ip->lloc.lang);
      ip->lloc.dbpath = (ccp)pool_copy((uccp)p, ip->p);
      ip->lloc.method = "xisdb";
    }
  else if ('t' == *ip->lloc.type) /* tis */
    {
      char p[strlen(ip->oracc)+strlen("/pub/")+strlen(ip->project)+strlen("/tok")+1];
      sprintf(p, "%s/pub/%s/tok", ip->oracc, ip->project);
      ip->lloc.dbpath = (ccp)pool_copy((uccp)p, ip->p);
      ip->lloc.dbname = "tok";
      ip->lloc.method = "dbx";
    }
}
