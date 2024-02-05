#include <oraccsys.h>

char **
space_split(char *l)
{
  int ncol = 1;
  char *s = l;
  while (*s)
    {
      if (isspace(*s))
	{
	  ++ncol;
	  while (isspace(*s))
	    ++s;
	}
      else
	++s;
    }
  if ('\n' == s[-1])
    s[-1] = '\0';

  char **p = calloc(ncol+1, sizeof(char *));
  int col;
  for (col = 0, s = l; *s; ++col)
    {
      p[col] = s;
      while (*s && !isspace(*s))
	++s;
      if (isspace(*s))
	{
	  *s++ = '\0';
	  while (isspace(*s))
	    ++s;
	}
    }

  return p;
}
