#include <oraccsys.h>
#include "isp.h"

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
isp_options(int argc, char **argv, Isp *ip)
{
  int ret;
  opt_ip = ip;
  ret = options(argc, argv, "3ELSZPWCFOfj:l:r:m:z:p:s:k:h:x:u:c:l:a:vw");
  opt_ip = NULL;
  if (ret)
    ip->err = "processing options";
  return ret;
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case '3':
      opt_ip->p3 = 1;
      break;
    case 'E':
      opt_ip->steps[ISP_STEP_0E] = 1;
      break;
    case 'L':
      opt_ip->steps[ISP_STEP_1L] = 1;
      break;
    case 'S':
      opt_ip->steps[ISP_STEP_2S] = 1;
      break;
    case 'Z':
      opt_ip->steps[ISP_STEP_3Z] = 1;
      break;
    case 'P':
      opt_ip->steps[ISP_STEP_4P] = 1;
      break;
    case 'W':
      opt_ip->steps[ISP_STEP_5W] = 1;
      break;
    case 'C':
      opt_ip->steps[ISP_STEP_6C] = 1;
      break;
    case 'F':
      opt_ip->steps[ISP_STEP_7F] = 1;
      break;
    case 'O':
      opt_ip->steps[ISP_STEP_8O] = 1;
      break;
    case 'f':
      opt_ip->force = 1;
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
    case 'a':
      opt_ip->aapi = arg;
      break;
    case 'u':
      opt_ip->uimd = arg;
      break;
    case 'v':
      ++opt_ip->verbose;
      break;
    case 'x':
      opt_ip->xhmd = arg;
      break;
    default:
      opt_ip->err = "unknown option";
      return 1;
    }
  return 0;
}
