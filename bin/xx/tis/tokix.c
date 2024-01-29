#include <oraccsys.h>
#include <dbi.h>
#include <ose.h>
#include <vid.h>

const char *curr_project = NULL;
static Dbi_index*dip;
static int cache_size = 32;

const char *curr_index = "tok";

int
main(int argc, char * const*argv)
{
  options(argc,argv,"p:");

  if (!curr_project)
    usage();

  const char *index_dir = ose_dir(curr_project, curr_index);

  struct vid_data *vp = vid_init();
  
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
  

  /* index toks here */
  char buf[1024], *s;
  while ((s = fgets(buf, 1024, stdin)))
    {
      static struct location8 l8;
      s = strchr(buf, '\t');
      if (s)
	{
	  ++s;
	  ose_wid2loc8(vid_map_id(vp,s),NULL,&l8);
	  dbi_add(dip,(unsigned char*)buf,&l8,1);
	}
    }    

  dbi_flush(dip);
  dbi_free(dip);

  ose_ce_cfg(curr_project, "tok", "tr", "txh", oce_tok, NULL);

  vid_dump_data(vp, ose_file(curr_project, curr_index, "vid.dat"));
  vid_term(vp);

  return 0;
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
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
