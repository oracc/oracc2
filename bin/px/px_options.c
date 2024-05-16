#include <oraccsys.h>
#include "px.h"

static Isp *opt_ip;

/* This belongs in isp_sort or isp_perm */
int
isp_perm_check(const char *p)
{
  int one=0, two=0, thr=0;
  if (p)
    {
      if (strlen(p) < 4)
	{
	  while (*p)
	    {
	      switch (*p)
		{
		case '1':
		  if (one++)
		    goto error;
		  break;
		case '2':
		  if (two++)
		    goto error;
		  break;
		case '3':
		  if (thr++)
		    goto error;
		  break;
		default:
		  goto error;
		}
	      ++p;
	    }
	}
    }
  return 0;
 error:
  return 1;
}

int
cgi_options(int argc, char **argv, Isp *ip)
{
  for (argc = 1; argv[argc]; ++argc)
    {
      char *opt = (char*)pool_copy((ucp)argv[argc], ip->p);
      char *equals = strchr(opt, '=');
      if (equals)
	{
	  *equals++ = '\0';
	  equals = (char*)pool_copy((ucp)CGI_decode_url(equals), ip->p);
	}
      struct cgioptstab *t = cgiopts(opt, strlen(opt));
      if (!t || opts(t->opt,equals))
	{
	  ip->err = PX_ERROR_START "invalid option: %s\n";
	  ip->errx = argv[argc];
	  return 1;
	}
    }
  optind = argc;
  return 0;
}

int
px_options(int argc, char **argv, Isp *ip)
{
  int ret;
  ip->argc = argc;
  ip->argv = (const char **)argv;
  opt_ip = ip;

  if (argv[1] && '-' != argv[1][0])
    ret = cgi_options(argc, argv, ip);
  else
    ret = options(argc, argv, "3ELSZPWCFOdf:j:l:r:R:m:a:z:p:g:e:i:b:s:k:h:x:u:c:l:a:t:vw");
  opt_ip = NULL;
  if (argv[optind])
    {
      ret = 1;
      ip->err = PX_ERROR_START "fatal: junk after options";
    }
  if (ret && !ip->err)
    ip->err = PX_ERROR_START "fatal: processing options";
  return ret;
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'q':
      opt_ip->srch = arg; /* The user-input for the query; already whitelisted by wx */
      break;
    case 'b':
      opt_ip->bkmk = arg;
      break;
    case 'F':
      opt_ip->force = 1;
      break;
    case 'f':
      opt_ip->form = arg;
      break;
    case 'g':
      if (arg && strlen(arg))
	opt_ip->glos = arg;
      break;
    case 'w':
      opt_ip->web = 1;
      break;
    case 'j':
      opt_ip->project = arg;
      break;
    case 'l':
      opt_ip->list_name = arg;
      break;
    case 'x':
      opt_ip->glosdata.xis = arg;
      break;
    case 'm':
      if (strlen(arg) == 1 && ('0'==*arg || '1'==*arg))
	opt_ip->dors = arg;
      else
	{
	  opt_ip->err = "bad mode argument--must be 0 or 1";
	  return 1;
	}
      break;
    case 'r':
      if (isp_perm_check(arg))
	{
	  opt_ip->err = "bad perm argument--must be one to three digits '1', '2', or '3', no repeats";
	  return 1;
	}	
      else
	opt_ip->perm = arg;
      break;
    case 'R':
      opt_ip->referer = arg;
      break;
    case 't':
      {
	char *slash = strrchr(arg,'/');
	if (slash && 's' == slash[1] && '.' == slash[2])
	  {
	    opt_ip->list_name = (ccp)pool_copy((ucp)slash+1,opt_ip->p);
	    opt_ip->cache.sub = opt_ip->tmp_dir = opt_ip->srchdata.tmp = arg;
	    opt_ip->cache.sys = (ccp)pool_copy((ucp)arg,opt_ip->p);
	    char *isd = strstr(opt_ip->cache.sys, "/is.d/");
	    isd += 5;
	    *isd = '\0';
	  }
	else
	  opt_ip->tmpdir = arg;
      }
      break;
    case 'a':
      opt_ip->glosdata.let = arg;
      break;
    case 'z':
      opt_ip->zoom = arg;
      break;
    case 'e':
      opt_ip->glosdata.ent = arg;
      break;
    case 'I':
      opt_ip->item_replace = arg;
      break;
    case 'i':
      opt_ip->item = arg;
      break;
    case 'p':
      opt_ip->page = arg;
      break;
    case 's':
      opt_ip->psiz = arg;
      break;
    case 'c':
      opt_ip->cemd = arg;
      break;
    case 'n':
      opt_ip->lang = arg;
      break;
    case 'k':
      opt_ip->pack = arg;
      break;
    case 'h':
      opt_ip->host = arg;
      break;
      /* possibly add 'o' for output selection, XML vs HMTL, or is
	 this sufficiently covered by -x? 2024-05-16 -f(orm) for this */
    case 'A':
      opt_ip->aapi = arg;
      break;
    case 'X':
      opt_ip->xhmd = arg;
      break;
    case 'u':
      opt_ip->uimd = arg;
      break;
    case 'D':
      ++opt_ip->debug;
      break;
    case 'V':
      ++opt_ip->verbose;
      break;
    default:
      opt_ip->err = "unknown option";
      return 1;
    }
  return 0;
}
