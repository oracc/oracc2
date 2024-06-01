#include <oraccsys.h>
#include "lx.h"

int check;
const char *output;
const char *project;
int qualify;
int sort;
int uniq;	/* uniq does not require sort in lx */
int verbose;

int
main(int argc, char **argv)
{
  if (options(argc, argv, "co:p:qsuv"))
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
      /* mesg_print frees the mesg list */
      mesg_print(stderr);
      if (lxf)
	{
	  if (!check || output)
	    {
	      if (sort)
		lx_sort(lxf);
	      if (output)
		{
		  FILE *outfp = stdout;
		  if (output && strcmp(output, "-"))
		    {
		      if (!(outfp = fopen(output, "w")))
			{
			  fprintf(stderr, "failed to open output %s: %s\n", output, strerror(errno));
			  exit(1);
			}
		    }
		  lx_output(lxf, outfp);
		}
	    }
	  lx_free(lxf);
	}
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
    default:
      return 1;  
    }
  return 0;
}

void help(void) { fprintf(stderr, "lx: use -c to check, -s to show, files on command line\n"); }
