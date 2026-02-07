#include <oraccsys.h>
#include <form.h>
#include <collate.h>

/* Turn .sig into .glo: N.B. this program is standalone and does not
   use lib/cbd so it uses Form not CForm */

const char *dir = "01bld";
const char *project = NULL;

Hash *cbds;
Pool *p, *hp;
Memo *mem_entries;

typedef struct entry
{
  const char *cgp;
  Hash *bases;
  Hash *forms;
  Hash *senses;
  const char *parts;
} Entry;

static void
sgx_bases(FILE *fp, Hash *b)
{
  int n;
  const char **bb = hash_keys2(b, &n);
  fprintf(fp, "@bases ");
  int i;
  for (i = 0; bb[i]; ++i)
    {
      fputs(bb[i], fp);
      if (bb[i+1])
	fputs("; ", fp);
    }
  fputs("\n", fp);
}

static void
sgx_forms(FILE *fp, Hash *f)
{
  int n;
  const char **ff = hash_keys2(f, &n);
  int i;
  for (i = 0; ff[i]; ++i)
    fprintf(fp, "@form %s\n", ff[i]);
}

static void
sgx_senses(FILE *fp, Hash *s)
{
  int n;
  const char **ff = hash_keys2(s, &n);
  int i;
  for (i = 0; ff[i]; ++i)
    fprintf(fp, "@sense %s\n", ff[i]);  
}

static void
sgx_entry(FILE *fp, const char *cgp, Entry *ep)
{
  fprintf(fp, "@entry %s\n", cgp);
  if (ep->parts)
    fprintf(fp, "@parts %s\n", ep->parts);
  if (ep->bases->key_count)
    sgx_bases(fp, ep->bases);
  if (ep->forms->key_count)
    sgx_forms(fp, ep->forms);
  if (ep->senses->key_count)
    sgx_senses(fp, ep->senses);
  fputs("@end entry\n\n", fp);
}

static void
sgx_lang(const char *l, Hash *h)
{
  char fn[strlen(dir)+(2*strlen(l))+strlen("//.sgx0")];
  sprintf(fn, "%s/%s/%s.sgx", dir, l, l);
  FILE *fp = xfopen(fn, "w");
  fprintf(fp, "@project %s\n@lang %s\n@name %s,%s\n\n", project, l, project, l);
  int n;
  const char **ee = hash_keys2(h, &n);
  qsort(ee, n, sizeof(const char *), /*cmpstringp*/ (sort_cmp_func*)collate_cmp_utf8_qs);
  int i;
  for (i = 0; i < n; ++i)
    sgx_entry(fp, ee[i], hash_find(h, (uccp)ee[i]));
  xfclose(fn,fp);
}

static void
register_form(Form *fp)
{
  int qpn_mode = 0;
  const unsigned char *cbdlang = fp->lang;
  if (strlen((ccp)fp->pos) == 2 && 'N' == fp->pos[1])
    {
      cbdlang = (uccp)"qpn";
      qpn_mode = 1;
    }
  else
    qpn_mode = 0;
  Hash *cbd = hash_find(cbds, cbdlang);
  if (!cbd)
    hash_add(cbds, cbdlang, (cbd = hash_create(1024)));
  char cgp[strlen((ccp)fp->cf)+strlen((ccp)fp->gw)+strlen((ccp)fp->pos)+strlen(" [] 0")];
  sprintf(cgp, "%s [%s] %s", fp->cf, fp->gw, fp->pos);
  Entry *ep = hash_find(cbd, (uccp)cgp);
  if (!ep)
    {
      ep = memo_new(mem_entries);
      ep->bases = hash_create(8);
      ep->forms = hash_create(16);
      ep->senses = hash_create(8);
      ep->cgp = (ccp)pool_copy((uccp)cgp, p);
      hash_add(cbd, (uccp)ep->cgp, ep);
      if (fp->psu_ngram)
	ep->parts = (ccp)pool_copy(fp->psu_ngram, p);
    }
  char sns[strlen((ccp)fp->epos)+strlen((ccp)fp->sense)+strlen(" 0")];
  sprintf(sns, "%s %s", fp->epos, fp->sense);
      
  if (!hash_find(ep->senses, (uccp)sns))
    hash_add(ep->senses, pool_copy((uccp)sns, p), "");

  if (fp->base && !hash_find(ep->bases, fp->base))
    hash_add(ep->bases, pool_copy(fp->base, p), "");
      
  unsigned char *frmp = form_cbd(fp, p, qpn_mode);
  if (!hash_find(ep->forms, frmp))
    hash_add(ep->forms, frmp, "");
}

int
main(int argc, char * const *argv)
{
  size_t line = 0;
  const char *file;
  FILE *in_fp;
  cbds = hash_create(10);
  form_init();
  mesg_init();
  p = pool_init();
  hp = hpool_init();
  collate_init((uccp)"unicode");
  collate_set_tiles_i();
  mem_entries = memo_init(sizeof(Entry), 1024);

  options(argc,argv,"d:p:");

  if (argv[optind])
    {
      file = argv[optind];
      in_fp = xfopen(file, "r");
      if (!in_fp)
	exit(1);
    }
  else
    {
      file = "-";
      in_fp = stdin;
    }

  unsigned char *s;
  while ((s = loadoneline(in_fp, NULL)))
    {
      ++line;
      
      if (line == 1 && !strncmp((ccp)s, "@fields", strlen("@fields")))
	continue;
      
      static Form f;
      memset(&f, '\0', sizeof(Form));
      form_parse((uccp)file, line, pool_copy((uccp)s,p), &f, NULL);
      register_form(&f);

      if (f.cof_id)
	{
	  int i;
	  for (i = 0; f.parts[i]; ++i)
	    register_form(f.parts[i]);
	}
    }

  const char **langs = hash_keys(cbds);
  int i;
  for (i = 0; langs[i]; ++i)
    sgx_lang(langs[i], hash_find(cbds, (uccp)langs[i]));

  mesg_print(stderr);
}

int bootstrap_mode, lem_autolem, lem_dynalem, verbose;

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      dir = arg;
      break;
    case 'p':
      project = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "sigglox";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "-d [dir] -p [project]";
void
help (void)
{
  printf("  -d [dir] Gives the name of the outpout directory; defaults to 01bld/\n");
  printf("  -p project; required if merge data is used by project\n");
}
