#include <oraccsys.h>
#include <form.h>
#include <lng.h>
#include <xcl.h>

FILE *f_log;
const char *file;
const char *project;
int lnum, verbose, fuzzy_aliasing, lem_autolem, lem_dynalem, bootstrap_mode;
int neo_mode = 0, oid_mode = 1;

Hash *neo = NULL;
Hash *sig = NULL;
Pool *poo = NULL;

static void
neo_proxies(void)
{
  int nlines;
  unsigned char *fmem;
  unsigned char **lpp = loadfile_lines3("00lib/proxy.lst", &nlines, &fmem);
  if (lpp)
    {
      neo = hash_create(nlines/100);
      int i;
      for (i = 0; i < nlines; ++i)
	hash_add(neo, lpp[i], "");
    }
  else
    neo = hash_create(1);
}

static void
sig_inst(List *lp, unsigned char *ip)
{
  unsigned char *s = ip;
  while (*s)
    {
      unsigned char *e = s, *dot = NULL, save;
      while (*e && !isspace(*e))
	{
	  if ('.' == *e && !dot)
	    dot = e;
	  ++e;
	  if (*e)
	    *e++ = '\0';
	}
      if (dot)
	{
	  save = '.';
	  *dot = '\0';
	}
      if (hash_find(neo, s))
	{
	  if (save)
	    *dot = '.';
	  list_add(lp, s);
	}
      s = e;
      while (isspace(*s)) /* can't happen ... */
	++s;
    }
}

int
main(int argc, char **argv)
{
  unsigned char *lp;
  Form f;
  int ln = 0;
  ccp lang;
  struct xcl_context *xcp = NULL;

  options(argc, argv, "no");

  if (neo_mode)
    {
      xcp = xcl_create();
      xcp->project = project = "neo";
      neo_proxies();
      sig = hash_create(8192);
      poo = pool_init();
    }
  
  mesg_init();
  form_init();
  lp = loadoneline(stdin, NULL);
  if (strncmp(lp, "@fields", strlen("@fields")))
    {
      fprintf(stderr, "sigx: first line of .sig file should start with @fields\n");
      exit(1);
    }
  while ((lp = loadoneline(stdin, NULL)))
    {
      const char *orig_lp = strdup(lp);
      char *inst = strchr(lp, '\t');
      if (inst)
	*inst++ = '\0';
      memset(&f, '\0', sizeof(Form));
      if (!form_parse((uccp)"<stdin>", ++ln, lp, &f, NULL))
	{
	  if (oid_mode)
	    {
	      if (f.pos && isupper(f.pos[0]) && 'N' == f.pos[1])
		lang = "qpn";
	      else
		lang = f.core->name;
	      printf("%s\t%s[%s]%s\tword\n", lang, f.cf, f.gw, f.pos);
	      printf("%s\t%s[%s//%s]%s'%s\tsense\t%s[%s]%s\tword\n",
		     lang, f.cf, f.gw, f.sense, f.pos, f.epos, f.cf, f.gw, f.pos);
	    }
	  else if (neo_mode)
	    {
	      f.project = (uccp)"neo";
	      if (f.core)
		{
		  f.lang = (uccp)f.core->name;
		  f.morph2 = NULL;
		  unsigned char *sigp = form_sig_nopool(xcp, &f);
		  List *sl = hash_find(sig, sigp);
		  if (!sl)
		    {
		      hash_add(sig, pool_copy(sigp, poo), (sl = list_create(LIST_SINGLE)));
		      free(sigp);
		    }
		  if (inst)
		    sig_inst(inst, sl);
		}
	      else
		{
		  fprintf(stderr, "sigx: no f.core\n");
		}
	    }
	}
      else
	{
	  mesg_print(stderr);
	  mesg_init();
	}
      free(orig_lp);
    }

  if (neo_mode)
    {
      int nk;
      unsigned char **k = hash_keys2(sig, &nk);
      qsort(k, nk, sizeof(unsigned char *), cmpstringp);
      int i;
      for (i = 0; i < nk; ++i)
	{
	  printf("%s\t", k[i]);
	  List *lp = hash_find(sig, k[i]);
	  if (lp)
	    {
	      unsigned char *ip;
	      int sp = 0;
	      for (ip = list_first(lp); ip; ip = list_next(lp))
		{
		  if (sp)
		    fputc(' ', stdout);
		  else
		    ++sp;
		  fputs(ip, stdout);
		}
	    }
	  fputc('\n', stdout);
	}
    }
}
int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'n':
      neo_mode = 1;
      oid_mode = 0;
      break;
    case 'o':
      neo_mode = 0;
      oid_mode = 1;
      break;
    default:
      fprintf(stderr, "sigx: unknown option -%c\n", opt);
      return 1;
    }
  return 0;
}

void help(void) { }
