#include <oraccsys.h>
#include <cbd.h>
#include <gdl.h>
#include <gt.h>
#include "gx.h"

/* Read all the 00lib *.glo files and output 02pub/lemm-*.sig files.
 *
 * Use existing 02pub/lemm-*.sig files to save instance info for sigs
 * that are already known.
 *
 * With -g arg create 01bld/LANG/glo.sig.
 *
 */

extern struct map *qpnnames(register const char *str, register size_t len);

FILE *f_xml;
int glo_mode = 0, quiet = 0;
int parser_status = 0;
int verbose = 1;
int bootstrap_mode, lem_autolem, lem_dynalem;
int out_stdout;
int rnvtrace, status;
Hash *hqlngs = NULL;
Mloc *xo_loc;
struct cbd* curr_cbd;
struct entry*curr_entry;
const char *file, *efile, *errmsg_fn;
extern void common_init(void);

int
lemsig_cmp(void *a, void *b)
{
  Lemsig *la = *(Lemsig**)a;
  Lemsig *lb = *(Lemsig**)b;
#if 0
  /* l2p1-lemm-data.plx which is replaced by glosigx did not sort by rank */
  if (la->rank != lb->rank)
    return lb->rank - la->rank;
#endif
  if (la->freq != lb->freq)
    return lb->freq - la->freq;
  return strcmp((ccp)la->sig, (ccp)lb->sig);
}

static char **
glo_files(void)
{
  static glob_t gres;
  if (!glob_pattern("00lib/*.glo",&gres))
    return gres.gl_pathv;
  else
    return NULL;
}

static char **
lem_files(void)
{
  static glob_t gres;
  if (!glob_pattern("02pub/lemm-*.sig",&gres))
    return gres.gl_pathv;
  else
    return NULL;
}

/* Iterate over the entry content creating sigs for any possible form.
 *
 * (Bases that do not occur as @forms have already been added to the entry's forms.)
 */
static void
cbd_entry_sigs(Entry *ep)
{
  Cform f;
  memset(&f, '\0', sizeof(Cform));

  f.f.project = ep->owner->project;
  f.f.lang = ep->owner->lang;
  f.f.cf = ep->cgp->cf;
  f.f.gw = ep->cgp->gw;
  f.f.pos = ep->cgp->pos;

  Sense *sp;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    {
      f.f.sense = sp->mng;
      f.f.epos = sp->pos;

      Cform *fp;
      for (fp = list_first(ep->forms); fp; fp = list_next(ep->forms))
	{
	  if (BIT_ISSET(fp->f.flags, FORM_FLAGS_COF_TAIL))
	    continue;

	  f.f.base = fp->f.base;
	  f.f.stem = fp->f.stem;
	  f.f.cont = fp->f.cont;
	  f.f.norm = fp->f.norm;
	  if (fp->f.lang)
	    f.f.lang = fp->f.lang;
	  char *n = strstr((ccp)f.f.lang, "/n");
	  if (n)
	    {
	      *n = '\0';
	      struct map *l949 = lang949((ccp)f.f.lang, strlen((ccp)f.f.lang));
	      if (l949)
		f.f.lang = (uccp)l949->v;
	      else
		fprintf(stderr,
			"glosigx: no -949 version of lang %s in lang949.g; ignoring -949 here\n",
			f.f.lang);		
	      f.f.form = (uccp)"*";
	    }
	  else
	    f.f.form = fp->f.form;

	  if (BIT_ISSET(fp->f.flags, FORM_FLAGS_COF_HEAD))
	    {
	      List *csigl = cof_sigs(&f, csetp->pool);
	      char *s;
	      if (out_stdout)
		{
		  for (s = list_first(csigl); s; s = list_next(csigl))		
		    fprintf(stdout, "%s\n", s);
		}
	      else
		cbd_sig_add_list(csigl);
	    }
	  else
	    {
	      f.f.flags = fp->f.flags;
	      f.f.parts = fp->f.parts;
	      /*f.f.cof_id = fp->f.cof_id;*//*No: this gets set on cbd load*/
	      char *sig = (char*)form_sig(csetp->pool, &f.f);
	      if (out_stdout)
		fprintf(stdout, "%s\n", sig);
	      else
		cbd_sig_add_one((uccp)sig, sp->rank | fp->f.rank);
	    }
	}
    }
}

static void
cbd_sigs(Cbd *c)
{
  Entry *ep;
  for (ep = list_first(c->entries); ep; ep = list_next(c->entries))
    if (ep->parts)
      psu_sigs(ep);
    else
      cbd_entry_sigs(ep);
}

static void
qpn_sub(Lemsig *lp, const char *epos)
{
  struct map *m = qpnnames(epos, strlen(epos));
  if (m)
    {
      const char *qlng = m->v;
      FILE *qfp = hash_find(hqlngs, (uccp)qlng);
      if (!qfp)
	{
	  char d[strlen("01bld/0")+strlen(qlng)];
	  strcpy(d, "01bld/");
	  strcat(d, qlng);
	  if (-1 == mkdir(d, 0700) && EEXIST != errno)
	    {
	      perror(qlng);
	      exit(1);
	    }
	  char qfn[strlen(qlng)+strlen("01bld//glo.sig0")];
	  sprintf(qfn, "01bld/%s/glo.sig", qlng);
	  qfp = xfopen(qfn, "w");
	  if (!qfp)
	    {
	      perror(qfn);
	      exit(1);
	    }
	  else
	    {
	      hash_add(hqlngs, (uccp)qlng, qfp);
	      fputs("@fields sig rank freq\n", qfp);
	    }
	}
      fprintf(qfp, "%s\t%d\t%d\n", lp->sig, lp->rank, lp->freq);      
    }
}

int
main(int argc, char * const *argv)
{
  extern int gdl_flex_debug, gdldebug, cbd_flex_debug, cbddebug;
  /*int nglo = 0, nlem = 0;*/
  char **glos = glo_files();
  char **lems = lem_files();

  common_init();

  options(argc,argv,"gqs");
  
  if (argv[optind])
    glos = (char**)argv+optind;
  else
    glos = glo_files();

  if (!glo_mode)
    lems = lem_files();
    
  if (glos)
    {
      int i;
      if (verbose)
	{
	  fputs("00lib/*.glo =", stdout);
	  for (i = 0; glos[i]; ++i)
	    printf(" %s", glos[i]);
	  fputc('\n', stdout);
	}
    }
  else
    {
      if (!quiet)
	fprintf(stderr, "glosigx: no 00lib/*.glo found.\n");
      if (glo_mode)
	exit(0);
    }
  if (lems)
    {
      int i;
      if (verbose)
	{
	  fputs("02pub/lemm-*.sig =", stdout);
	  for (i = 0; lems[i]; ++i)
	    printf(" %s", lems[i]);
	  fputc('\n', stdout);
	}
    }
  else if (!glo_mode)
    fprintf(stderr, "glosigx: no 02pub/lemm-*.sig found.\n");

  gdl_flex_debug = gdldebug = cbddebug = 0;
  cbd_flex_debug = 0;

  Hash *htokens = hash_create(1024);
  Memo *mtokens = memo_init(sizeof(Gt), 1024);

  if (glo_mode)
    hqlngs = hash_create(32);

  gdl_unicode = 1;
  gdl_init();
  gsort_init();
  gdl_set_word_id("w");
  gt_config(htokens, mtokens);
  gvl_setup("osl","osl","020");
 
  int i;
  for (i = 0; glos[i]; ++i)
    {
      mesg_init();
      Cbd *c = cbd_bld_cbd();
      curr_cbd = c;
      file = efile = curr_cbd->file = glos[i];
      phase = "syn";
      FILE *glofp = xfopen(file, "r");
      if (glofp)
	{
	  extern void cbdrestart(FILE*fp);
	  extern int cbdlineno;
	  if (verbose)
	    fprintf(stderr, "glosigx: reading %s\n", file);
	  cbdrestart(glofp);
	  cbdlineno = 1;
	  int parse_return = cbdparse();
	  if (parse_return || parser_status)
	    {
	      mesg_print(stderr);
	      exit(1);
	    }
	  xfclose(file, glofp);
	  cbd_psus();
	  cbd_sigs(c);
	}
      mesg_print(stderr);
    }

  if (!out_stdout)
    {
      int i;

      if (lems)
	{
	  for (i = 0; lems[i]; ++i)
	    lemm_sigs(lems[i]);
	}

      const char **langs = hash_keys(csetp->hsiglangs);
      for (i = 0; langs[i]; ++i)
	{
	  List *lp = hash_find(csetp->hsiglangs, (uccp)langs[i]);
	  if (list_len(lp))
	    {
	      int n;
	      Lemsig **lsp = (Lemsig**)list2array_c(lp, &n);
	      if (glo_mode)
		{
		  char d[strlen("01bld/0")+strlen(langs[i])];
		  strcpy(d, "01bld/");
		  strcat(d, langs[i]);
		  if (-1 == mkdir(d, 0700) && EEXIST != errno)
		    {
		      perror(langs[i]);
		      exit(1);
		    }
		  char fn[strlen(langs[i])+strlen("01bld/glo.sig0")];
		  sprintf(fn, "01bld/%s/glo.sig", langs[i]);
		  FILE *fp;
		  if ((fp = xfopen(fn, "w")))
		    {
		      int j;
		      fputs("@fields sig rank freq\n", fp);
		      for (j = 0; lsp[j]; ++j)
			{
			  fprintf(fp, "%s\t%d\t%d\n", lsp[j]->sig, lsp[j]->rank, lsp[j]->freq);
			  char *sqb = strchr((ccp)lsp[j]->sig, '\'');
			  if ('N' == sqb[2] && !isalnum(sqb[3]))
			    {
			      char epos[3] = { sqb[1], sqb[2], '\0' };
			      qpn_sub(lsp[j], epos);
			    }
			}
		      xfclose(fn, fp);
		    }
		}
	      else
		{
		  Hash *hlem = hash_find(csetp->lems, (uccp)langs[i]);
		  if (hlem)
		    {
		      int ii;
		      for (ii = 0; lsp[ii]; ++ii)
			{
			  const char **r = hash_find(hlem, lsp[ii]->sig);
			  if (r)
			    /* Don't set rank here; only do it from glossary */
			    lsp[ii]->freq = atoi(r[2]);
			}
		    }
		  qsort(lsp, n, sizeof(Lemsig *), (sort_cmp_func*)lemsig_cmp);
		  char fn[strlen(langs[i])+strlen("02pub/lemm-.sig0")];
		  sprintf(fn, "02pub/lemm-%s.sig", langs[i]);
		  FILE *fp;
		  if ((fp = xfopen(fn, "w")))
		    {
		      int j;
		      fputs("@fields sig rank freq\n", fp);
		      for (j = 0; lsp[j]; ++j)
			fprintf(fp, "%s\t%d\t%d\n", lsp[j]->sig, lsp[j]->rank, lsp[j]->freq);
		      xfclose(fn, fp);
		    }
		  else
		    fprintf(stderr, "glosigx: skipping output to %s\n", fn);
		}
	    }
	}
    }
}

int opts(int och, const char *oarg)
{
  switch (och)
    {
    case 'g':
      glo_mode = 1;
      break;
    case 'q':
      quiet = 1;
      break;
    case 's':
      out_stdout = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void){}
