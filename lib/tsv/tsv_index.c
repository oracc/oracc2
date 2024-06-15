#include <oraccsys.h>
#include "tsv.h"

static int tdb_line(Tsv *tp);

int
tsv_index(const char *tsv_file, const char *dir, const char *name)
{
  Tsv *tp = tsv_create();
  tsv_paths(tp, tsv_file, name, dir);
  if (!strcmp(tp->tsv_fn, "-"))
    tp->tsv_fp = stdin;
  else
    {
      if (!(tp->tsv_fp = fopen(tp->tsv_fn, "r")))
	{
	  /*fprintf(stderr, "xisdb: failed to open %s\n", tis_file);*/
	  tsv_destroy(tp);
	  return 2;
	}
    }
	  
  if ((tp->dp = dbi_create (tp->base, tp->tdb_dir, 1024, sizeof(Tsv_data), DBI_BALK)))
    {
      do
	tdb_line(tp);
      while (!feof(tp->tsv_fp));
      dbi_flush(tp->dp);
    }
  else
    {
      fclose (tp->tsv_fp);
      /*fprintf(stderr, "xisdb: failed to create %s/%s.dbh\n", dir, name);*/
      tsv_destroy(tp);
      return 1;
    }

  tsv_destroy(tp);
  return 0;
}

static int
tdb_line(Tsv *tp)
{
  int ch, len = 0;
  int s = 0, n = -1, l = 0;
  static char *buf = NULL;
  static int bufsiz = 0;
  int key = 1;
#if 0
  static int tis_line = 0;
#endif
  
  if (!tp)
    {
      if (buf)
	{
	  free(buf);
	  buf = NULL;
	  bufsiz = 0;
	}
    }
  else if (!buf)
    {
      buf = malloc(1024);
      bufsiz = 1024;
    }

#if 0
  ++tdb_line;
#endif

  while (EOF != (ch = fgetc(tp->tsv_fp)))
    {  
      if ('\n' == ch)
	{
	  ++l;
	  break;
	}
      else if ('\t' == ch && key)
	{
	  if (len >= 0)
	    {
	      buf[len] = '\0';
	      len = -1;
	    }
	  s = ftell(tp->tsv_fp);
	  key = n = l = 0;
	}
      else if (' ' == ch)
	{
	  ++n;
	  ++l;
	}
      else
	{
	  if (len >= 0)
	    {
	      while (len >= bufsiz)
		{
		  bufsiz *= 2;
		  buf = realloc(buf, bufsiz);
		}
	      buf[len++] = ch;
	    }
	  else
	    ++l;
	}
    }
  if (l > 0)
    {
      if (!tp->count_items)
	n = -1;
      tdb_add(tp, buf, s, l, n);
    }

  return 0;
}

