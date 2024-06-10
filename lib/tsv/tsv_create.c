#include <oraccsys.h>
#include "tsv.h"

Tsv *
tsv_create(void)
{
  return calloc(1, sizeof(Tsv));
}

void
tsv_destroy(Tsv *tp)
{
  if (tp)
    {
      if (tp->free_dir)
	free((void*)tp->tdb_dir);
      if (tp->free_name)
	free((void*)tp->tdb_name);
    }
}
