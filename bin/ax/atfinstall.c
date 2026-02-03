#include <oraccsys.h>

const char *list_fn;
char *o_dir;
const char *project;
const char *efile;
size_t eline;
int error_mode = 0, verbose = 0;

#define PQX_MAX 7

#define BOM2(str)	(((uccp)(str))[0]==0xfe && ((uccp)(str))[1]==0xff)
#define BOM3(str)	(((uccp)(str))[0]==0xef && ((uccp)(str))[1]==0xbb && ((uccp)(str))[1]==0xbf)

static void
errormode(void)
{
  if (!error_mode)
    {
      error_mode = 1;
      fprintf(stderr, "atfinstall: entering error mode, ATF install will be incomplete\n");
    }
}

static FILE *
listfile(void)
{
  char buf[strlen(oracc())+strlen(project)+strlen("//01bld/lists/atfinstall.lst0")];
  sprintf(buf, "%s/%s/01bld/lists/atfinstall.lst", oracc(), project);
  list_fn = strdup(buf);
  return xfopen(buf, "w");
}

static int
o_dirs(const char *o_fn)
{
  char dirs[strlen(o_fn)], *f;
  strcpy(dirs, o_fn);
  f = strrchr(dirs, '/');
  if (f)
    {
      *f = '\0';
      o_dir = strdup(dirs);
      return xmkdirs(dirs);
    }
  else
    {
      fprintf(stderr, "%s:%ld: malformed path '%s' has no '/'\n", efile, eline, dirs);
      errormode();
      return 1;
    }
}

const char *
pqx_of(unsigned char *l)
{
  static char pqx[PQX_MAX+1], *ret = NULL;
  int i;
  if ('P' == *l || 'Q' == *l || 'X' == *l)
    {
      *pqx = *l;
      for (i = 1; i < PQX_MAX; ++i)
	{
	  if (isdigit(l[i]))
	    pqx[i] = l[i];
	  else
	    {
	      pqx[i] = '\0';
	      fprintf(stderr, "%s:%ld: malformed PQX--expected digit after %s\n", efile, eline, pqx);
	      break;
	    }
	}
      pqx[i] = '\0';
      if (l[i] && !isspace(l[i]) && '=' != l[i])
	fprintf(stderr, "%s:%ld: malformed PQX--junk after %s\n", efile, eline, pqx);
      else
	ret = pqx;
    }
  else
    fprintf(stderr, "%s:%ld: malformed PQX--no initial P or Q or X\n", efile, eline);
  return ret;
}

int
main(int argc, char *const *argv)
{
  FILE *o = NULL;
  char *o_fn = NULL;
  options(argc, argv, "p:v");
  if (!project)
    {
      fprintf(stderr, "%s: must give project on command line. Stop.\n", argv[0]);
      exit(1);
    }

  FILE *list_fp = listfile();
  if (!list_fp)
    return error_mode;

  while (argv[optind])
    {
      FILE *a = xfopen(argv[optind], "r");
      efile = argv[optind];
      eline = 0;
      size_t n;
      unsigned char *l;
      while ((l = loadoneline(a, &n)))
	{
	  ++eline;
	  if (BOM2(l))
	    {
	      l += 2;
	      n -= 2;
	    }
	  else if (BOM3(l))
	    {
	      l += 3;
	      n -= 3;
	    }

	  if ('&' == *l)
	    {
	      if (o)
		{
		  xfclose(o_fn, o);
		  o_fn = NULL;
		  o = NULL;
		}
	      const char *pqx = pqx_of(l+1);
	      if (pqx)
		{
		  /* expand uses a static buffer so as long as we are
		     only expanding one filename at a time we don't
		     need to strdup/free */
		  o_fn = expand(project, pqx, "atf");
		  if (!o_dirs(o_fn))
		    {
		      o = xfopen(o_fn, "w");
		      if (!o)
			errormode();
		      else
			{
			  if (verbose)
			    fprintf(stderr, "atfinstall: writing to %s\n", o_fn);
			  fputs(o_dir, list_fp);
			  fputc('\n', list_fp);
			  free(o_dir);
			  fprintf(o, "##file %s\n##line %ld\n", efile, eline);
			}
		    }
		  else
		    errormode();
		}
	      else
		errormode();
	    }

	  if (o)
	    {
	      /* remove \r and ensure all lines end in \n */
	      while (n && ('\n' == l[n-1] || '\r' == l[n-1]))
		--n;
	      l[n] = '\0';

	      fputs((ccp)l, o);
	      fputc('\n', o);
	    }
	}
      xfclose(argv[optind++], a);
      if (o)
	xfclose(o_fn, o);
    }
  if (xfclose(list_fn, list_fp))
    error_mode = 1;

  exit(error_mode);
}


void help(void) { fprintf(stderr, "atfinstall: -p [PROJECT] [ATF-FILES]\n"); }
int
opts(int c, const char *arg)
{
  switch (c)
    {
    case 'p':
      project = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    }
  return 0;
}
