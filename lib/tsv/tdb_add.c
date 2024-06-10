#include <oraccsys.h>
#include "tsv.h"

int
tdb_add(Tsv *tp, const char *id, long int s, int l, int n)
{
  Tsv_data t_info = { -1L, -1 , -1};
  t_info.seek = s;
  t_info.len = l;
  t_info.count = n;
  if (DBI_BALK == dbi_add(tp->dp, (Uchar*)id, &t_info, 1))
    return 1;
  else
    return 0;
}
