#include <oraccsys.h>
#include "lx.h"

int check;
int inplace;
const char *output;
const char *prog;
const char *project;
const char *projfile;
int proxy;
int qualify;
int sort;
int uniq;	/* uniq does not require sort in lx */
int verbose;

int
main(int argc, char * const*argv)
{
  loadfile_prog((prog = argv[0]));
  
  if (options(argc, argv, "a:cio:p:qsuvx"))
    return 1;

  if (qualify && !project)
    {
      fprintf(stderr, "%s: -q specified without -p PROJECT. Stop.\n", argv[0]);
      return 1;
    }

  List *todo = NULL;
  
  if (argv[optind])
    {
      if (inplace)
	output = argv[optind];

      mesg_init();
      todo = lx_set_args(argv, optind);
    }
  else if (projfile)
    {
      todo = lx_set_args(lx_projfile(projfile), 0);
    }
  else
    {
      fprintf(stderr, "%s: nothing to do. Stop.\n", argv[0]);
      exit(1);
    }

  /* We output the errors detected during load-time first; if any
     were unrecoverable todo will be NULL otherwise we will go
     ahead and process the todo list anyway
     
     mesg_print frees the mesg list */
  mesg_print(stderr);
  
  Lxfile *lxf = NULL;
  if (todo)
    if ((lxf = lx_set_ops(todo)))
      lx_finish(lxf);
  
  if (lxf)
    {
      if (!check || output)
	lx_output(lxf, output);
      lx_free(lxf);
    }
  else
    fprintf(stderr, "%s: lx processing failed. Stop.\n", argv[0]);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'a':
      projfile = arg;
      break;
    case 'c':
      check = 1;
      break;
    case 'i':
      inplace = 1;
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
