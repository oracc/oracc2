#include <oraccsys.h>
#include <oracclocale.h>
#include <ose.h>
#include <dbxlib.h>

/* These count from 1 so to get items 0..9 you need -F1 -T10*/
int dbx_from = 0, dbx_to = 0;

const char *file = NULL;
int lnum = 0;
const char *outfile = NULL;
int test_mode = 0;
int value_only = 0;
int word_ids = 0;
char dbx_sep_char = '\n';
const char *dbidir, *dbiname;

void
dbx_val(Dbi_index *d, const void *v, size_t n, FILE *o)
{
  switch (d->h.data_type)
    {
    case DBI_DT_UNDEF:
    case DBI_DT_CHARP:
      fprintf(o, "%s%c", (ccp)v, dbx_sep_char);
      break;
    case DBI_DT_LOC8:
      if (word_ids)
	dbx_wids(d, (Loc8*)v, n, o);
      else
	dbx_l8s(d, (Loc8*)v, n, o);
      break;
    default:
      fprintf(stderr, "dbx_val: unknown DBI type code %d\n", d->h.data_type);
      break;
    }
}

void
dbx_one(Dbi_index *d, const char *k, FILE *o)
{
  Unsigned32 n;
  const void *v = dbx_key(d,k,&n);
  
  if (dbx_from)
    v = dbi_slice(d,dbx_from-1,dbx_to-1,&n);
  
  if (v)
    {
      if (test_mode)
	fputs("1\n", o);
      else
	dbx_val(d, v, n, o);
    }
  else
    {
      if (test_mode)
	fputs("0\n", o);
      else if (value_only)
	fputc('\n', o);
    }
}

/* should reimplement less lazily to support arbitrary length keys */
void
dbx_input(Dbi_index *d, FILE *fp, FILE *o)
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
	    fprintf(o, "%s\t\n", k);
	    dbx_one(d, k, o);
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
  
  options(argc, argv, "d:f:F:n:o:stT:vw");
  key = argv[optind];
  d = dbx_init(dbidir, dbiname);
  d->h.sep_char = dbx_sep_char;
  FILE *o = stdout;
  if (outfile)
    {
      o = fopen(outfile, "w");
      if (!o)
	{
	  perror(outfile);
	  exit(1);
	}
    }
  if (key)
    {
      if (!word_ids)
	fprintf(o, "%s\t", key);
      dbx_one(d, key, o);
    }
  else if (file)
    {
      FILE *fp = fopen(file, "r");
      if (fp)
	dbx_input(d, fp, o);
      else
	fprintf(stderr, "dbx: unable to read from keys file %s\n", file);
    }
  else
    {
      file = "<stdin>";
      dbx_input(d, stdin, o);
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
    case 'F':
      dbx_from = atoi(arg);
      break;
    case 'n':
      dbiname = arg;
      break;
    case 'o':
      outfile = arg;
      break;
    case 's':
      dbx_sep_char = ' ';
      break;
    case 't':
      test_mode = 1;
      break;
    case 'T':
      dbx_to = atoi(arg);
      break;
    case 'v':
      value_only = 1;
      break;
    case 'w':
      word_ids = 1;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}
