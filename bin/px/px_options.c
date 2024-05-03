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
	*equals++ = '\0';
      struct cgioptstab *t = cgiopts(opt, strlen(opt));
      if (!t || opts(t->opt,equals))
	{
	  ip->err = PX_ERROR_START "invalid option: %s\n";
	  ip->errx = argv[argc];
	  return 1;
	}
    }
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
    ret = options(argc, argv, "3ELSZPWCFOdfj:l:r:m:z:p:g:i:b:s:k:h:x:u:c:l:a:vw");
  opt_ip = NULL;
  if (ret && !ip->err)
    ip->err = "processing options";
  return ret;
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
#if 0
    case '3':
      opt_ip->p3 = 1;
      break;
#endif
    case 'E':
      opt_ip->steps[PX_STEP_0E] = 1;
      break;
    case 'L':
      opt_ip->steps[PX_STEP_1L] = 1;
      break;
    case 'S':
      opt_ip->steps[PX_STEP_2S] = 1;
      break;
    case 'Z':
      opt_ip->steps[PX_STEP_3Z] = 1;
      break;
    case 'P':
      opt_ip->steps[PX_STEP_4P] = 1;
      break;
    case 'W':
      opt_ip->steps[PX_STEP_5W] = 1;
      break;
    case 'C':
      opt_ip->steps[PX_STEP_6C] = 1;
      break;
    case 'F':
      opt_ip->steps[PX_STEP_7F] = 1;
      break;
    case 'O':
      opt_ip->steps[PX_STEP_8O] = 1;
      break;
    case 'b':
      opt_ip->bkmk = arg;
      break;
    case 'f':
      opt_ip->force = 1;
      break;
    case 'g':
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
    case 'z':
      opt_ip->zoom = arg;
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
      /* possibly add 'o' for output selection, XML vs HMTL, or is this sufficiently covered by -x? */
    case 'a':
      opt_ip->aapi = arg;
      break;
    case 'x':
      opt_ip->xhmd = arg;
      break;
    case 'u':
      opt_ip->uimd = arg;
      break;
    case 'd':
      ++opt_ip->debug;
      break;
    case 'v':
      ++opt_ip->verbose;
      break;
    default:
      opt_ip->err = "unknown option";
      return 1;
    }
  return 0;
}
