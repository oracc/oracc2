#include <oraccsys.h>
#include <cbd.h>

/* Read all the 00lib *.glo files and output 02pub/lemm-*.sig files.
 *
 * Use existing 02pub/lemm-*.sig files to save instance info for sigs
 * that are already known.
 */

extern struct map *lang949(register const char *str, register size_t len);

int parser_status = 0;
int verbose = 1;
int bootstrap_mode, lem_autolem, lem_dynalem;
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

static List *
cbd_no_form_bases(Entry *ep)
{
  List *no_fo_ba = list_create(LIST_SINGLE);
  
  /* index the bases that occur in @form with morph=#~ */
  Hash *fb = hash_create(5);
  Form *fp;
  for (fp = list_first(ep->forms); fp; fp = list_next(ep->forms))
    if (fp->morph && !strcmp((ccp)fp->morph, "#~"))
      hash_add(fb, fp->base, "");

  /* now go through the primary bases and create a basic @form for
     those not in the form-base index */
  List_node *outer;
  for (outer = ep->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      struct loctok *ltp = (struct loctok *)inner->data;
      const unsigned char *pri = ltp->tok;
      if (!hash_find(fb, pri))
	{
	  if (strlen((ccp)ep->cgp->pos) == 2 && 'N' == ep->cgp->pos[1])
	    {
	      const char *lang = ltp->lang ? ltp->lang : "sux";
	      char f[strlen((ccp)pri)*2 + strlen(lang) + strlen("%/#~0")];
	      sprintf(f, "%s%%%s/%s#~", pri, lang, pri);
	      list_add(no_fo_ba, pool_copy((uccp)f, csetp->pool));
	    }
	  else
	    {
	      char f[strlen((ccp)pri)*2 + strlen("/#~0")];
	      sprintf(f, "%s/%s #~", pri, pri);
	      list_add(no_fo_ba, pool_copy((uccp)f, csetp->pool));
	    }
	}
    }
  
  if (list_len(no_fo_ba))
    return no_fo_ba;
  else
    {
      list_free(no_fo_ba, NULL);
      return NULL;      
    }
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
  List *no_form_bases = NULL;
  if (list_len(ep->bases))
    no_form_bases = cbd_no_form_bases(ep);

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
	  if (BIT_ISSET(fp, FORM_FLAGS_COF_TAIL))
	    {
	      continue;
	    }
	  else if (BIT_ISSET(fp, FORM_FLAGS_COF_TAIL))
	    {
	      cof_sigs(csetp->pool, fp, stdout);
	    }
	  else
	    {
	      if (fp->lang)
		f.lang = fp->lang;
	      char *n = strstr(f.lang, "/n");
	      if (n)
		{
		  *n = '\0';
		  struct map *l949 = lang949(f.lang, strlen(f.lang));
		  if (l949)
		    f.lang = l949->v;
		  else
		    fprintf(stderr,
			    "glosigx: no -949 version of lang %s in lang949.g; ignoring -949 here\n",
			    f.lang);		
		  f.form = "*";
		}
	      else
		f.form = fp->form;
	      f.base = fp->base;
	      f.stem = fp->stem;
	      f.cont = fp->cont;
	      f.norm = fp->norm;
	      f.flags = fp->flags;
	      f.parts = fp->parts;
	      /*f.cof_id = fp->cof_id;*//*No: this gets set on cbd load*/
	    }

	  fprintf(stdout, "%s\n", form_sig(csetp->pool, &f));
	}
    }
}

static void
cbd_sigs(Hash *h, Cbd *c)
{
  Entry *ep;
  for (ep = list_first(c->entries); ep; ep = list_next(c->entries))
    cbd_entry_sigs(ep, h);
}

int
main(int argc, char * const *argv)
{
  extern int gdl_flex_debug, gdldebug, cbd_flex_debug, cbddebug;
  int nglo = 0, nlem = 0;
  common_init();
  char **glos = glo_files();
  char **lems = lem_files();
  if (glos)
    {
      int i;
      fputs("00lib/*.glo =", stdout);
      for (i = 0; glos[i]; ++i)
	printf(" %s", glos[i]);
      fputc('\n', stdout);
      nglo = i;
    }
  else
    fprintf(stderr, "glosigx: no 00lib/*.glo found.\n");
  if (lems)
    {
      int i;
      fputs("02pub/lemm-*.sig =", stdout);
      for (i = 0; lems[i]; ++i)
	printf(" %s", lems[i]);
      fputc('\n', stdout);
      nlem = i;
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
