#include <oraccsys.h>

char **
tab_split(char *l)
{
  int ncol = 1;
  char *s = l;
  while (*s)
    {
      if ('\t' == *s)
	++ncol;
      ++s;
    }
  if ('\n' == s[-1])
    s[-1] = '\0';

  char **p = calloc(ncol+1, sizeof(char *));
  int col;
  for (col = 0, s = l; *s; ++col)
    {
      if ('\t' == *s)
	p[col] = "";
      else
	p[col] = s;
      while (*s && '\t' != *s)
	++s;
      if ('\t' == *s)
	*s++ = '\0';
    }

  return p;
}
