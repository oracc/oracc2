#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

/* Read all the 00lib *.glo files and output 02pub/lemm-*.sig files.
 *
 * Use existing 02pub/lemm-*.sig files to save instance info for sigs
 * that are already known.
 */

extern struct map *lang949(register const char *str, register size_t len);

int parser_status = 0;
int verbose = 1;
int bootstrap_mode, lem_autolem, lem_dynalem;
int out_stdout;

struct cbd* curr_cbd;
struct entry*curr_entry;
const char *file, *efile, *errmsg_fn;
extern void common_init(void);

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
 * In order to ensure that any base in the glossary entry can be
 * matched as a sig even if there is no corresponding @form, we begin
 * by augmenting the list of @form with a minimal @form for any base
 * that is not attested in its absolute form (i.e., with morph=#~).
 */
static void
cbd_entry_sigs(Entry *ep, Hash *h)
{
  Form f;
  memset(&f, '\0', sizeof(Form));

  f.project = ep->owner->project;
  f.lang = ep->owner->lang;
  f.cf = ep->cgp->cf;
  f.gw = ep->cgp->gw;
  f.pos = ep->cgp->pos;

  Sense *sp;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    {
      f.sense = sp->mng;
      f.epos = sp->pos;

      Form *fp;
      for (fp = list_first(ep->forms); fp; fp = list_next(ep->forms))
	{
	  if (BIT_ISSET(fp->flags, FORM_FLAGS_COF_TAIL))
	    continue;

	  f.base = fp->base;
	  f.stem = fp->stem;
	  f.cont = fp->cont;
	  f.norm = fp->norm;
	  if (fp->lang)
	    f.lang = fp->lang;
	  char *n = strstr((ccp)f.lang, "/n");
	  if (n)
	    {
	      *n = '\0';
	      struct map *l949 = lang949((ccp)f.lang, strlen((ccp)f.lang));
	      if (l949)
		f.lang = (uccp)l949->v;
	      else
		fprintf(stderr,
			"glosigx: no -949 version of lang %s in lang949.g; ignoring -949 here\n",
			f.lang);		
	      f.form = (uccp)"*";
	    }
	  else
	    f.form = fp->form;

	  if (BIT_ISSET(fp->flags, FORM_FLAGS_COF_HEAD))
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
	      f.flags = fp->flags;
	      f.parts = fp->parts;
	      /*f.cof_id = fp->cof_id;*//*No: this gets set on cbd load*/
	      char *sig = (char*)form_sig(csetp->pool, &f);
	      if (out_stdout)
		fprintf(stdout, "%s\n", sig);
	      else
		cbd_sig_add_one((uccp)sig);
	    }
	}
    }
}

static void
cbd_sigs(Hash *h, Cbd *c)
{
  Entry *ep;
  for (ep = list_first(c->entries); ep; ep = list_next(c->entries))
    if (ep->parts)
      psu_sigs(ep);
    else
      cbd_entry_sigs(ep, h);
}

int
main(int argc, char * const *argv)
{
  extern int gdl_flex_debug, gdldebug, cbd_flex_debug, cbddebug;
  /*int nglo = 0, nlem = 0;*/
  char **glos = glo_files();
  char **lems = lem_files();

  common_init();

  options(argc,argv,"s");
  
  if (argv[optind])
    glos = (char**)argv+optind;
  else
    glos = glo_files();
  lems = lem_files();
    
  if (glos)
    {
      int i;
      if (verbose)
	fputs("00lib/*.glo =", stdout);
      for (i = 0; glos[i]; ++i)
	if (verbose)
	  printf(" %s", glos[i]);
      if (verbose)
	fputc('\n', stdout);
      /*nglo = i;*/
    }
  else
    fprintf(stderr, "glosigx: no 00lib/*.glo found.\n");
  if (lems)
    {
      int i;
      if (verbose)
	fputs("02pub/lemm-*.sig =", stdout);
      for (i = 0; lems[i]; ++i)
	if (verbose)
	  printf(" %s", lems[i]);
      if (verbose)
	fputc('\n', stdout);
      /*nlem = i;*/
    }
  else
    fprintf(stderr, "glosigx: no 02pub/lemm-*.sig found.\n");

  gdl_flex_debug = gdldebug = cbddebug = 0;
  cbd_flex_debug = 0;

  Hash *hsig = hash_create(16);
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
	  cbd_sigs(hsig, c);
	  xfclose(file, glofp);
	}
      mesg_print(stderr);
    }
}

int opts(int och, const char *oarg)
{
  switch (och)
    {
    case 's':
      out_stdout = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void){}
