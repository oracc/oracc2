#include <oraccsys.h>
#include "lx.h"

int check;
int show;

int
main(int argc, char **argv)
{
  options(argc, argv, "cs");
  if (check)
    {
      while (argv[optind])
	lx_check(argv[optind++]);
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
    case 's':
      show = 1;
      break;
    default:
      return 1;  
    }
  return 0;
}

void help(void) { fprintf(stderr, "lx: use -c to check, -s to show, files on command line\n"); }
