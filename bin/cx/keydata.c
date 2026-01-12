#include <oraccsys.h>
#include <runexpat.h>
#include "keydata.h"

static KD_key *curr_keyp = NULL;

Keydata *
keydata_init(Cx *c, const char *file, Hash *hfields)
{
  Keydata *kp = calloc(1, sizeof(Keydata));
  kp->file = file;
  kp->hfields = hfields;
  kp->notindexed = hash_create(128);
  kp->keytypes = hash_create(128);
  kp->needtype = hash_create(128);
  kp->sortable = hash_create(128);
  kp->hkeys = hash_create(128);
  kp->mkey = memo_init(sizeof(KD_key), 256);
  kp->mval = memo_init(sizeof(KD_val), 256);
  kp->mmap = memo_init(sizeof(KD_map), 256);
  kp->p = pool_init();
  kp->c = c;
  return kp;
}

void
keydata_term(Keydata *kp)
{
  hash_free(kp->notindexed, NULL);
  hash_free(kp->keytypes, NULL);
  hash_free(kp->needtype, NULL);
  hash_free(kp->sortable, NULL);
  hash_free(kp->hkeys, NULL);
  memo_term(kp->mkey);
  memo_term(kp->mval);
  pool_term(kp->p);
}

int
keydata_validate(Keydata *kp)
{
  char *rnc = oracc_rnc("keydata.rnc");
  int chk = rnv_check(rnc, kp->file);
  free(rnc);
  return chk;
}

char *
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
    {
      const char *n = findAttr(atts, "n");
      if (*n && hash_find(kp->hfields, (uccp)n))
	{
	  uccp t;
	  hash_add(kp->keytypes, pattr("n"), (void*)(t = pattr("type")));
	  hash_add(kp->needtype, t, "");
	}
    }
  else if (!strcmp(name, "type"))
    {
      const char *n = findAttr(atts, "n");
      if (*n && hash_find(kp->needtype, (uccp)n))
	{
	  KD_map *mp = memo_new(kp->mmap);
	  mp->hr = (ccp)pattr("hr");
	  mp->fields = list_create(LIST_SINGLE);
	  hash_add(kp->sortable, pattr("n"), mp);
	}
    }
  else if (!strcmp(name, "keys"))
    {
      KD_key *keyp = memo_new(kp->mkey);
      keyp->class = (ccp)pattr("class");
      keyp->method = (ccp)pattr("method");
      keyp->type = (ccp)pattr("type");
      keyp->remapto = (ccp)pattr("remap-to");
      keyp->queryable = atoi(findAttr(atts, "queryable"));
      keyp->remap = atoi(findAttr(atts, "remap"));
      keyp->reorder = atoi(findAttr(atts, "reorder"));
      keyp->lvals = list_create(LIST_SINGLE);
      keyp->hvals = hash_create(128);
      hash_add(kp->hkeys, keyp->type, keyp);
      curr_keyp = keyp;
    }
  else if (!strcmp(name, "val"))
    {
      KD_val *vp = memo_new(kp->mval);
      vp->v = (ccp)pattr("n");
      vp->r = (ccp)pattr("remap");
      if (!*vp->r)
	vp->r = NULL;
      list_add(curr_keyp->lvals, vp);
      hash_add(curr_keyp->hvals, (uccp)vp->v, (void*)vp->r);
    }
#undef kp
}

static void
keydata_map(const unsigned char *k, void *v, void *u)
{
  KD_map *mp = hash_find(((Keydata*)u)->sortable, v);
  if (mp)
    list_add(mp->fields, (void*)k);
}

void
keydata_load(Keydata *kp)
{
  const char *fns[2] = { NULL,NULL};
  fns[0] = kp->file;
  runexpatNSuD(i_list, fns, kd_sH, NULL, NULL, kp);
  kp->fields = (char**)hash_keys2(kp->sortable, &kp->nfields);
  qsort(kp->fields, kp->nfields, sizeof(char*), cmpstringp);
  kp->nmapentries = kp->keytypes->key_count;
  hash_exec_user_key_data(kp->keytypes, keydata_map, kp);
}

