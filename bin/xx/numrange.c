#include <oraccsys.h>

const char *wp;
char *pending_ws = NULL;
int last_n = -1, start = -1, comma = 0, pending = 0, this_n;

const char *
getword(void)
{
  static char word[1024];
  int ch;
  static int i = 0;
  while ((EOF != (ch = getchar())))
    {
      if (isspace(ch))
	{
	  if (i)
	    {
	      ungetc(ch, stdin);
	      word[i] = '\0';
	      i = 0;
	      return word;
	    }
	  else
	    {
	      i = 0;
	      word[0] = ch;
	      word[1] = '\0';
	      return word;
	    }
	}
      else
	{
	  if (i == 1023)
	    {
	      fprintf(stderr, "numrange: ridiculous length word; giving up\n");
	      exit(1);
	    }
	  word[i++] = ch;
	}
    }
  if (i)
    {
      word[i] = '\0';
      i = 0;
      return word;
    }
  return NULL;
}

void
put_ws(void)
{
  fputs(pending_ws, stdout);
  free(pending_ws);
  pending_ws = NULL;
}

void
putnum(int with_ws)
{
  if (start == -1)
    return;
  if (comma)
    fputs(", ", stdout);
  if (start == last_n)
    printf("%d", start);
  else
    printf("%d-%d", start, last_n);
  start = last_n = -1;
  pending = 0;
  comma = 1;
  if (pending_ws)
    {
      if (with_ws)
	{
	  put_ws();
	  comma = 0;
	}
      else
	{
	  free(pending_ws);
	  pending_ws = NULL;
	}
    }
}

int
main(int argc, char *const *argv)
{
  if (argv[1])
    freopen(argv[1], "r", stdin);

  while ((wp = getword()))
    {
      if (isdigit(*wp))
	{
	  const char *np = wp+1;
	  int range_n = -1;
	  while (*np && isdigit(*np))
	    ++np;
	  if ('-' == *np)
	    {
#if 1
	      this_n = atoi(wp);
	      range_n = atoi(++np);
	      np += strlen(np);
	    }
	  if (!*np)
#else
	      fprintf(stderr, "numrange: internal ranges not yet handled\n");
	    }
	  else if (!*np) /* number word */
#endif
	    {
	      this_n = atoi(wp);
	      if (last_n >= 0)
		{
		  if (this_n - last_n == 1)
		    last_n = range_n > 0 ? range_n : this_n;
		  else
		    {
		      putnum(0);
		      start = this_n;
		      if (range_n > 0)
			last_n = range_n;
		      else
			last_n = this_n;
		      pending = 1;
		      if (pending_ws)
			{
			  free(pending_ws);
			  pending_ws = NULL;
			}
		    }
		}
	      else
		{
		  start = last_n = this_n;
		  pending = 1;
		  if (pending_ws)
		    {
		      if (!comma)
			put_ws();
		      free(pending_ws);
		      pending_ws = NULL;
		    }
		}
	    }
	  else
	    {
	      if (pending)
		putnum(0);
	      fputs(wp, stdout);
	      comma = 0;
	    }
	}
      else
	{
	  if (isspace(*wp))
	    {
	      if (pending_ws)
		free(pending_ws);
	      pending_ws = strdup(wp);
	    }
	  else
	    {
	      if (pending)
		putnum(1);
	      else if (pending_ws)
		put_ws();
	      fputs(wp, stdout);
	      comma = 0;
	    }
	}
    }
  if (pending)
    putnum(1);
  else if (pending_ws)
    put_ws();
}
