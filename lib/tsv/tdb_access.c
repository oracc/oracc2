#include <oraccsys.h>
#include "tsv.h"

void
tdb_open(Tsv *tp)
{
  tp->dp = dbi_open(tp->tdb_name, tp->tdb_dir);
}

int
tdb_exists(Tsv *tp, const char *id)
{
  dbi_find(tp->dp, (Uchar*)id);
  return tp->dp->nfound;
}

int
tdb_find(Tsv *tp, const char *id)
{
  dbi_find(tp->dp, (Uchar*)id);
  if (tp->dp->nfound)
    {
      Tsv_data *tip = dbi_detach_data(tp->dp, NULL);
      tp->data = *tip;
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
tdb_one_off(Tsv *tp, const char *id)
{
  tdb_open(tp);
  int ret = tdb_find(tp, id);
  tdb_close(tp);
  return ret;
}
