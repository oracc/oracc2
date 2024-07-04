#include <oraccsys.h>
#include "qx.h"

static struct qxdata *opt_qp;

int
qx_options(int argc, char **argv, struct qxdata *qp)
{
  opt_qp = qp;
  qp->argv = (const char **)argv;
  if (argv[1] && '-' != argv[1][0])
    {
      if (qx_cgi_options(argc, argv, &qd))
	{
	  opt_qp = NULL;
	  return 1;
	}
      qp->argc = -1; /* flag value to show options were cgi-opts  */
    }
  else
    {
      if (options(argc, argv, "8acC:dg:i:j:o:p:P:q:stuvx:Ww"))
	{
	  opt_qp = NULL;
	  return 1;
	}
      qp->argc = optind;
    }
  return 0;
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case '8':
      use_unicode = 1;
      break;
    case 'a':
      any_index = opt_qp->any_index = 1;
      break;
    case 'c':
      show_count = 1;
      break;
    case 'C':
      opt_qp->cemd = arg;
      break;
    case 'd':
      doing_debug = 1;
      opt_qp->debug = 1;
      break;
    case 'e':
      errfile = arg;
      break;
    case 'g':
      opt_qp->glos = arg;
      break;
    case 'i':
      arg_index = arg;
      break;
    case 'j':
      set_project_arg(project = arg);
      opt_qp->project = arg;
      break;
    case 'o':
      outfile = arg;
      out_f = xfopen(outfile,"w");     
      break;
    case 'p':
      pretrim_file = (char*) arg; /* etcsri:Q000834 or etcsri:Q000834.2 */
      break;
    case 'P':
      if (!pretrim_args)
	pretrim_args = list_create(LIST_SINGLE);
      list_add(pretrim_args, (void*)arg);
      break;
    case 'q':
      opt_qp->srch = CGI_decode_url(arg);
      break;
    case 's':
      s2 = 1;
      break;
    case 't':
      show_tokens = 1;
      break;
    case 'u':
      do_uniq = 0;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      xmldir = arg;
      break;
    case 'W':
      opt_qp->noexec = 1;
    case 'w':
      opt_qp->web = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "qx";
int major_version = 1, minor_version = 0;
const char *usage_string = " #<PROJ> !<IDX> %<LNG> [search terms]";
void
help ()
{
  printf("  -c = show count of results\n");
  printf("  -d = output debugging information\n");
  printf("  -t = show tokens and exit\n");
  printf("  -u = do not uniq results\n");
  printf("  -v = verbose operation\n");
}

extern const char *file; /* for libatf */
extern int lnum; /* for libatf */
