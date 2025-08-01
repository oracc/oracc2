#include <oraccsys.h>
#include "px.h"

static Isp *opt_ip;
static const char *skip_bio(const char *);

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
    ret = options(argc, argv, "03EZPC:FG:Odf:j:l:r:R:m:a:z:p:Q:q:g:e:i:b:B:s:S:k:Hh:x:u:c:L:a:t:vwW:");

  if (opt_ip->itemdata.block)
    {
      if (opt_ip->itemdata.file)
	{
	  opt_ip->what = "file";
	  opt_ip->form = "frag";
	}
      else
	{
	  opt_ip->what = "score";
	  opt_ip->form = "block";
	}
    }

  opt_ip = NULL;

  if (ip->err)
    ip->err = fatalize(ip->err);
  
  if (!ret && argv[optind])
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
    case 'B':
      opt_ip->itemdata.block = arg;
      break;
    case 'C':
      opt_ip->cache.sys = arg;
      break;
    case 'P':
      opt_ip->cache.pub = "02pub";
      opt_ip->pub_output = 1;
      break;
    case 'F':
      opt_ip->force = 1;
      break;
    case 'f':
      opt_ip->form = arg;
      break;
    case 'G':
      opt_ip->itemdata.file = arg;
      break;
    case 'g':
      if (arg && strlen(arg))
	opt_ip->glos = arg;
      break;
    case 'W':
      opt_ip->what = arg;
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
      opt_ip->list_name = "list";
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
	if ((slash && 's' == slash[1] && '.' == slash[2]))	  
	  {
	    opt_ip->srchdata.list = slash+1;
	    opt_ip->cache.sub = opt_ip->tmp_dir = opt_ip->srchdata.tmp = arg;
	    opt_ip->cache.sys = (ccp)pool_copy((ucp)arg,opt_ip->p);
	    char *isd = strstr(opt_ip->cache.sys, "/p4.d/");
	    isd += 5;
	    *isd = '\0';
	    opt_ip->list_name = "list";
	  }
	else if ('s' == arg[0] && '.' == arg[1])
	  {
	    opt_ip->tmp_dir = opt_ip->srchdata.tmp = arg;
	    opt_ip->list_name = "list";
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
      /*opt_ip->item_replace = arg;*/ /* ignore because this is handled differently in P4 */
      break;
    case 'i':
      opt_ip->item = arg;
      break;
    case 'Q':
      /* part option */
      break;
    case 'p':
      opt_ip->page = arg;
      break;
    case 's':
      opt_ip->psiz = arg;
      break;
    case 'S':
      opt_ip->sig = skip_bio(arg);
      opt_ip->what = "sig";
      break;
    case 'c':
      opt_ip->cemd = arg;
      break;
    case 'L':
      opt_ip->lang = arg;
      break;
    case 'k':
      opt_ip->pack = arg;
      break;
    case 'H':
      opt_ip->host = arg;
      break;
    case 'h':
      opt_ip->itemdata.hili = arg;
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
    case 'v':
      ++opt_ip->verbose;
      break;
    case 'o':
      opt_ip->ood = 1;
      opt_ip->itemdata.type = "ood";
      break;
    case '0':
      ++opt_ip->noheader;
      break;
    default:
      opt_ip->err = "unknown option";
      return 1;
    }
  return 0;
}

static const char *
skip_bio(const char *s)
{
  const unsigned char *u = (uccp)s;
  unsigned int bio[] = { 0xE2 , 0x98 , 0xA3 };
  
  if (bio[0] == u[0] && bio[1] == u[1] && bio[2] == u[2])
    return s + 3;
  else
    return s;
}
