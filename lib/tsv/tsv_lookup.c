#include <oraccsys.h>
#include "tsv.h"

int
tsv_lookup(const char *tsv_fn, const char *dir, const char *name, const char **keys, size_t nkeys)
{
  int ret = 0;
  Tsv *tp = tsv_create();
  tsv_paths(tp, tsv_fn, dir, name);
  tp->keys = keys;
  tp->nkey = nkeys;
  tp->data = calloc(nkeys, sizeof(Tsv_data));
  if (!(ret = tdb_lookup(tp)))
    {
      tp->out_fp = stdout;
      tsv_output(tp);
    }
  tsv_destroy(tp);
  return ret;
}
