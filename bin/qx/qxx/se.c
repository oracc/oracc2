#include <oraccsys.h>
#include <locale.h>
#include <atf2utf.h>
#include <charsets.h>
#include "types.h"
#include "se.h"
#include "selib.h"

#include "qx.h"
#include "px.h"

FILE *f_log = NULL;
int any_index = 0;
int do_uniq = 1;
int doing_debug = 0;
int l2 = 1;
int p4 = 0;
int s2 = 1;
int show_count = 0;
int show_tokens = 0;
int textresult = 0;
int verbose = 0;
const char *xmldir = NULL;
const char *errfile = NULL;
const char *outfile = NULL;
const char *arg_index = NULL;
const char *project = NULL;
const char *return_index = NULL;
FILE*out_f = NULL,  *f_err;
extern FILE *f_log;
enum result_granularity res_gran = g_not_set, best_res_gran = g_not_set;
char *pretrim_file = NULL;
unsigned char **pretrim_lines, *pretrim_content;
List *pretrim_args = NULL;
Hash *pretrim = NULL;
FILE *f_mangletab = NULL;

const char *curr_project;

int
main(int argc, char **argv)
{
  mesg_init();

  f_log = stderr;
  if (errfile)
    {
      if (!(f_err = freopen(errfile, "w", stderr)))
	{
	  fprintf(stderr, "se: unable to reopen stderr to write to %s\n", errfile);
	  exit(1);
	}
    }
  else
    f_err = stderr;

  setlocale(LC_ALL,ORACC_LOCALE);

  qd.p = sdata.p = pool_init();

  if (qx_options(argc, argv, &qd))
    {
      qx_error(&qd, &sdata);
      exit(0);
    }
  
  if (!out_f)
    out_f = stdout;

  if (qd.web)
    {
      if (qx(&qd))
	{
	  qx_error(&qd, &sdata);
	  exit(0);
	}
    }
  else
    se_run(&qd, &sdata);
  
  return 0;
}
