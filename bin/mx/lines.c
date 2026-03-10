#include <oraccsys.h>
#include "matrix.h"

static Hash *hhaves;
static Hash *hwants;
static Pool *plines;

static void
lines_miss(void)
{
  size_t w;
  const char **want = hash_keys(hwants, &w);
  qsort(want, w, sizeof(const char *), cmpdigitp);
  int i;
  for (i = 0; want[i]; ++i)
    {
      Hash *haves = hash_find(hhaves, want[i]);
      if (haves)
	{
	  List *wants = hash_find(hwants, want[i]);
	  List *missing = list_create(LIST_SINGLE);
	  const char *sig;
	  for (sig = list_first(wants); sig; sig = list_next(wants))
	    if (!hash_find(haves, sig))
	      list_add(missing, sig);
	  if (list_len(missing))
	    {
	      int n;
	      const char **cc = list2array_c(missing, &n);
	      qsort(cc, n, sizeof(const char *), cmpstringp);
	      char *s = vec_to_str(cc);
	      fprintf(stderr, "line %s missing %s\n", want[i], s);
	      free(c);
	      free(s);
	    }
	}
      else
	fprintf(stderr, "wanted line %s has no exemplars\n", want[i]);
    }      
}

void
lines_have(char *l, char *s)
{
  Hash *hp = hash_find(hhaves, bufp);
  if (!hp)
    hash_add(hhaves, l, (hp=hash_create(10)));
  hash_add(hp, s, "");
}

static void
lines_want(char *s, int start, int end)
{
  char buf[strlen(s)+10], *bufp;
  strcpy(buf, s);
  strcat(buf, ":");
  bufp = buf + strlen(buf);
  ++end;
  while (start < end)
    {
#if 0
      sprintf(bufp, "%d", start++);
#endif
      List *lp = hash_find(hwants, bufp);
      if (!lp)
	hash_add(hwants, pool_copy(bufp, plines), (lp=list_create(LIST_SINGLE)));
      list_add(lp, pool_copy(buf, plines));
    }
}

static void
lines_expand(char *lp)
{
  static int lno = 0;
  ++lno;
  char *siglum = lp;
  lp = strchr(sig, '\t');
  if (lp)
    {
      *lp++ = '\0';
      while (*lp)
	{
	  while (!isdigit(*lp))
	    ++lp;
	  char *start = lp, *end = lp;
	  while (isdigit(*lp))
	    ++lp;
	  if ('-' == *lp)
	    {
	      ++lp;
	      end = lp;
	      while (isdigit(*lp))
		++lp;
	    }
	  lines_register(siglum, atoi(start), atoi(end));
	}
    }
  else
    fprintf(stderr, "%s: syntax error: no <TAB> in line\n", lno);
}

void
lines_init(void)
{
  hhaves = hash_init(256);
  hwants = hash_init(256);
  plines = hpool_init();
}

int
lines_mode(const char *wants_list)
{
  char *lp;
  size_t n;
  FILE *wp = xfopen(wants_list, "r");
  if (wp)
    {
      while ((lp = loadoneline(stdin, &n)))
	lines_expand(lp);
      lines_miss();
    }
  else
    fprintf(stderr, "mx: failed to open wants list %s. Stop.\n", wants_list);
}
