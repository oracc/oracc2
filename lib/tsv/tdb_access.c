#include <oraccsys.h>
#include "tsv.h"

void
tdb_open(Tsv *tp)
{
  tp->dp = dbi_open(tp->base, tp->tdb_dir);
}

int
tdb_exists(Tsv *tp, const char *id)
{
  dbi_find(tp->dp, (Uchar*)id);
  return tp->dp->nfound;
}

int
tdb_find(Tsv *tp, const char *id, int i)
{
  dbi_find(tp->dp, (Uchar*)id);
  if (tp->dp->nfound)
    {
      tp->data[i] = *(Tsv_data*)(dbi_detach_data(tp->dp, NULL));
      return 0;
    }
  else
    return 1;
}

void
tdb_close(Tsv *tp)
{
  if (tp->dp)
    {
      dbi_close(tp->dp);
      tp->dp = NULL;
    }
}

int
tdb_lookup(Tsv *tp)
{
  tdb_open(tp);
  int i, ret = 0;
  for (i = 0; tp->keys[i]; ++i)
    {
      if (tdb_find(tp, tp->keys[i], i))
	{
	  fprintf(stderr, "tx: %s not found in %s\n", tp->keys[i], tp->tsv_fn);
	  ++ret;
	}
    }
  tdb_close(tp);
  return ret;
}
