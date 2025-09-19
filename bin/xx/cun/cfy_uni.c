#include <oraccsys.h>
#include "cfy.h"

int
cfy_uni_check(Cfy *c, uccp u)
{
  return (intptr_t)hash_find(c->c->fntp->uni, u);
}

Hash *
cfy_uni_load(const char *unifile)
{
  Hash *u = hash_create(4096);
  if (!access(unifile, R_OK))
    {
      size_t nl;
      uchar **l = loadfile_lines((uccp)unifile, &nl);
      int i;
      for (i = 0; i < nl; ++i)
	hash_add(u, l[i], "");
      return u;
    }
  return NULL;
}
