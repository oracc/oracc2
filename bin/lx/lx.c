#include <oraccsys.h>
#include "lx.h"

int check;
int output;
const char *project;
int qualify;
int sort;
int uniq;
int verbose;

int
main(int argc, char **argv)
{
  if (options(argc, argv, "cop:qsuv"))
    return 1;
  if (qualify && !project)
    {
      fprintf(stderr, "%s: -q specified without -p PROJECT. Stop.\n", argv[0]);
      return 1;
    }
      
  while (argv[optind])
    {
      mesg_init();
      Lxfile *lxf = lx_load(argv[optind++]);
      mesg_print(stderr);
      if (!check)
	{
	  if (sort)
	    lx_sort(lxf);
	  if (output)
	    lx_output(lxf, stdout);
	}
      lx_free(lxf);
      mesg_term();
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
      output = 1;
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
    default:
      return 1;  
    }
  return 0;
}

void help(void) { fprintf(stderr, "lx: use -c to check, -s to show, files on command line\n"); }
