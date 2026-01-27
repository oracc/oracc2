#include <oraccsys.h>
#include <cbd.h>

/* Read all the 00lib *.glo files and output 02pub/lemm-*.sig files.
 *
 * Use existing 02pub/lemm-*.sig files to save instance info for sigs
 * that are already known.
 */
int parser_status = 0;
int verbose = 1;
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

static void
cbd_entry_sigs(Entry *ep, Hash *h)
{
  fprintf(stderr, "@%s%%%s:%s[%s]%s\n",
	  ep->owner->project,
	  ep->owner->lang,
	  ep->cgp->cf,
	  ep->cgp->gw,
	  ep->cgp->pos);
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
