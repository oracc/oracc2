#include <oraccsys.h>
#include <oracclocale.h>
#include "sx.h"

static Hash *extra_compounds;
static Hash *extra_signs;
static Pool *extra_p;

void
sxx_init(void)
{
  extra_p = pool_init();
  extra_compounds = hash_create(128);
  extra_signs = hash_create(128);
}

void
sxx_compound(unsigned const char *c)
{
  hash_add(extra_compounds, pool_copy(c,extra_p), "");
}

void
sxx_sign(unsigned const char *s)
{
  hash_add(extra_signs, pool_copy(s,extra_p), "");
}

static char *
strip_outer_parens(const char *cpdelt)
{
  char *ret = malloc(strlen(cpdelt));
  const char *end;
  if ('|' == *cpdelt)
    {
      ++cpdelt;
      ret[0] = '|';
      ret[1] = '\0';
    }
  else
    ret[0] = '\0';
  if ('(' == *cpdelt)
    ++cpdelt;
  end = cpdelt + strlen(cpdelt);
  if ('|' == end[-1])
    --end;
  if (')' == end[-1])
    --end;
  strncat(ret, cpdelt, end-cpdelt);
  if ('|' == *ret)
    strcat(ret, "|");
  return ret;
}

void
sxx_output(FILE *fp)
{
  const char **k = hash_keys(extra_signs);
  int i;
  for (i = 0; k[i]; ++i)
    fprintf(fp, "@sign %s\n@end sign\n\n", k[i]);
  free(k);
  k = hash_keys(extra_compounds);
  for (i = 0; k[i]; ++i)
    if (!hash_find(extra_signs, (uccp)k[i]))
      {
	char *x = NULL;
	const char *e = k[i] + strlen(k[i]);
	if (('(' == k[i][0] && ')' == e[-1])
	    || ('|' == k[i][0] && '|' == e[-1]
		&& '(' == k[i][1] && ')' == e[-2]))
	  x = strip_outer_parens(k[i]);
	fprintf(fp, "@compoundonly %s\n", x ? x : k[i]);
	if (x)
	  free(x);
      }
  free(k);
  hash_free(extra_compounds, NULL);
  hash_free(extra_signs, NULL);
}
