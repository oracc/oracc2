#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

void
isp_list_cemd(Isp *ip)
{
  const char *path = ip->lloc.path;
  int ln = 1;
  char *lp = nth_line(ip, path, ln, 1);
  if (!lp)
    return;
  while (lp && '#' == *lp)
    lp = nth_line(ip, NULL, ++ln, 1);
  (void)nth_line(ip, NULL,-1,-1);
  if (lp)
    {
      char *colon = strchr(lp, ':');
      if (colon)
	lp = colon+1;
      if ('o' == *lp || 'x' == *lp)
	{
	  ip->data = "dglo";
	  ip->show = "rglo";
	  ip->cemd = "cglo";
	  ip->ceid = "cbd";
	}
      else if (strchr(lp, '_'))
	{
	  ip->data = "dtra";
	  ip->show = "rref";
	  ip->cemd = "ctra";
	  ip->ceid = "tra";
	}
      else if (strchr(lp, '.'))
	{
	  ip->data = "dtxt";
	  ip->show = "rref";
	  if (!ip->cemd)
	    ip->cemd = "line";
	  ip->ceid = "xtf";
	}
      else
	{
	  ip->data = "dcat";
	  ip->show = "rcat";
	  ip->cemd = "ccat";
	  ip->ceid = "xmd";
	}
    }
  else
    ip->err = "unable to set CE type from list contents";
}

int
isp_list_create(Isp *ip)
{
  if (ip->lloc.method && !strcmp(ip->lloc.method, "file"))
    {
      if (strcmp(ip->list_name, "outlined.lst"))
	{
	  /* defensive programming: */
	  if (strcmp(ip->lloc.path, ip->cache.list))
	    {
	      int ret = file_copy(ip->lloc.path, ip->cache.list);
	      if (ret)
		{
#if 1
		  ip->err = px_err("isp_list_create: failed to copy %s to %s\n", ip->lloc.path, ip->cache.list);
#else
		  ip->err = PX_ERROR_START "isp_list_create: failed to copy %s to cache-list";
		  ip->errx = ip->lloc.path;
#endif
		}
	      return ret;
	    }
	}
      else
	{
	  if (ip->pub_output)
	    file_copy(ip->lloc.path, ip->cache.list);
	}
    }
  else if ('t' == *ip->lloc.type)
    return isp_tis_list(ip);
  else if (!strcmp(ip->lloc.type, "xtl"))
    return isp_xtl_list(ip);
  else
    return isp_xis_list(ip);
  return 0;
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
	fprintf(stderr, "isp: isp_list_dbx: looking for dbi %s\n", dbi);
      if (!access(dbi, R_OK))
	return 0;
    }
  return 1;
}

static int
isp_try_list(Isp *ip, const char *dir)
{
  char path[strlen(ip->oracc)+strlen(dir)+2+
	    +strlen(ip->project)+strlen("/lists/")
	    +strlen(ip->list_name)];
  sprintf(path, "%s/%s/%s/lists/%s", ip->oracc, dir, ip->project, ip->list_name);

  if (!access(path, R_OK))
    {
      ip->lloc.path = (ccp)pool_copy((uccp)path, ip->p);
      ip->lloc.method = "file";
      ip->lloc.type = "www";
      return 0;
    }
  return 1;
}

int
isp_try_bld_list(Isp *ip)
{
  return isp_try_list(ip, "bld");
}

int
isp_try_www_list(Isp *ip)
{
  return isp_try_list(ip, "www");
}

void
isp_list_type(Isp *ip)
{
  char *p = NULL;

  if (ip->glos && !ip->glosdata.xis)
    ip->lloc.type = "glo";
  else
    {
      if (!strncmp(ip->list_name, "is.", 3))
	ip->lloc.type = "isp";
      else if (ip->srchdata.tmp)
	{
	  ip->lloc.type = "srch";
	}
      else if (ip->glosdata.xis)
	{
	  char l[strlen(ip->glosdata.xis)+1];
	  strcpy(l,ip->glosdata.xis);
	  ip->lloc.type = "xis";
	  p = strchr(l, '.');
	  *p = '\0';
	  ip->lloc.lang = (ccp)pool_copy((uccp)l, ip->p);
	  *p = '.';
	}
      else if ('t' == *ip->list_name && strlen(TIS_TEMPLATE) == strlen(ip->list_name))
	ip->lloc.type = "tis";
      else
	ip->lloc.type = "www";
    }
}

/* This routine cannot use ip->cache */
int
isp_list_method(Isp *ip)
{
  ip->from = "list";

  if (isp_is_xtl(ip))
    return ip->err ? 1 : 0;

  /* Only call isp_list_type if the list doesn't exist in www/lists or
     bld/lists (the latter for precomputation during build process) */
  if (isp_try_www_list(ip) || isp_try_bld_list(ip))
    isp_list_type(ip);
  
  if ('w' == *ip->lloc.type) /* www */
    {
      if (!ip->lloc.method && isp_try_www_list(ip) && isp_try_bld_list(ip))
	ip->err = "list file not found in project www or bld lists directories";
      else
	isp_list_cemd(ip);
    }
  else if ('i' == *ip->lloc.type || 's' == *ip->lloc.type) /* isp */
    {
      isp_tmp_dir(ip);
      char path[strlen(ip->tmp_dir)+strlen("/list")];
      sprintf(path, "%s/list", ip->tmp_dir);
      if (access(path, R_OK))
	ip->err = (ccp)px_err("list %s not found in tmpdir: %s", path, strerror(errno));
      else
	{
	  ip->lloc.path = (ccp)pool_copy((uccp)path, ip->p);
	  ip->lloc.method = "file";
	  isp_list_cemd(ip);
	}
    }
  else if ('x' == *ip->lloc.type) /* xis */
    {
      char p[strlen(ip->oracc)+strlen("/pub//cbd/")+strlen(ip->glos)+1];
      sprintf(p, "%s/pub/%s/cbd/%s", ip->oracc, ip->project, ip->glos);
      ip->lloc.dbpath = (ccp)pool_copy((uccp)p, ip->p);
      ip->lloc.dbname = "tis";
      ip->lloc.method = "xisdb";
      if (isp_list_dbx(ip))
	{
	  ip->err = PX_ERROR_START "xis database %s/tis.dbh not found\n";
	  ip->errx = ip->lloc.dbpath;
	}
    }
  else if ('t' == *ip->lloc.type) /* tis */
    {
      char p[strlen(ip->oracc)+strlen("/pub/")+strlen(ip->project)+strlen("/tok")+1];
      sprintf(p, "%s/pub/%s/tok", ip->oracc, ip->project);
      ip->lloc.dbpath = (ccp)pool_copy((uccp)p, ip->p);
      ip->lloc.dbname = "tok";
      ip->lloc.method = "dbx";
      if (isp_list_dbx(ip))
	{
	  ip->err = PX_ERROR_START "tok/tis database %s/tok.dbh not found\n";
	  ip->errx = ip->lloc.dbpath;
	}
    }
  else if ('g' == *ip->lloc.type) /* glossary */
    {
      /* deferred until partial cache setting */
    }
  else
    {
      ip->err = "bad type in list locator";
    }
  return ip->err ? 1 : 0;
}
