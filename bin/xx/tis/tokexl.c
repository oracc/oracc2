#include <oraccsys.h>
#include <vido.h>
#include <form.h>

/**tokex reads and expands the output of tokx, simplifying it to a
 * stream of triples of the form:
 *
 * token token-ID word-ID
 *
 * The expansions vary according to token type.
 *
 * THIS PROGRAM IS FOR LEMMATIZATION SIGNATURES ^l IN TOKX OUTPOUT
 *
 * A signature looks like this:
 *
 * @gudea%sux:mu-tud=utud[bear//to bear]V/t'V/t$mu:utud/tud#mu:~
 *
 * The expansion mimics the tripartite structure of grapheme
 * keys. Because the content of lemma keys can be practically any
 * character, we separate the parts with ^A (ASCII 1).
 *
 * The full signature is saved without ^A
 
 * Part 1, sem(el) is the LANG_CGP:
 *
 * 	%sux:utud[bear]V/t^A^A
 *
 * Part 2, bis, is the SENSE/EPOS:
 *
 *	%sux:utud[bear]V/t^A[to bear]V/t^A
 *
 * Part 3, ter, is constructed for the CGP and each bis; this is
 * then emitted for each individual signature field:
 *
 *      %sux:utud[bear]V/t^A[to bear]V/t^Amu-tud
 *      %sux:utud[bear]V/t^A[to bear]V/t^A$mu:utud
 *      %sux:utud[bear]V/t^A[to bear]V/t^A/tud
 *      %sux:utud[bear]V/t^A[to bear]V/t^A#mu:~
 */

const char *index_dir = "02pub/tokl";
const char *project = NULL;
char period[1024];
Vido *vp;

int files_from_stdin = 0;
int no_output = 0;

static Form f;
const char *file;
Pool *mpool;
Hash *mhash;

static void
pr(const char *k, Vido *vp, const char *q)
{
  printf("%s\t%s\t%s\n", k, vido_new_id(vp,k), q);
}

static void
pr_sig_fields(char *t, const char *qid)
{
  char *insert = t + strlen(t);
  sprintf(insert, "%s", period);
  pr(t, vp, qid);
#define fpr(c,x) if(f.x){sprintf(insert,"%c%s",c,f.x); pr(t,vp,qid);}
  if (f.oform)
    {
      sprintf(insert, "=%s~~%s", f.form, f.oform);
      pr(t,vp,qid);
    }
  else
    fpr('=',form);
  fpr('$',norm);
  fpr('#',morph);
  fpr('/',base);
  fpr('+',cont);
  fpr('*',cont);
  if (f.morph2)
    {
      sprintf(insert, "##%s", f.morph2);
      pr(t,vp,qid);
    }
#undef fpr
}

static void
toks_from_file(const char *fn, FILE *fp, Vido *vp)
{
  char *b, qid[1024];
  size_t line = 0;
  while ((b = (char*)loadoneline(fp, NULL)))
    {
      ++line;
      
      if ((b[0] != 'K' && b[0] != 'l') || b[1] != '\t')
	continue;
      
      if (b[strlen((ccp)b)-1] == '\n')
	b[strlen((ccp)b)-1] = '\0';

      if (b[0] == 'K')
	{
	  if (!strncmp((ccp)b+2, "period", 6))
	    strcpy(period, (ccp)b+9);
	  continue;
	}
      
      unsigned char *s;
      char*w;
      w = (char*)b+2;
      s = (ucp)strchr(w, '\t');  /* signature */
      if (s)
	{
	  *s++ = '\0';
	  char *tab = strchr((ccp)s, '\t');
	  if (tab)
	    *tab = '\0';
	}

      /* ignore l entries for bad lemmatizations */
      if (s[0] != '@')
	continue;
      
      /* For l-tokens the word location is the project set on the command line */
      strcpy(qid, project);
      strcat(qid, ":");
      strcat(qid, w);
      
      /* Always save the entire sig */
      pr((ccp)s, vp, qid);

      memset(&f, '\0', sizeof(Form));
      form_parse((uccp)file, line, (ucp)s, &f, NULL);

      char t[strlen((ccp)s)+strlen(period)+10];

      sprintf(t, "%%%s:%s[%s]%s%c%c", f.lang, f.cf, f.gw, f.pos, 1, 1);
      pr(t, vp, qid);

      pr_sig_fields(t, qid);
      
      if (strcmp((ccp)f.gw, (ccp)f.sense) || strcmp((ccp)f.pos, (ccp)f.epos))
	{
	  sprintf(t, "%%%s:%s[%s]%s%c//%s'%s%c", f.lang, f.cf, f.gw, f.pos, 1, f.sense, f.epos, 1);
	  pr(t, vp, qid);
	  pr_sig_fields(t, qid);
	}
    }
}

void
stdin_files_toks(Vido *vp)
{
  char buf[_MAX_PATH+1], *b;
  while ((b = fgets(buf, _MAX_PATH, stdin)))
    {
      FILE *fp = xfopen(buf, "r");
      if (fp)
	toks_from_file(buf, fp, vp);
      xfclose(buf, fp);
    }
}

int
main(int argc, char **argv)
{
  options(argc,argv,"d:nl:p:s");
  if (project)
    {
      mpool = pool_init();
      mhash = hash_create(100);
    }
  else
    {
      fprintf(stderr, "%s: -p [PROJECT] is required. Stop.\n", "tokexl");
      exit(1);
    }

  form_init();
  Vido *vp = vido_init('l', 0);
  *period = '\0';

  if (argv[optind])
    {
      file = argv[optind];
      FILE *in_fp = xfopen(file, "r");
      if (!in_fp)
	exit(1);
      toks_from_file(file, stdin, vp);
    }
  else if (files_from_stdin)
    stdin_files_toks(vp);
  else
    toks_from_file("-", stdin, vp);
  
  if (!no_output)
    {
      char vidfile[strlen(index_dir)+strlen("tid.vid0")];
      char tsvfile[strlen(index_dir)+strlen("tid.vid0")];
      sprintf(vidfile,"%s/tid.vid",index_dir);
      sprintf(tsvfile,"%s/tid.tsv",index_dir);
      vido_dump_data(vp, vidfile, tsvfile);
      vido_term(vp);
    }
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      index_dir = arg;
      break;
    case 'n':
      no_output = 1;
      break;
    case 'p':
      project = arg;
      break;
    case 's':
      files_from_stdin = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "tokexl";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "-d [index_dir] [-n]";
void
help (void)
{
  printf("  -d [index_dir] Gives the name of the index directory; defaults to 02pub/tokl\n");
  printf("  -n no output; suppress vid dump\n");
  printf("  -p project; required if merge data is used by project\n");
}
