#include <stdio.h>
#include <stdlib.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include <dbxlib.h>

const char *file = NULL;
int lnum = 0;
const char *outfile = NULL;
int test_mode = 0;
int value_only = 0;

void
dbx_one(Dbi_index *d, const char *k)
{
  const char *v = dbx_key(d,k);
  if (v)
    {
      if (test_mode)
	puts("1");
      else if (outfile)
	{
	  FILE *o = fopen(outfile, "w");
	  if (o)
	    {
	      while (*v)
		{
		  if (' ' == *v)
		    fputc('\n', o);
		  else
		    fputc(*v, o);
		  ++v;
		}
	      fclose(o);
	    }	  
	  else
	    {
	      perror(outfile);
	      exit(1);
	    }
	}
      else if (value_only)
	puts(v);
      else
	printf("%s\t%s\n", k, v);
    }
  else
    {
      if (test_mode)
	puts("0");
      else if (value_only)
	puts("");
      else
	printf("%s\t\n", k);
    }
}

/* should reimplement less lazily to support arbitrary length keys */
void
dbx_input(Dbi_index *d, FILE *fp)
{
  char keybuf[256], *k;
  lnum = 0;
  while ((k = fgets(keybuf, 256, stdin)))
    {
      k[strlen((ccp)k)-1] = '\0';
      ++lnum;
      if (*k)
	{
	  if (*k == 0x04) {
	    break;
	  } else {
	    dbx_one(d, k);
	  }
	}
    }
}

int
main(int argc, char **argv)
{
  Dbi_index *d = NULL;
  const char *key = NULL;

  program_values("dbx", 1, 0, "[-d DBNAME] [key]", NULL);

  setlocale(LC_ALL,ORACC_LOCALE);
  
  options(argc, argv, "d:f:n:o:tv");
  key = argv[optind];
  d = dbx_init(dbidir, dbiname);
  if (key)
    dbx_one(d, key);
  else if (file)
    {
      FILE *fp = fopen(file, "r");
      if (fp)
	dbx_input(d, fp);
      else
	fprintf(stderr, "dbx: unable to read from keys file %s\n", file);
    }
  else
    {
      file = "<stdin>";
      dbx_input(d, stdin);
    }
  dbx_term(d);
}

extern int verbose;

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      dbidir = arg;
      break;
    case 'f':
      file = arg;
      break;
    case 'n':
      dbiname = arg;
      break;
    case 'o':
      outfile = arg;
      break;
    case 't':
      test_mode = 1;
      break;
    case 'v':
      value_only = 1;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}
