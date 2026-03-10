#include <oraccsys.h>
#include <hash.h>
#include "matrix.h"

static Hash *hhaves;
static Hash *hwants;
static Pool *plines;

static int lines_trace = 0;

static void
lines_miss(void)
{
  int w;
  const char **want = hash_keys2(hwants, &w);
  qsort(want, w, sizeof(const char *), cmpdigitp);
  int i;
  for (i = 0; want[i]; ++i)
    {
      Hash *haves = hash_find(hhaves, (uccp)want[i]);
      if (haves)
	{
	  List *wants = hash_find(hwants, (uccp)want[i]);
	  List *missing = list_create(LIST_SINGLE);
	  const char *sig;
	  for (sig = list_first(wants); sig; sig = list_next(wants))
	    if (!hash_find(haves, (uccp)sig))
	      list_add(missing, (void*)sig);
	  if (list_len(missing))
	    {
	      int n;
	      char **cc = (char **)list2array_c(missing, &n);
	      qsort(cc, n, sizeof(const char *), cmpstringp);
	      char *s = vec_to_str(cc, n, " ");
	      fprintf(stderr, "line %s missing %s\n", want[i], s);
	      free(cc);
	      free(s);
	    }
	  list_free(missing, NULL);
	}
      else
	fprintf(stderr, "wanted line %s has no exemplars\n", want[i]);
    }      
}

void
lines_have(char *l, char *s)
{
  if (lines_trace)
    fprintf(stderr, "lines_have: %s has %s\n", l, s);
  Hash *hp = hash_find(hhaves, (uccp)l);
  if (!hp)
    hash_add(hhaves, (uccp)l, (hp=hash_create(10)));
  hash_add(hp, (uccp)s, "");
}

static void
lines_want(char *s, int start, int end)
{
  char buf[strlen(s)+10];
  strcpy(buf, s);
  strcat(buf, ":");
  ++end;
  while (start < end)
    {
      if (lines_trace)
	fprintf(stderr, "lines_want: %d wants %s\n", start, s);
      sprintf(buf, "%d", start++);
      List *lp = hash_find(hwants, (uccp)buf);
      if (!lp)
	hash_add(hwants, pool_copy((uccp)buf, plines), (lp=list_create(LIST_SINGLE)));
      list_add(lp, pool_copy((uccp)s, plines));
    }
}

static void
lines_expand(char *lp)
{
  if (lines_trace)
    fprintf(stderr, "lines_expand: %s\n", lp);
  static int lno = 0;
  ++lno;
  char *siglum = lp;
  lp = strchr(siglum, '\t');
  if (lp)
    {
      *lp++ = '\0';
      while (*lp)
	{
	  while (*lp && !isdigit(*lp))
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
	  if (isdigit(*start) && isdigit(*end))
	    lines_want(siglum, atoi(start), atoi(end));
	}
    }
  else
    fprintf(stderr, "%d: syntax error: no <TAB> in line\n", lno);
}

void
lines_init(void)
{
  hhaves = hash_create(256);
  hwants = hash_create(256);
  plines = hpool_init();
}

void
lines_mode(const char *wants_list)
{
  unsigned char *lp;
  size_t n;
  FILE *wp = xfopen(wants_list, "r");
  if (wp)
    {
      while ((lp = loadoneline(wp, &n)))
	lines_expand((char*)lp);
      lines_miss();
    }
  else
    fprintf(stderr, "mx: failed to open wants list %s. Stop.\n", wants_list);
}
