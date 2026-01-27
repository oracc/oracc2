#include <oraccsys.h>
#include <form.h>
#include <lng.h>
#include <xcl.h>

FILE *f_log;
const char *file;
const char *project;
int lnum, verbose, fuzzy_aliasing, lem_autolem, lem_dynalem, bootstrap_mode;
int bare_input = 0, neo_mode = 0, oid_mode = 1;

Hash *neo = NULL;
Hash *sig = NULL;
Pool *poo = NULL;

int
cmpinstidp(const void *p1, const void *p2)
{
  char *pq1 = strchr(* (char * const *) p1, ':') + 1;
  char *pq2 = strchr(* (char * const *) p2, ':') + 1;
  int ret;

  char *d1 = strchr(pq1, '.');
  char *d2 = strchr(pq1, '.');
  if (!d1 || !d2)
    return d1 ? 1 : 0;
  *d1 = *d2 = '\0';

  ret = strcmp(pq1,pq2);
  *d1 = *d2 = '.';
  if (ret)
      return ret;

  /* compare line id */
  int n1 = atoi(d1+1);
  int n2 = atoi(d2+1);
  if (n1 != n2)
    return n1 - n2;

  d1 = strchr(d1+1, '.');
  d2 = strchr(d2+1, '.');
  if (!d1 || !d2)
    return d1 ? 1 : 0;

  /* compare word id */
  n1 = atoi(d1+1);
  n2 = atoi(d2+1);
  if (n1 != n2)
    return n1 - n2;

  d1 = strchr(d1+1, '.');
  d2 = strchr(d2+1, '.');
  if (!d1 || !d2)
    return d1 ? 1 : 0;

  /* compare grapheme id */
  n1 = atoi(d1+1);
  n2 = atoi(d2+1);
  if (n1 != n2)
    return n1 - n2;

  return 0;
}

static void
neo_proxies(void)
{
  size_t nlines;
  unsigned char *fmem;
  unsigned char **lpp = loadfile_lines3((uccp)"00lib/proxy.lst", &nlines, &fmem);
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
	    {
	      dot = e;
	      break;
	    }
	  ++e;
	}
      if (dot)
	{
	  save = '.';
	  *dot = '\0';
	}
      else
	*e++ = '\0';
      if (hash_find(neo, s))
	{
	  if (save)
	    *dot = '.';
	  list_add(lp, s);
	}
      s = e;
      while (*s && !isspace(*s))
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

  options(argc, argv, "bno");

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
  if (!bare_input)
    {
      lp = loadoneline(stdin, NULL);
      if (strncmp((ccp)lp, "@fields", strlen("@fields")))
	{
	  fprintf(stderr, "sigx: first line of .sig file should start with @fields\n");
	  exit(1);
	}
    }
  while ((lp = loadoneline(stdin, NULL)))
    {
      lp = (ucp)strdup((ccp)lp);
      char *orig_lp = strdup((ccp)lp);
      char *inst = strchr((ccp)lp, '\t');
      if (inst)
	*inst++ = '\0';
      memset(&f, '\0', sizeof(Form));
      if (-1 != form_parse((uccp)"<stdin>", ++ln, lp, &f, NULL))
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
		  unsigned char *sigp = form_sig_sub(poo, &f);
		  if (sigp)
		    {
		      List *sl = hash_find(sig, sigp);
		      if (!sl)
			{
			  hash_add(sig, pool_copy(sigp, poo), (sl = list_create(LIST_SINGLE)));
			  free(sigp);
			}
		      if (inst)
			sig_inst(sl, (ucp)inst);
		    }
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
      printf("@fields sig rank freq inst\n");
      int nk;
      const char **k = hash_keys2(sig, &nk);
      qsort(k, nk, sizeof(const char *), cmpstringp);
      int i;
      for (i = 0; i < nk; ++i)
	{
	  printf("%s\t", k[i]);
	  List *lp = hash_find(sig, (uccp)k[i]);
	  if (lp && list_len(lp))
	    {
	      int n;
	      const char **ll = (const char **)list2array_c(lp, &n);
	      qsort(ll, n, sizeof(const char *), cmpinstidp);
	      printf("0\t%d\t", n);
	      int j;
	      for (j = 0; j < n; ++j)
		{
		  if (j)
		    fputc(' ', stdout);
		  fputs(ll[j], stdout);
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
    case 'b':
      bare_input = 1;
      break;
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
