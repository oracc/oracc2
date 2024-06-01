#include <oraccsys.h>
#include "lx.h"

int check;
const char *output;
const char *project;
int proxy;
int qualify;
int sort;
int uniq;	/* uniq does not require sort in lx */
int verbose;

int
main(int argc, char * const*argv)
{
  loadfile_prog(argv[0]);
  
  if (options(argc, argv, "co:p:qsuv"))
    return 1;
  if (qualify && !project)
    {
      fprintf(stderr, "%s: -q specified without -p PROJECT. Stop.\n", argv[0]);
      return 1;
    }

  if (argv[optind])
    {
      Lxfile *lxf = NULL;

      mesg_init();
      List *todo = lx_set_args(argv);

      /* We output the errors detected during load-time first; if any
	 were unrecoverable todo will be NULL otherwise we will go
	 ahead and process the todo list anyway

	 mesg_print frees the mesg list */
      mesg_print(stderr);

      if (todo)
	if ((lxf = lx_set_ops(todo)))
	  lx_finish(lxf);
      
      if (lxf && !check)
	{
	  lx_output(lxf, output);
	  lx_free(lxf);
	}
      else
	fprintf(stderr, "%s: lx processing failed. Stop.\n", argv[0]);
    }
  else
    {
      fprintf(stderr, "%s: nothing to do. Stop.\n", argv[0]);
      exit(1);
    }
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'c':
      check = 1;
      break;
    case 'o':
      output = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'q':
      qualify = 1;
      break;
    case 's':
      sort = 1;
      break;
    case 'u':
      uniq = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      proxy = 1;
      break;
    default:
      return 1;  
    }
  return 0;
}

void help(void) { fprintf(stderr, "lx: use -c to check, -s to show, files on command line\n"); }
