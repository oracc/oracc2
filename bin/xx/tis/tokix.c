#include <oraccsys.h>
#include <dbi.h>
#include <ose.h>
#include <vido.h>

const char *curr_project = NULL;
static Dbi_index*dip;
static int cache_size = 32;

const char *counts_file = NULL;
const char *curr_index = "tok";
const char *index_dir = NULL;

int
main(int argc, char * const*argv)
{
  FILE *counts_fp = NULL;
  
  options(argc,argv,"c:d:p:");

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

  dbi_set_user(dip,d_tok);
  dbi_set_cache(dip,cache_size);

  if (counts_file)
    {
      counts_fp = fopen(counts_file, "w");
      if (counts_fp)
	dbi_set_counts(counts_fp);  
    }
  
  /* index toks here */
  char buf[1024], *s;
  while ((s = fgets(buf, 1024, stdin)))
    {
      static struct location8 l8;
      s[strlen(s)-1] = '\0';
      s = strchr(buf, '\t');
      if (s)
	{
	  *s++ = '\0';
	  ose_wid2loc8(vido_new_id(vp,s),NULL,&l8);
	  dbi_add(dip,(unsigned char*)buf,&l8,1);
	}
    }    

  dbi_flush(dip);
  dbi_free(dip);

  if (counts_fp)
    fclose(counts_fp);

  /* ose_ce_cfg(curr_project, "tok", "tr", "txh", oce_tok, NULL); */

  vido_dump_data(vp, "vid.vid"/*ose_file(curr_project, curr_index, "vid.dat")*/, NULL);
  vido_term(vp);

  return 0;
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'c':
      counts_file = arg;
      break;
    case 'd':
      index_dir = arg;
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
