#include <oraccsys.h>
#include <xpd.h>
#include "px.h"

static int
px_integer(unsigned const char *p)
{
  while (*p && *p < 128 && isdigit(*p))
    ++p;
  return *p;
}

static int
px_valid_arg(const char *p, int step)
{
  if (p)
    {
      struct ispargstab *ipt = pxargs(p, strlen(p));
      if (!ipt || ipt->step != step)
	return 1;
    }
  return 0;
}

/* This routine loads the project's config.xml as a side-effect */
static int
px_valid_project(Isp *ip)
{
  char dir[strlen(ip->oracc)+strlen(ip->project)+2];
  sprintf(dir, "%s/%s", ip->oracc, ip->project);
  struct stat sb;
  if (stat(dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      ip->err = "unknown project";
      return 1;
    }
  else
    {
      ip->projdir = (ccp)pool_copy((ucp)dir, ip->p);
      struct xpd *xp = xpd_init(ip->project, ip->p);
      if (0 == xp->opts->key_count)
	{
	  ip->err = "failed to load project config.xml";
	  return 1;
	}
      else
	ip->xpd = xp;
    }
  return 0;
}

static int
px_valid_glos(Isp *ip)
{
  char dir[strlen(ip->projdir)+strlen(ip->glos)+strlen("/02pub/cbd/0")];
  sprintf(dir, "%s/02pub/cbd/%s", ip->projdir, ip->glos);
  struct stat sb;
  if (stat(dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      ip->err = "unknown lang while validating glos";
      return 1;
    }
  else
    {
      if (ip->zoom && !strcmp(ip->zoom, "0"))
	ip->zoom = NULL;
      ip->glosdata.dir = (ccp)pool_copy((ucp)dir, ip->p);
      if (ip->glosdata.xis)
	{
	  if (!ip->zoom || isalpha(*ip->zoom))
	    ip->zoom = "0";
	}
    }
  return 0;  
}

static int
px_valid_host(Isp *ip)
{
  char hostpath[strlen(ip->oracc)+strlen(ip->project)+strlen("02pub")+strlen(ip->host)+3];
  sprintf(hostpath, "%s/%s/02pub/%s", ip->oracc, ip->project, ip->host);
  if (access(hostpath, R_OK))
    {
      ip->err = "host file non-existent or unreadable";
      return 1;
    }
  else
    ip->host_path = (ccp)pool_copy((uccp)hostpath, ip->p);
  return 0;
}

int
px_validate(Isp *ip)
{
  if (!ip->oracc)
    {
      ip->err = "ORACC_BUILDS not set in environment";
      goto error;
    }

  if (!ip->project)
    {
      ip->err = "PROJECT not set, use -j PROJECT";
      goto error;
    }
  else if (px_valid_project(ip))
    goto error;

  if (ip->glos)
    {
      if (px_valid_glos(ip))
	goto error;
      ip->list_name = NULL; /* undefine the default outlined.lst */
    }
  
  if (!ip->list_name)
    {
      if (ip->glos)
	{
	  ip->list_name = (ccp)pool_alloc(strlen(ip->glos)+5, ip->p);
	  sprintf((char*)ip->list_name, "%s.glo", ip->glos);
	}
      else
	{
	  ip->err = "LIST not set, use -l LIST";
	  goto error;
	}
    }

  if (ip->zoom && !ip->glos && px_integer((uccp)ip->zoom))
    {
      ip->err = "zoom parameter is not a positive integer";
      goto error;
    }

  if (ip->page && px_integer((uccp)ip->page))
    {
      ip->err = "page parameter is not a positive integer";
      goto error;
    }

  if (strcmp(ip->psiz,"25") && strcmp(ip->psiz,"50") && strcmp(ip->psiz, "100"))
    {
      ip->err = "psiz must be 25, 50, or 100";
      goto error;
    }

  if (px_valid_arg(ip->cemd, PX_STEP_6C))
    {
      ip->err = "argument for -c must be kwic|line|unit";
      goto error;
    }

  if (ip->lang)
    {
      const char *l = ip->lang;
      while (*l && *l > 0 && isalpha(*l) && (l - ip->lang) < 4)
	++l;
      if (*l)
	{
	  ip->err = "lang given with -l must be two or three letter language code";
	  goto error;
	}
    }

  if (px_valid_arg(ip->xhmd, PX_STEP_7F))
    {
      ip->err = "argument for -m must be oxml|html";
      goto error;
    }

  if (px_valid_arg(ip->uimd, PX_PARM_9U))
    {
      ip->err = "argument for -u must be mini|maxi";
      goto error;
    }

  if (px_valid_arg(ip->pack, PX_STEP_8O))
    {
      ip->err = "argument for -k must be asis|esp2";
      goto error;
    }

  if (ip->host && px_valid_host(ip))
    goto error;
  
  if (px_valid_arg(ip->aapi, PX_PARM_9A))
    {
      ip->err = "argument for -a must be file|rest";
      goto error;
    }

  return 0;

 error:
  return 1;
}
