#include <oraccsys.h>
#include "lx.h"

char *const *
lx_projfile(const char *fn)
{
  int umbrella = (strstr(fn, "umbrella") ? 1 : 0);
  static char *f = NULL;

  if (umbrella && !project)
    {
      fprintf(stderr, "%s: must give -p PROJECT when processing umbrella.lst\n", prog);
      return NULL;
    }
  if (fn)
    {
      f = (char*)loadfile((uccp)fn, NULL);
      if (f)
	{
	  char *tok, *t = f;
	  List *l = list_create(LIST_SINGLE);
	  while ((tok = strtok(t, "\n\r\t ")))
	    {
	      list_add(l, tok);
	      if (t)
		t = NULL;
	    }
	  char **c = calloc(2*list_len(l), sizeof(char*));
	  int i = 0;
	  for (t = list_first(l); t; t = list_next(l))
	    {
	      if (umbrella)
		c[i++] = lx_atf_data(t, project);
	      else
		c[i++] = lx_atf_data(t, "");
	      c[i++] = "+";
	    }
	  c[i-1] = NULL;
	  list_free(l, NULL);
	  return c;
	}
    }
  return NULL;
}
