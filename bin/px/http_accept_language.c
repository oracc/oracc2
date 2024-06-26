#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "px.h"

typedef struct HAL
{
  const char *lang;
  double weighting;
} HAL;

static int
hal_cmp(const void*a, const void*b)
{
  if (((HAL*)b)->weighting > ((HAL*)a)->weighting)
    return 1;
  else if (((HAL*)a)->weighting > ((HAL*)b)->weighting)
    return -1;
  return strcmp(((HAL*)a)->lang, ((HAL*)b)->lang);
}

const char **
http_accept_language(const char *hal)
{
  int n = 1;
  char *orig = strdup(hal), *s;

  s = orig;
  while (*s)
    if (',' == *s++)
      ++n;
  HAL *h = malloc(n * sizeof(HAL));
  n = 0;
  s = orig;
  while (*s)
    {
      h[n].lang = lang32(s);
      h[n].weighting = 1.0;
      ++n;
      while (*s)
	{
	  if ('-' == *s)
	    *s++ = '\0'; /* Oracc ignores language flavour suffixes */
	  else if (';' == *s)
	    {
	      *s++ = '\0';
	      h[n-1].weighting = strtod(s, &s);
	    }
	  else if (',' == *s)
	    {
	      *s++ = '\0';
	      while (isspace(*s))
		++s;
	      break;
	    }
	  else
	    ++s;
	}
    }
  /* ensure that all lang codes are 2-letter if there are both 3- and
     2-letter versions */
  int i;
  for (i = 0; i < n; ++i)
    h[i].lang = lang32(h[i].lang);
  
  if (n > 1)
    qsort(h, n, sizeof(HAL), hal_cmp);

  const char **ret = malloc((1+n)*sizeof(char*));
  int j;
  for (i = j = 0; i < n; ++i)
    if (!j || strcmp(ret[j-1], h[i].lang))
      ret[j++] = h[i].lang;
  ret[i] = NULL;
  return ret;
}

#if 0
int
main(int argc, char **argv)
{
  const char **l = http_accept_language(argv[1]);
  while (*l)
    printf("%s ", *l++);
  printf("\n");
}
#endif
