#include <oraccsys.h>

extern const char *textid;
extern FILE *f_log;
FILE *f_mangletab = NULL;

const char *curr_project = NULL;
static Dbi_index*dp;
static FILE *pqidsf;
static struct location8 l8;
static int indexing = 0;
static FILE *fldsf;
static int cache_size = 16;

const char *curr_index = "tok";

static void
process_cdata(Uchar*cdata)
{
  dbi_add(dp,(unsigned char*)buf,&l8,1);
}

indexit()
{
  wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
}

int
main(int argc, char * const*argv)
{
  options(argc,argv,"p:");

  if (!curr_project)
    usage();

  setlocale(LC_ALL,ORACC_LOCALE);

  index_dir = se_dir (curr_project, curr_index);

  progress ("indexing %s ...\n", index_dir);
  indexed_mm = init_mm (sizeof (struct indexed), 256);
  parallels_mm = init_mm (sizeof (struct parallel), 256);
  grapheme_mm = init_mm (sizeof (struct grapheme), 256);
  node_mm = init_mm (sizeof (struct node), 256);

  dip = dbi_create (curr_index, index_dir, 10000, /* hash_create will adjust */
		    sizeof(struct location16), DBI_ACCRETE);

  dbi_set_user(dip,d_txt);
  if (NULL == dip) 
    error (NULL, "unable to create index for %s", curr_index);
  if (cache_elements > 0)
    dbi_set_cache (dip, cache_elements);

  dp = dbi_create("tok",
		  "tok",
		  500000,
		  sizeof(struct location8),
		  DBI_ACCRETE);
  dbi_set_cache(dp,cache_size);
  dbi_set_user(dp,d_cat);

  /* index toks here */

  dbi_flush(dp);
  dbi_free(dp);

  ce_cfg(curr_project,"tok","tr","txh",ce_byid, proxies);

  return 0;
}

int
opts(int argc, char *arg)
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

const char *prog = "segdfx";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-p [project] [-s]";
void
help ()
{
  printf("  -p [project] Gives the name of the project; required\n");
}
