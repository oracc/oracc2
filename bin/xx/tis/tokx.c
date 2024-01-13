#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

const char *projproj = NULL;
int run_multi = 0;
int stdinput = 0;
int tok_input_cbd = 0, tok_input_xtf = 1;
int tok_data_g = 1, tok_data_l = 0, tok_data_m = 1;

static void
tokx_sH(void *userData, const char *name, const char **atts)
{
}

static void
tokx_eH(void *userData, const char *name)
{
}

static void
tokx_cbd(const char *summaries)
{
  const char *fname[2] = { summaries , NULL };
  /*fprintf(tab, "F\t%s\n", fname[0]);*/
  runexpat(i_list, fname, tokx_sH, tokx_eH);
}

static void
tokx_one(const char *QPQX)
{
  char *fname[2];
  char *dot;
  if ((dot = strchr(QPQX,'.')))
    *dot = '\0';
  fname[0] = expand(NULL, QPQX, "xtf");
  fname[1] = NULL;
  /*fprintf(tab, "F\t%s\n", fname[0]);*/
  runexpat(i_list, fname, tokx_sH, tokx_eH);
}

static void
tokx_stdin(void)
{
  runexpat(i_stdin, NULL, tokx_sH, tokx_eH);
}

void
tokx_input(const char *arginput)
{
  if (tok_input_cbd && (tok_data_l || tok_data_m))
    {
      fprintf(stderr, "tokx: -c is not compatible with -l or -m. Stop.\n");
      exit(1);
    }
  if (arginput)
    {
      if (tok_input_xtf)
	tokx_one(arginput);
      else
	tokx_cbd(arginput);
    }
  else if (stdinput)
    {
      /* This means read the XML data in .xtf format from stdin */
      tokx_stdin();
    }
  else
    {
      /* This means read a list of qualifed PQX entries from stdin and
	 process each one */
      static char pqx[QUALIFIED_PQX_MAX];
      while (fgets(pqx,QUALIFIED_PQX_MAX,stdin))
	{
	  pqx[strlen(pqx)-1] = '\0';
	  tokx_one(pqx);
	}
    }
}

int
main(int argc, char **argv)
{
  Trun *r = NULL;
  
  if (options(argc, argv, "cfp:s"))
    exit(1);

  r = trun_init(run_multi);
  tlb_init(r, projproj, tok_input_xtf ? "xtf" : "cbd");

  /* tlw_init(r, stdout); */

#if 0
  if (tok_input_xtf)
    loc = loc_xtf;
  else
    loc = loc_cbd;
#endif
  
  tokx_input(argv[optind]);
  
  trun_term(r);
  
  return 0;
}

const char *prog = "tokx";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "tokx";
void help () { }
int opts(int arg,char*str)
{
  switch (arg)
    {
    case '+':
      run_multi = 1;
      break;
    case 'c':
      tok_input_xtf = 0;
      tok_input_cbd = 1;
      tok_data_g = 1;
      tok_data_l = tok_data_m = 0;
      break;
    case 'g':
      tok_data_g = 1;
      break;
    case 'l':
      tok_data_l = 1;
      break;
    case 'm':
      tok_data_m = 1;
      break;
    case 'p':
      projproj = str;
      break;
    case 's':
      stdinput = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

