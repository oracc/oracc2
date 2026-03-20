#include <oraccsys.h>

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

int
main(int argc, char *const *argv)
{
  const char *wp;
  int last_n = -1, start = -1, comma = 0, pending = 0;

  if (argv[1])
    freopen(argv[1], "r", stdin);
  
  while ((wp = getword()))
    {
      if (isdigit(*wp))
	{
	  const char *np = wp+1;
	  while (*np && isdigit(*np))
	    ++np;
	  if ('-' == *np)
	    fprintf(stderr, "numrange: internal ranges not yet handled\n");
	  else if (!*np) /* number word */
	    {
	      int this_n = atoi(wp);
	      if (last_n >= 0)
		{
		  if (this_n - last_n == 1)
		    last_n = this_n;
		  else
		    {
		      if (comma)
			fputs(", ", stdout);
		      printf("%d-%d", start, last_n);
		      last_n = this_n;
		      comma = pending = 1;
		      start = last_n = this_n;
		    }
		}
	      else
		{
		  start = last_n = this_n;
		  pending = 1;
		}
	    }
	  else
	    {
	      if (pending)
		{
		  if (comma)
		    fputs(", ", stdout);
		  printf("%d-%d", start, last_n);
		  start = last_n = -1;
		  comma = 1;
		  pending = 0;
		}
	      fputs(wp, stdout);
	      if ('\n' == *wp)
		comma = 0;
	    }
	}
      else
	{
	  if (isspace(*wp))
	    {
#if 1
	      if (!pending)
		fputs(wp, stdout);
#else
	      if (start != last_n)
		printf("%d-%d", start, last_n);
	      else
		printf("%d", start);
	      pending = 0;
	      start = last_n = -1;
#endif
	    }
	  else
	    fputs(wp, stdout);
	}
    }
}
