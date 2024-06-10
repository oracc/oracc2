#include <oraccsys.h>
#include "tsv.h"

int
tsv_one_off(const char *tsv_fn, const char *dir, const char *name, const char *key)
{
  int ret = 0;
  Tsv *tp = tsv_create();
  tsv_paths(tp, tsv_fn, dir, name);
  if (!(ret = tdb_one_off(tp, key)))
    {
      tp->out_fp = stdout;
      tsv_output(tp);
    }
  tsv_destroy(tp);
  return ret;
}
