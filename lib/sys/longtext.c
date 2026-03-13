#include <oraccsys.h>

static const char *sep = "\n\t";

void
longtext_init(Pool *p,char *s)
{
  (void)longtext_sep(NULL,NULL,NULL,NULL);
  (void)longtext_sep(p, s, NULL, NULL);
}

void
longtext_set_sep(const char *s)
{
  sep = s;
}

char *
longtext_sep(Pool *p, char *t, char *a, const char *sep)
{
  static char *n;
  char *ret = NULL;
  if (n && !a)
    {
      /* after longtext_init n is set but on first invocation a is not
	 set so we have to reset the args to pretend the text in t is
	 actually additional, i.e., supplied in a */
      a = t;
      t = NULL;
    }
  if (a)
    {
      char *n2;
      while ('\t' == *a || ' ' == *a)
	++a;
      n2 = (char*)pool_alloc(strlen(n)+strlen(a)+strlen(sep)+1, p);
      strcpy(n2,n);
      strcat(n2,sep);
      strcat(n2, a);
      ret = n = n2;
    }
  else if (t)
    ret = n = (char*)pool_copy((uccp)t, p);
  else
    {
      ret = n;
      n = NULL;
    }
  return ret;
}

/**If t and a are both NULL return n; else if a is NULL initialize to t; else append a to t.
 */
char *
longtext(Pool *p, char *t, char *a)
{
  return longtext_sep(p, t, a, sep);
}
