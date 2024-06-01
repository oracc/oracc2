#include <oraccsys.h>
#include "lx.h"

char **
lx_loadfile(const char *fname, size_t *nlines, char **fmem)
{
  size_t n = -1;
  char *f = NULL;
  char **lp = NULL;

  if (!strcmp(fname, "-"))
    f = (char*)loadstdin(&n);
  else
    f = (char*)loadfile((uccp)fname,&n);
  
  if (f)
    {
      *fmem = f;
      if (strlen(f))
	{
	  size_t i, j, l;
	  for (i = l = 0; i < n; ++i)
	    {
	      if ('\n' == f[i] || ('\r' == f[i] && '\n' != f[i+1]))
		++l;
	    }
	  /* catch missing final newline */
	  if ('\n' != f[n-1])
	    ++l;
	  lp = malloc((l+1) * sizeof(char *));
	  for (i = j = 0; i < n; ++i)
	    {
	      lp[j++] = &f[i];
	      while (i < n && f[i] && f[i] != '\r' && f[i] != '\n')
		++i;
	      if (f[i])
		f[i] = '\0';
	    }
	  lp[j] = NULL;
	  *nlines = l;
	}
    }
  return lp;
}
