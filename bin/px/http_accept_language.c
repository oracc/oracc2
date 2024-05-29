#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
  return 0;
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
      h[n].lang = s;
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
  if (n > 1)
    qsort(h, n, sizeof(HAL), hal_cmp);
  const char **ret = malloc((1+n)*sizeof(char*));
  int i, j;
  for (i = j = 0; i < n; ++i)
    if (!j || strcmp(ret[j-1], h[i].lang))
      ret[j++] = h[i].lang;
  ret[i] = NULL;
  return ret;
}

int
main(int argc, char **argv)
{
  const char **l = http_accept_language(argv[1]);
  while (*l)
    printf("%s ", *l++);
  printf("\n");
}
