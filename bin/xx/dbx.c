#include <stdio.h>
#include <stdlib.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include <dbxlib.h>

const char *file = NULL;
int lnum = 0;

void
dbx_one(Dbi_index *d, const char *k)
{
  const char *v = dbx_key(d,k);
  if (!v)
    v = "";
  printf("%s\t%s\n", k, v);
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
  
  options(argc, argv, "d:f:n:");
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
opts(int argc, char *arg)
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
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}
