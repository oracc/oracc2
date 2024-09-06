#include <memo.h>
#include <keva.h>

Keva *
keva_create(Memo *kvmem, const char *key, const char *val)
{
  Keva *k = memo_new(kvmem);
  k->k = key;
  k->v = val;
  return k;
}

Keva *
keva_create3(Memo *kvmem, const char *key, const char *val, void *user)
{
  Keva *k = memo_new(kvmem);
  k->k = key;
  k->v = val;
  k->u = user;
  return k;
}
