#include <oraccsys.h>
#include <sll.h>
#include <tok.h>

/* P010590.13.1 P230105.12.1 */

/* Turn a list of sign-sigs into a signlist; sign sigs are
   SIGN-FORM-VALUE where FORM and VALUE can be empty */

Pool *p;

const char *infile = "<stdin>";
const char *mergers;
const char *outfile;
const char *project;
const char *signlist;
const char *sl = "sl"; /* -c for "pc" -e for "pe" -l for "sl" */
FILE *i, *o;
int stdinput = 0;

Hash *h_merger;

void
load_mergers(void)
{
  unsigned char *fmem, **lp;
  size_t nline;
  h_merger = hash_create(256);
  lp = loadfile_lines3((uccp)mergers, &nline, &fmem);
  int i;
  for (i = 0; i < nline; ++i)
    {
      unsigned char *v = lp[i];
      while (*v && !isspace(*v))
	++v;
      if (*v)
	{
	  *v++ = '\0';
	  while (isspace(*v))
	    ++v;
	  if (*v)
	    hash_add(h_merger, lp[i], v);
	}
    }
}

const char *
gsig_id(void)
{
  static int id = 0;
  char s[32];
  sprintf(s, "0x%05x", id++);
  return (ccp)pool_copy((ucp)s, p);
}

void
asl_input(const char *argfile)
{
  if (argfile)
    {
      if (!(i = fopen(argfile, "r")))
	{
	  perror(argfile);
	  exit(1);
	}
      infile = argfile;
    }
  else
    i = stdin;
}

void
asl_output(const char *outfile)
{
  if (outfile)
    {
      if (!(o = fopen(outfile, "w")))
	{
	  perror(outfile);
	  exit(1);
	}
    }
  else
    o = stdout;
}

int
main(int argc, char **argv)
{
  Hash *h = hash_create(1024);
  Hash *hform = hash_create(1024);
  char *lp;
  p = pool_init();
  size_t lnum = 0;

  if (options(argc, argv, "celm:o:P:S:s"))
    exit(1);

  asl_input(argv[optind]);
  asl_output(outfile);

  if (mergers)
    load_mergers();
  
  if (project)
    fprintf(o, "@project %s\n@signlist corpus\n\n", project);

  while ((lp = (char*)loadoneline(i, NULL)))
    {
      ++lnum;
      if ('g' == *lp)
	{
	  char **ff = tab_split(lp);
	  Gsig *gp = gsig_parse_alloc(NULL);
	  gp = gsig_parse(ff[2], gp, NULL);
	  if (gp)
	    {
	      if (!strcmp(sl, gp->asltype))
		{
		  const char *id = gsig_id();
		  hash_add(hform, (uccp)id, pool_copy((ucp)gp->form,p));
		  List *fl = hash_find(hform, (ucp)gp->coresig);
		  if (!fl)
		    {
		      fl = list_create(LIST_SINGLE);
		      list_add(fl, (void*)id);
		      hash_add(hform, pool_copy((ucp)gp->coresig, p), (void*)fl);
		    }
		  if (*gp->sname)
		    {
		      Hash *hs, *hf;

		      if (!(hs = hash_find(h, (ucp)gp->sname)))
			hash_add(h, pool_copy((ucp)gp->sname,p), hs = hash_create(32));

		      if (*gp->fname || *gp->value)
			{
			  const char *ffs = (*gp->fname ? gp->fname : "-");

			  if (!(hf = hash_find(hs, (ucp)ffs)))
			    hash_add(hs, pool_copy((ucp)ffs,p), hf = hash_create(32));
		      
			  if (*gp->value && !hash_find(hf, (ucp)gp->value))
			    hash_add(hf, pool_copy((ucp)gp->value, p), "");
			}
		    }
		}
	    }
	  else
	    {
	      fprintf(stderr, "%s:%ld: %s\n", infile, lnum, gsig_parse_last_error());
	    }
	}
      else if ('P' == *lp && !project)
	{
	  char *proj = strchr(lp, '\t');
	  if (proj && proj[1])
	    {
	      ++proj;
	      fprintf(o, "@project %s\n@signlist corpus\n\n", proj);
	    }
	  else
	    {
	      fprintf(stderr,
		      "tok2asl: .tok file has no project in P line; use -p [PROJECT]. Stop.\n");
		      exit(1);
	    }
	}
    }

  int i;
  char **sk = (char**)hash_keys(h);

  Hash *conly = hash_create(128);
  if (!strcmp(sl, "sl"))
    {
      (void)sll_init_t("ogsl", "sl");
      for (i = 0; sk[i]; ++i)
	{
	  const char *o = (ccp)sll_lookup((uccp)sk[i]);
	  if (o)
	    {
	      const char *d = (ccp)sll_lookup(sll_tmp_key((uccp)o, "d"));
	      if (d)
		{
		  const char **dd = sll_deep_oids(d);
		  int j;
		  for (j = 0; dd[j]; ++j)
		    {
		      const char *n = (ccp)sll_lookup((uccp)dd[j]);
		      if (n)
			hash_add(conly, (ucp)n, "");
		    }
		  free((void*)dd[0]);
		  free(dd);
		}
	    }
	}
    }

  for (i = 0; sk[i]; ++i)
    {
      fprintf(o, "@sign %s\n", sk[i]);
      Hash *f = hash_find(h, (ucp)sk[i]);
      Hash *v = hash_find(f, (ucp)"-");
      if (v)
	{
	  char **vk = (char **)hash_keys(v);
	  int v;
	  for (v = 0; vk[v]; ++v)
	    fprintf(o, "@v %s\n", vk[v]);
	}
      char **fk = (char**)hash_keys(f);
      int j;
      for (j = 0; fk[j]; ++j)
	if (strcmp(fk[j], "-"))
	  {
	    fprintf(o, "@form %s\n", fk[j]);
	    Hash *v = hash_find(f, (ucp)fk[j]);
	    if (v)
	      {
		char **vk = (char**)hash_keys(v);
		int v;
		for (v = 0; vk[v]; ++v)
		  fprintf(o, "@v %s\n", vk[v]);
	      }
	    fprintf(o, "@@\n");
	  }
      const char *m = hash_find(h_merger, (uccp)sk[i]);
      if (m)
	fprintf(o, "@merge %s\n", m);
      fprintf(o, "@end sign\n\n");
    }

  const char **ck = hash_keys(conly);
  for (i = 0; ck[i]; ++i)
    fprintf(o, "@compoundonly %s\n", ck[i]);
}

const char *prog = "tok2asl";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "tok2asl";
void help () { }
int opts(int arg, const char*str)
{
  switch (arg)
    {
    case 'c':
      sl = "pc";
      break;
    case 'e':
      sl = "pe";
      break;
    case 'l':
      sl = "sl";
      break;
    case 'm':
      mergers = str;
      break;
    case 'o':
      outfile = str;
      break;
    case 'P':
      project = str;
      break;
    case 'S':
      signlist = str;
      break;
    case 's':
      stdinput = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
