#include <oraccsys.h>

Hash *lig;

void
lig_load(const char *ligfile)
{
  lig = hash_create(1024);
  size_t nl;
  uchar **l = loadfile_lines((uccp)ligfile, &nl);
  int i;
  for (i = 0; i < nl; ++i)
    hash_add(lig, l[i], "");
}
