#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"
#include "../iso/iso.h"

int
isp_cache_sys(Isp *ip)
{
  if (!ip->cache.sys)
    {
      ip->cache.sys = getenv("ORACC_P4_CACHE");
      if (!ip->cache.sys)
	{
	  char dir[strlen(ip->oracc)+strlen("/www/p4.d0")];
	  sprintf(dir, "%s/www/p4.d", ip->oracc);
	  ip->cache.sys = (ccp)pool_copy((uccp)dir, ip->p);
	}
      struct stat sb;
      if (stat(ip->cache.sys, &sb) || !S_ISDIR(sb.st_mode))
	ip->err = PX_ERROR_START "fatal: cache.sys directory %s not found\n";
      else if (access(ip->cache.sys, W_OK))
	ip->err = PX_ERROR_START "fatal: cache.sys directory %s not writeable\n";
    }
  if (ip->err)
    ip->errx = ip->cache.sys;
  return ip->err ? 1 : 0;
}

int
isp_cache_project(Isp *ip)
{
  int len = strlen(ip->cache.sys)+strlen(ip->project)+2;
  char dir[len];
  sprintf(dir, "%s/%s", ip->cache.sys, ip->project);
  ip->cache.project = (ccp)pool_copy((ucp)dir, ip->p);

  int len2 = strlen(ip->oracc)+strlen(ip->project)+strlen("//02pub/p4.d0");
  char pub[len2];
  sprintf(pub, "%s/%s/02pub/p4.d", ip->oracc, ip->project);
  ip->cache.pub = (ccp)pool_copy((ucp)pub, ip->p);
  return 0;
}

static int
xmkdirs(Isp *ip, const char *dir)
{
  struct stat sb;
  if (stat(dir, &sb) || !S_ISDIR(sb.st_mode))
    {
      if (strchr(dir, '/'))
	{
	  char xdir[strlen(dir)+1];
	  strcpy(xdir, dir);
	  if ('/' == xdir[strlen(xdir)-1])
	    xdir[strlen(xdir)-1] = '\0';
	  char *slash = xdir+1;
	  while ((slash = strchr(slash,'/')))
	    {
	      *slash = '\0';
	      if (stat(xdir, &sb) || !S_ISDIR(sb.st_mode))
		{
		  if (ip->verbose)
		    fprintf(stderr, "isp: isp_cache_sub: creating %s\n", xdir);
		  if (mkdir(xdir, 0775))
		    {
		      ip->err = PX_ERROR_START "fatal: cache.sub[1] project-component directory %s could not be created";
		      ip->errx = (ccp)pool_copy((ucp)xdir, ip->p);
		      break;
		    }
		}
	      *slash++ = '/';
	    }
	  if (!ip->err)
	    {
	      if (ip->verbose)
		fprintf(stderr, "isp: isp_cache_sub: creating %s\n", xdir);
	      if (mkdir(xdir, 0775))
		{
		  ip->err = PX_ERROR_START "fatal: cache.sub[2] project-component directory %s could not be created";
		  ip->errx = (ccp)pool_copy((ucp)xdir, ip->p);
		}
	    }
	}
      else
	{
	  if (ip->verbose)
	    fprintf(stderr, "isp: isp_cache_sub: creating %s\n", dir);
	  if (mkdir(dir, 0775))
	    {
	      ip->err = PX_ERROR_START "fatal: cache.sub[3] project-level directory %s could not be created";
	      ip->errx = (ccp)pool_copy((ucp)dir, ip->p);
	    }
	}
    }

  return ip->err ? 1 : 0;
}

/* if there is no directory in the cache for this list create one */
int
isp_cache_sub(Isp *ip)
{
  if (ip->cache.sub)
    {
      struct stat sb;
      if (stat(ip->cache.sub, &sb) || !S_ISDIR(sb.st_mode))
	{
	  ip->err = (ccp)pool_copy((ucp)px_err("isp_cache_sub: cache sub %s not found",
					       ip->cache.sub), ip->p);
	  return 1;
	}
    }
  else
    {
      /* If args set xis or tmp_dir they also need to reset
	 opt_ip->list_name to "list" */
      if (!strcmp(ip->list_name, "outlined.lst") || !strcmp(ip->list_name, "txtindex.lst"))
	{
	  ip->cache.use = ip->cache.pub;
	  char dir[strlen(ip->cache.use)+strlen(ip->list_name)+2];
	  sprintf(dir, "%s/%s", ip->cache.use, ip->list_name);
	  ip->cache.sub = (ccp)pool_copy((uccp)dir, ip->p);
	  if (xmkdirs(ip, ip->cache.sub))
	    return 1;

	  /* issx doesn't use this output so don't create it */
	  if (!ip->pub_output)
	    {
	      char outd[strlen(ip->cache.project)+strlen(ip->list_name)+2];
	      sprintf(outd, "%s/%s", ip->cache.project, ip->list_name);
	      ip->cache.out = (ccp)pool_copy((uccp)outd, ip->p);
	      if (xmkdirs(ip, ip->cache.out))
		return 1;
	    }
	}
      else
	{
	  ip->cache.use = ip->cache.project;
	  
	  if (ip->glosdata.xis)
	    {
	      char dir[strlen(ip->cache.use)+strlen(ip->glos)+strlen(ip->glosdata.xis)+2];
	      sprintf(dir, "%s/%s/%s", ip->cache.use, ip->glos, ip->glosdata.xis);
	    }
	  else if (ip->srchdata.tmp)
	    {
	      char dir[strlen(ip->cache.use)+strlen(ip->srchdata.list)+2];
	      sprintf(dir, "%s/%s", ip->cache.use, ip->srchdata.list);
	    }
	  else if (ip->lloc.key)
	    {
	      char dir[strlen(ip->cache.use)+strlen(ip->lloc.key)+2];
	      sprintf(dir, "%s/%s", ip->cache.use, ip->lloc.key);
	    }
	  else
	    {
	      char dir[strlen(ip->cache.use)+strlen(ip->list_name)+2];
	      sprintf(dir, "%s/%s", ip->cache.use, ip->list_name);
	    }

	  ip->cache.sub = (ccp)pool_copy((uccp)dir, ip->p);
	  ip->cache.out = ip->cache.sub;
	  if (xmkdirs(ip, ip->cache.sub))
	    return 1;
	}
    }  

  return ip->err ? 1 : 0;
}

/* if there is no list file in the cache.sub and the list type is not
   isp, try to create the list based on the specs given in ip->lloc */
int
isp_cache_list(Isp *ip)
{
  char list[strlen(ip->cache.sub)+strlen("/list0")];
  sprintf(list, "%s/list", ip->cache.sub);
  ip->cache.list = (ccp)pool_copy((uccp)list, ip->p);
  if (ip->force)
    {
      (void)isp_list_create(ip);
      isp_list_cemd(ip);
    }
  else
    {
      if (access(list, R_OK))
	{
	  if (!access(list, F_OK))
	    {
	      ip->err = "cache.list %s exists but is not readable";
	      ip->errx = strdup(list);
	    }
	  else if ('i' == *ip->lloc.type || 's' == *ip->lloc.type)
	    ip->err = "cache.list does not exist in tmp/isp";
	  else
	    {
	      if (!isp_list_create(ip))
		{
		  if (!ip->data)
		    {
		      const char *p = ip->lloc.path;
		      ip->lloc.path = ip->cache.list;
		      isp_list_cemd(ip);
		      ip->lloc.path = p;
		    }
		}
	    }
	}
      else
	{
	  if (!ip->data)
	    {
	      const char *p = ip->lloc.path;
	      ip->lloc.path = ip->cache.list;
	      isp_list_cemd(ip);
	      ip->lloc.path = p;
	    }
	}
    }
  return ip->err ? 1 : 0;
}

int
isp_cache_page(Isp *ip)
{
  if (ip->glos && !ip->glosdata.xis)
    {
      ip->cache.zout = (ccp)pool_alloc(strlen(ip->cache.sub)+strlen("/letters.div0"), ip->p);
      sprintf((char*)ip->cache.zout, "%s/letters.div", ip->cache.sub);
      if (!ip->item && ip->glosdata.ent)
	ip->item = ip->glosdata.ent;
      if (ip->item)
	{
	  const char *etm = isp_glos_etm(ip);
	  if (etm)
	    {
	      fprintf(stderr, "%s => %s\n", ip->item, etm);
	      isp_item_undump((char*)pool_copy((ucp)etm, ip->p), &ip->itemdata);
	      ip->zoom = ip->itemdata.zoom;
	      ip->glosdata.let = ip->itemdata.pindex;
	      ip->itemdata.pindex = NULL;
	      ip->glosdata.lmax = ip->itemdata.proj;
	      ip->itemdata.proj = NULL;
	      ip->page = ip->itemdata.zpag;
	    }
	  else
	    fprintf(stderr, "oops\n");
	}
      
      if (isp_glos_menu(ip))
	return 1;
      
      char pbuf[strlen(ip->cache.sub)+strlen(ip->glosdata.lbase)+strlen(ip->page)+strlen("z-p.div0")];
      sprintf(pbuf, "%s/z%s-p%s.div", ip->cache.sub, ip->glosdata.lbase, ip->page);
      ip->cache.page = (ccp)pool_copy((uccp)pbuf, ip->p);

      ip->cache.pgin = (ccp)pool_copy((uccp)ip->cache.page, ip->p);
      char *tmp = strrchr(ip->cache.pgin, '.');
      strcpy(tmp, ".pag");
      
      if (ip->item || ip->glosdata.ent)
	{
	  if (isp_glos_item(ip))
	    return 1;
	}
      else
	{
	  if (isp_glos_data(ip))
	    return 1;
	}
    }
  else
    {
      const char *zout = ((ip->item && ip->cache.t_sort) ? ip->cache.t_sort : ip->cache.out);
      char zbuf[strlen(zout)+strlen(ip->zoom)+strlen("-123-z.otl0")];
      sprintf(zbuf, "%s/%s-z%s.otl", zout, ip->perm, ip->zoom);
      ip->cache.zout = (ccp)pool_copy((uccp)zbuf, ip->p);

      char pbuf[strlen(ip->cache.out)+strlen(ip->zoom)+strlen(ip->page)+strlen("-123-z-p.div0")];
      sprintf(pbuf, "%s/%s-z%s-p%s.div", ip->cache.out, ip->perm, ip->zoom, ip->page);
      ip->cache.page = (ccp)pool_copy((uccp)pbuf, ip->p);

      if (ip->verbose)
	fprintf(stderr, "isp: isp_cache_page: need %s:::%s\n", zbuf, pbuf);

      if (iso_page_zoom(ip))
	return 1;

      if (isp_page_data(ip))
	return 1;
    }
  return 0;
}
