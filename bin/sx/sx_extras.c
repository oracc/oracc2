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
      fprintf(fp, "@compoundonly %s\n", k[i]);
  free(k);
  hash_free(extra_compounds, NULL);
  hash_free(extra_signs, NULL);
}
