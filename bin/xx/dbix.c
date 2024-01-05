#include <stdio.h>
#include <stdlib.h>
#include <c2types.h>
#include <oraccsys.h>
#include <dbi.h>

#ifdef HELP
#include "help.h"
#endif

const char *dbidir, *dbiname;
int stdinput = 0;

int
main(int argc, char **argv)
{
  unsigned char *k;
  
  const char *tsvfile = NULL;
  Dbi_index *dbi = NULL;
  FILE *infile;
  size_t nbytes;

  options(argc, argv, "d:n:s?");
  tsvfile = argv[optind];

  if (!tsvfile && !stdinput)
    {
      fprintf(stderr, "dbix: must give a tabbed data file for indexing or use -s flag\n");
      exit(1);
    }
  else if (!dbidir || !dbiname)
    {
      fprintf(stderr, "dbix: [-s] -d DIR -n NAME required for DBI creation\n");
      exit(1);
    }

  if (stdinput)
    infile = stdin;
  else if (!(infile = fopen(tsvfile, "r")))
    {
      fprintf(stderr, "dbix: unable to read key/values from %s\n", tsvfile);
      exit(1);
    }

  dbi = dbi_create(dbiname, dbidir, 0, 1, DBI_BALK);
  while ((k = loadoneline(infile,&nbytes)))
    {
      unsigned char *v = (unsigned char*)strchr((char*)k,'\t');
      if (v)
	{
	  *v++ = '\0';
	  dbi_add(dbi,k,v,strlen((char*)v)+1);
	}
      else if ('#' != *k)
	{
	  fprintf(stderr, "dbix: no tab in line `%s'\n", k);
	  exit(1);
	}
    }
  dbi_flush(dbi);
  return 0;
}

const char *prog = "dbix";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "[ -b | -p [PROJECT] ]";

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'd':
      dbidir = arg;
      break;
    case 'n':
      dbiname = arg;
      break;
    case 's':
      stdinput = 1;
      break;
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void help (void)
{
}
