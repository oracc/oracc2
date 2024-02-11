#include <oraccsys.h>
#include <dbi.h>
#include <ose.h>
#include <vido.h>

const char *curr_project = NULL;
static Dbi_index*dip;
static int cache_size = 32;

const char *curr_index = "tok";
const char *index_dir = NULL;
const char *outfile = NULL;

struct tokix_data
{
  const char *fn;
  FILE *fp;
  Hash *htokens; /* TID to TOKEN */
  Hash *hcounts; /* TID to COUNT */
  Pool *p;
} td;

void
tokix_hook(const char *key, Unsigned32 n, struct tokix_data *tp)
{
#if 0
  fprintf(stderr, "tokix_hook: adding count %u for key %s\n", n, key);
#endif
  hash_add(tp->hcounts, hash_exists(tp->htokens,(uccp)key), (void*)(uintptr_t)n);
}

int
main(int argc, char * const*argv)
{
  options(argc,argv,"d:o:p:");

  if (!curr_project && !index_dir)
    {
      usage();
      exit(1);
    }

  if (!index_dir)
    index_dir = ose_dir(curr_project, curr_index);

  Vido *vp = vido_init('v', 1);
  
  dip = dbi_create("tok",
		  index_dir,
		  500000,
		  sizeof(struct location8),
		  DBI_ACCRETE);

  if (NULL == dip)
    {
      fprintf(stderr, "unable to create index for %s:%s", curr_project, curr_index);
      exit(1);
    }

  dbi_set_vids(dip,"vid.vid");
  dbi_set_type(dip,DBI_DT_LOC8);
  dbi_set_user(dip,d_tok);
  dbi_set_cache(dip,cache_size);

  if (outfile)
    {
      td.fn = outfile;
      td.fp = fopen(td.fn, "w");
    }
  else
    {
      td.fn = "<stdout>";
      td.fp = stdout;
    }
  td.htokens = hash_create(4096);
  td.hcounts = hash_create(4096);
  td.p = pool_init();
  
  dbi_set_hook(dip, (void(*)(const char *,Unsigned32,void*))tokix_hook, &td);
  
  /* index toks here; the input is a triple:
   *
   * TOKEN TOKENID WORDID
   */
  char buf[1024], *s;
  while ((s = fgets(buf, 1024, stdin)))
    {
      static struct location8 l8;
      s[strlen(s)-1] = '\0';
      s = (char*)pool_copy((uccp)s, td.p);
      char **f = tab_split(s);
      if (f[0] && f[1] && f[2])
	{
	  hash_add(td.htokens, (uccp)f[1], f[0]);
	  ose_wid2loc8(vido_new_id(vp,f[2]),NULL,&l8);
	  dbi_add(dip,(unsigned char*)f[1],&l8,1);
	}
      free(f);
    }    

  dbi_flush(dip);

  const char **k = hash_keys(td.htokens);
  int i;
  for (i = 0; k[i]; ++i)
    fprintf(td.fp, "%s\t%s\t%lu\n",
	    (char*)hash_find(td.htokens, (uccp)k[i]),
	    k[i],
	    (uintptr_t)hash_find(td.hcounts, (uccp)k[i]));
  
  /* ose_ce_cfg(curr_project, "tok", "tr", "txh", oce_tok, NULL); */

  vido_dump_data(vp, "02pub/tok/vid.vid", NULL);
  vido_term(vp);

  dbi_free(dip);

  if (strcmp(td.fn, "<stdout>"))
    fclose(td.fp);

  return 0;
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      index_dir = arg;
      break;
    case 'o':
      outfile = arg;
      break;
    case 'p':
      curr_project = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "tokix";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-p [project]";
void
help ()
{
  printf("  -p [project] Gives the name of the project; required\n");
}
