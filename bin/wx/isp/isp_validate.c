#include <oraccsys.h>
#include "isp.h"

static int
isp_integer(unsigned const char *p)
{
  while (*p && *p < 128 && isdigit(*p))
    ++p;
  return *p;
}

static int
isp_valid_arg(const char *p, int step)
{
  if (p)
    {
      struct ispargstab *ipt = ispargs(p);
      if (!ipt || ipt->step != step)
	return 1;
    }
  return 0;
}

static int
isp_valid_project(struct isp *ip)
{
  char dir[strlen(ip->oracc)+strlen(ip->project)+2];
  sprintf(dir, "%s/%s", ip->oracc, ip->project);
  struct stat sb;
  if (stat(dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      ip->err = "unknown project";
      return 1;
    }
  return 0;
}

static int
isp_valid_host(struct isp *ip)
{
  char hostpath[strlen(ip->oracc)+strlen(ip->project)+strlen("02pub")+strlen(ip->host)+3];
  sprintf(hostpath, "%s/%s/02pub/%s", ip->oracc, ip->project, ip->host);
  if (access(hostpath, R_OK))
    {
      ip->err = "host file non-existent or unreadable";
      return 1;
    }
  else
    ip->host_path = pool_copy((uccp)hostpath, ip->pool);
  return 0;
}

int
isp_validate(struct isp *ip)
{
  if (!ip->oracc)
    {
      ip->err = "ORACC_BUILDS not set";
      goto error;
    }

  if (!ip->project)
    {
      ip->err = "PROJECT not set";
      goto error;
    }
  else if (!isp_valid_project(ip))
    goto error;

  if (!isp_integer((uccp)ip->zoom))
    {
      ip->err = "zoom parameter is not a positive integer";
      goto error;
    }

  if (!isp_integer((uccp)ip->page))
    {
      ip->err = "page parameter is not a positive integer";
      goto error;
    }

  if (!isp_integer((uccp)ip->size))
    {
      ip->err = "size parameter is not a positive integer";
      goto error;
    }

  if (!isp_valid_arg(ip->cemd, ISP_STEP_6C))
    {
      ip->err = "argument for -c must be kwic|line|unit";
      goto error;
    }

  if (ip->lang)
    {
      unsigned const char *l = (uccp)ip->lang;
      while (*l && *l < 128 && isalpha(*l) && l = ip->lang < 4)
	++l;
      if (*l)
	{
	  ip->err = "lang given with -l must be two or three letter language code";
	  goto error;
	}
    }

  if (!isp_valid_arg(ip->xhmd, ISP_STEP_7F))
    {
      ip->err = "argument for -m must be oxml|html";
      goto error;
    }

  if (!isp_valid_arg(ip->uimd, ISP_STEP_9U))
    {
      ip->err = "argument for -u must be mini|maxi";
      goto error;
    }

  if (!isp_valid_arg(ip->pack, ISP_STEP_8O))
    {
      ip->err = "argument for -k must be asis|esp2";
      goto error;
    }

  if (ip->host && !isp_valid_host(ip))
    goto error;
  
  if (!isp_valid_arg(ip->aapi, ISP_STEP_9A))
    {
      ip->err = "argument for -a must be file|rest";
      goto error;
    }

  return 0;

 error:
  return 1;
}
