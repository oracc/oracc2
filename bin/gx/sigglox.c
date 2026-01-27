#include <oraccsys.h>
#include <form.h>

const char *dir = "01bld";
const char *project = NULL;

Hash *cbds;
Pool *p, *hp;

typedef struct entry
{
  const char *cgp;
  Hash *bases;
  Hash *forms;
  Hash *senses;
} Entry;

static void
sgx_entry(FILE *fp, const char *cgp, Entry *ep)
{
  fprintf(fp, "@entry %s\n", cgp);
  
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
  qsort(ee, n, sizeof(const char *), cmpstringp);
  int i;
  for (i = 0; i < n; ++i)
    sgx_entry(fp, ee[i], hash_find(h, (uccp)ee[i]));
  xfclose(fn,fp);
}

int
main(int argc, char * const *argv)
{
  size_t line = 0;
  const char *file;
  FILE *in_fp;
  int qpn_mode = 0;
  cbds = hash_create(10);
  form_init();
  p = pool_init();
  hp = hpool_init();
  Memo *mem_entries;
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
      
      Form f;
      form_parse((uccp)file, line, pool_copy((uccp)s,p), &f, NULL);
      const unsigned char *cbdlang = f.lang;
      if (strlen((ccp)f.pos) == 2 && 'N' == f.pos[1])
	{
	  cbdlang = (uccp)"qpn";
	  qpn_mode = 1;
	}
      else
	qpn_mode = 0;
      Hash *cbd = hash_find(cbds, cbdlang);
      if (!cbd)
	hash_add(cbds, cbdlang, (cbd = hash_create(1024)));
      char cgp[strlen((ccp)f.cf)+strlen((ccp)f.gw)+strlen((ccp)f.pos)+strlen(" [] 0")];
      sprintf(cgp, "%s [%s] %s", f.cf, f.gw, f.pos);
      Entry *ep = hash_find(cbd, (uccp)cgp);
      if (!ep)
	{
	  ep = memo_new(mem_entries);
	  ep->bases = hash_create(8);
	  ep->forms = hash_create(16);
	  ep->senses = hash_create(8);
	  ep->cgp = (ccp)pool_copy((uccp)cgp, p);
	  hash_add(cbd, (uccp)ep->cgp, ep);
	}
      char sns[strlen((ccp)f.epos)+strlen((ccp)f.sense)+strlen(" 0")];
      sprintf(sns, "%s %s", f.epos, f.sense);
      
      if (!hash_find(ep->senses, (uccp)sns))
	hash_add(ep->senses, pool_copy((uccp)sns, p), "");

      if (!hash_find(ep->bases, f.base))
	hash_add(ep->bases, pool_copy(f.base, p), "");
      
      unsigned char *fp = form_cbd(&f, p, qpn_mode);
      if (!hash_find(ep->forms, fp))
	hash_add(ep->forms, fp, "");	
    }

  const char **langs = hash_keys(cbds);
  int i;
  for (i = 0; langs[i]; ++i)
    sgx_lang(langs[i], hash_find(cbds, (uccp)langs[i]));
  
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
