#include <oraccsys.h>
#include "wx.h"

void
dump_env(FILE *fp)
{
  extern char **environ;
  char **s = environ, **e;

  int i = 0;
  for (e = s; *e; ++e)
    ++i;

  qsort(s, i, sizeof(char **), cmpstringp);
  
  for (; *s; s++)
    {
      if (1 || !strncmp(*s, "HTTP", 4))
	{
	  if (!strncmp(*s, "HTTP_COOKIE", strlen("HTTP_COOKIE")))
	    /*isp_show_cookies(fp, *s)*/;
	  else
	    fprintf(fp, "%s\n", *s);
	}
    }
}

void
dump_vec(const char *porq, char * const*vec)
{
  int i;
  fprintf(stderr, "wx calling %s", porq);
  for (i = 0; vec[i]; ++i)
    fprintf(stderr, " %s", vec[i]);
  fputc('\n', stderr);
}

void
dump_vl(CGI_varlist *vl)
{
  const char *name;
  CGI_value  *value;

  fprintf(stderr, "====dumping CGI value list====\n");

  for (name = CGI_first_name(vl); name != 0;
       name = CGI_next_name(vl))
    {
      int i;
      value = CGI_lookup_all(vl, 0);
      for (i = 0; value[i] != 0; i++) 
	fprintf(stderr, "%s [%d] >>%s<<\n", name, i, value[i]);
    }

  fprintf(stderr, "====done=====\n");
}
