#include <oraccsys.h>
#include <runexpat.h>
#include "keydata.h"

static Key *curr_keyp = NULL;

Keydata *
keydata_init(const char *file)
{
  Keydata *kp = calloc(1, sizeof(Keydata));
  kp->file = file;
  kp->notindexed = hash_create(128);
  kp->keytypes = hash_create(128);
  kp->sortable = hash_create(128);
  kp->hkeys = hash_create(128);
  kp->mkey = memo_init(sizeof(Key), 256);
  kp->mval = memo_init(sizeof(Val), 256);
  kp->p = pool_init();
  return kp;
}

void
keydata_term(Keydata *kp)
{
  hash_free(kp->notindexed, NULL);
  hash_free(kp->keytypes, NULL);
  hash_free(kp->sortable, NULL);
  hash_free(kp->hkeys, NULL);
  memo_term(kp->mkey);
  memo_term(kp->mval);
  pool_term(kp->p);
}

const char *
keydata_find(const char *project)
{
  return findfile(project, "keydata.xml", "lib/data/keydata.xml");
}

static void
kd_sH(void *userData, const char *name, const char **atts)
{
#define kp ((Keydata*)userData)
#define pattr(a) pool_copy((uccp)findAttr(atts, (a)), kp->p)
  if (!strcmp(name, "field"))
    hash_add(kp->notindexed, pattr("n"), (void*)"");
  else if (!strcmp(name, "keytype"))
    hash_add(kp->keytypes, pattr("n"), pattr("type"));
  else if (!strcmp(name, "type"))
    hash_add(kp->sortable, pattr("n"), pattr("hr"));
  else if (!strcmp(name, "keys"))
    {
      Key *keyp = memo_new(kp->mkey);
      keyp->class = (ccp)pattr("class");
      keyp->method = (ccp)pattr("method");
      keyp->type = (ccp)pattr("type");
      keyp->remapto = (ccp)pattr("remap-to");
      keyp->queryable = atoi(findAttr(atts, "queryable"));
      keyp->remap = atoi(findAttr(atts, "remap"));
      keyp->reorder = atoi(findAttr(atts, "reorder"));
      keyp->lvals = list_create(LIST_SINGLE);
      curr_keyp = keyp;
    }
  else if (!strcmp(name, "val"))
    {
      Val *vp = memo_new(kp->mval);
      vp->v = (ccp)pattr("n");
      vp->r = (ccp)pattr("remap");
      if (!*vp->r)
	vp->r = NULL;
      list_add(curr_keyp->lvals, vp);
      hash_add(curr_keyp->hvals, (uccp)vp->v, (void*)vp->r);
    }
#undef kp
}

void
keydata_load(Keydata *kp)
{
  const char *fns[2] = { NULL,NULL};
  fns[0] = kp->file;
  runexpatNSuD(i_list, fns, kd_sH, NULL, NULL, kp);
}

