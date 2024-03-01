#include <oraccsys.h>
#include "isp.h"

int
isp_tis_list(Isp *ip)
{
  Dbi_index *dp = dbx_init(ip->lloc.dbpath, ip->lloc.dbname);
  if (dp)
    {
      Unsigned32 len;
      dp->h.sep_char = '\n';
      const Loc8*l8p = dbx_key(dp, ip->list_name, &len);
      if (l8p)
	{
	  FILE *fp;
	  if (!(fp = fopen(ip->cache.list, "w")))
	    ip->err = "isp_list_from_dbx failed to open tis dbi";
	  else
	    dbx_wids(dp, l8p, len, fp);
	}
      dbx_term(dp);
    }
  else
    ip->err = "failed to open .dbh/.dbi database";
  return ip->err ? 1 : 0;
}

int
isp_list_create(Isp *ip)
{
  if ('w' == *ip->lloc.type)
    return file_copy(ip->lloc.path, ip->cache.list);
  else if ('t' == *ip->lloc.type)
    return isp_tis_list(ip);
  else
    return 0/*isp_xis_list(ip)*/;
}

int
isp_list_dbx(Isp *ip)
{
  struct stat sb;
  if (!stat(ip->lloc.dbpath, &sb) && S_ISDIR(sb.st_mode))
    {
      char dbi[strlen(ip->lloc.dbpath)+strlen(ip->lloc.dbname)+strlen("/.dbi0")];
      sprintf(dbi, "%s/%s.dbi", ip->lloc.dbpath, ip->lloc.dbname);
      if (ip->verbose)
	fprintf(stderr, "isp_list_dbx: looking for dbi %s\n", dbi);
      if (!access(dbi, R_OK))
	return 0;
    }
  return 1;
}

void
isp_list_type(Isp *ip)
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

int
isp_list_location(Isp *ip)
{
  isp_list_type(ip);
  
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
      char p[strlen(ip->oracc)+strlen("/pub//cbd/")+strlen(ip->lloc.lang)+1];
      sprintf(p, "%s/pub/%s/cbd/%s", ip->oracc, ip->project, ip->lloc.lang);
      ip->lloc.dbpath = (ccp)pool_copy((uccp)p, ip->p);
      ip->lloc.dbname = "tis";
      ip->lloc.method = "xisdb";
      if (isp_list_dbx(ip))
	ip->err = "xis database not found";
    }
  else if ('t' == *ip->lloc.type) /* tis */
    {
      char p[strlen(ip->oracc)+strlen("/pub/")+strlen(ip->project)+strlen("/tok")+1];
      sprintf(p, "%s/pub/%s/tok", ip->oracc, ip->project);
      ip->lloc.dbpath = (ccp)pool_copy((uccp)p, ip->p);
      ip->lloc.dbname = "tok";
      ip->lloc.method = "dbx";
      if (isp_list_dbx(ip))
	ip->err = "tok/tis database not found";
    }
  else
    {
      ip->err = "bad type in list locator";
    }
  return ip->err ? 1 : 0;
}
