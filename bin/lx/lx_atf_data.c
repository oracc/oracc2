#include <oraccsys.h>
#include "lx.h"

/*Construct the atf_data.tab file path for the project named in u; if
  u is from an umbrella project it is lacking the first member, which
  must be supplied in p */
char *
lx_atf_data(const char *u, const char *p)
{
  char *s = NULL;
  if (!p)
    p = "";
  s = malloc(strlen(p)+strlen(u)+2);
  sprintf(s, "%s%s%s", p, *p ? "/" : "", u);
  char *a = malloc(strlen(oracc_builds())+strlen(s)+strlen("//02pub/atf-data.tab0"));
  sprintf(a, "%s/%s/02pub/atf-data.tab", oracc_builds(), s);
  free(s);
  return a;
}
