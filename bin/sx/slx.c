#include <stdio.h>
#include <stdlib.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include <sll.h>
#include <gvl.h>

const char *file = NULL;
int lnum = 0;
char *key = NULL;
const char *name = "sl";
const char *project = "ogsl";

Dbi_index *d = NULL;
Hash *h = NULL;

int dbi_mode = 0;
int error_mode = 0;
int fast_mode = 0;
int tsv_mode = 1;
int verbose = 0;

static void
slx_handler()
{
  int done = 0;
  if (fast_mode)
    {
      unsigned char *v = NULL;
      if (dbi_mode)
	v = sll_lookup_d(d,(uccp)key);
      else
	v = sll_lookup_t((uccp)key);
      if (v)
	{
	  printf("%s\n", v);
	  done = 1;
	}
    }
  if (!done)
    {
      /*sll_cli_error_token = error_mode;*/
      sll_cli_handler((uccp)key);
    }
  fflush(stdout);
}

int
main(int argc, char **argv)
{
  program_values("slx", 1, 0, "[-p PROJECT] [key]", NULL);

  setlocale(LC_ALL,ORACC_LOCALE);
  
  options(argc, argv, "C:E:G:P:k:dfn:p:Rrtv");

  if (wcaller)
    {
      sll_web_handler(wcaller, wproject, wgrapheme, wextension);
      /* sll_web_handler should exit in normal circumstances */
      fprintf(stderr, "slx: strange--sll_web_handler did not exit\n");
      return 1;
    }
  else
    {
      if (dbi_mode)
	{
	  d = sll_init_d(project, name);
	  sll_cli_voidsl();
	}
      else
	{
	  if (!(h = sll_init_t(project, name)))
	    {
	      fprintf(stderr, "slx: failed to load tsv sl for project %s/name %s\n", project, name);
	      exit(1);
	    }
	  else
	    gvl_quick_setup(project, h);
	}

      if (key)
	slx_handler();
      else
	{
	  char keybuf[256];
	  file = "<stdin>";
	  lnum = 0;
	  if (verbose)
	    fprintf(stderr, "slx waiting for input ...\n");
	  while ((key = fgets(keybuf, 256, stdin)))
	    {
	      key[strlen((ccp)key)-1] = '\0';
	      ++lnum;
	      if (verbose)
		fprintf(stderr, "slx got [%d] %s\n", lnum, key);
	      if (*key)
		{
		  if (*key == 0x04 || !strlen(key)) {
		    break;
		  } else {
		    slx_handler();
		  }
		}
	    }
	}
    }  
  if (h)
    sll_term_t(h);
  else
    sll_term_d(d);
  
  return 0;
}

extern int verbose;

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'C':
      wcaller = arg;
      break;
    case 'E':
      wextension = arg;
      break;
    case 'G':
      wgrapheme = arg;
      break;
    case 'P':
      wproject = arg;
      break;
    case 'd':
      dbi_mode = 1;
      break;
    case 'e':
      error_mode = 1;
      break;
    case 'f':
      fast_mode = 1;
      break;
    case 'k':
      key = arg;
      break;
    case 'n':
      name = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'R':
      sll_raw_output = 2; /* only output the value part of the lookup */
      break;
    case 'r':
      sll_raw_output = 1;
      break;
    case 't':
      sll_trace = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}
