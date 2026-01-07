#include <oraccsys.h>
#include "keydata.h"

Keydata *
keydata_init(const char *file)
{
  Keydata *kp = calloc(1, sizeof(Keydata));
  kp->file = file;
  kp->notindexed = hash_create(128);
  kp->keytypes = hash_create(128);
  kp->sortable = hash_create(128);
  kp->lkeys = list_create(LIST_SINGLE);
  kp->hkeys = hash_create(128);
  kp->mkey = memo_init(sizeof(Key), 256);
  kp->mval = memo_init(sizeof(Val), 256);
  kp->p = pool_init();
  kp->hp = hpool_init();
}

void
keydata_term(Keydata *kp)
{
  hash_free(kp->notindex, NULL);
  hash_free(kp->keytypes, NULL);
  hash_free(kp->sortable, NULL);
  list_free(kp->lkeys, NULL);
  hash_free(kp->hkeys, NULL);
  memo_term(kp->mkey);
  memo_term(kp->mval);
  pool_term(kp->p);
  hpool_term(kp->p);
}

