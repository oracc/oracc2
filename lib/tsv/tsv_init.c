#include <oraccsys.h>
#include "tsv.h"

Tsv *
tsv_init(const char *tsv_file, const char *dir, const char *name)
{
  Tsv *tp = tsv_create();
  tsv_paths(tp, tsv_file, dir, name);
  if ((tp->dp = dbi_create (tp->base, tp->tdb_dir, 1024, sizeof(Tsv_data), DBI_BALK)))
    return tp;
  tsv_destroy(tp);
  return NULL;
}

void
tsv_term(Tsv  *tp)
{
  dbi_flush(tp->dp);
  tsv_destroy(tp);
}
