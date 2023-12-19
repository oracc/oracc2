#include <oracc.h>

Tisp
tis_init(void)
{
  return hash_create(1024);
}
void
tis_term(Tisp tp)
{
  hash_free(tp);
}

void
tis_add(Tisp tp, const char *key, const char *wid)
{
  Memo *mp = hash_find(tp, key);
  if (!mp)
    {
      mp = memo_init(sizeof(char**), 128);
      hash_add(tp, key, mp);
    }
  const char **n = memo_new(mp);
  *n = wid;
}

void
tis_sort(Tisp tp)
{
  const char **k = hash_keys(tp);
  int i;
  for (i = 0; k[i]; ++i)
    {
      Memo *mp = hash_find(tp, k[i]);
      size_t nk;
      const char **tk = memo_keys(tp, &nk);
      qsort(tk, sizeof(const char *), nk, cmpstringp);
      memo_term(mp);
      hash_add(tp, k[i], tk);
    }
}

void
tis_dump(File *fp, Tisp tp)
{
  size_t nk;
  const char **k = hash_keys2(tp, &nk);
  qsort(k,sizeof(const char *), nk, cmpstringp);
  int i;
  for (i = 0; i < nk; ++i)
    fprintf(fp, "%s\t%s\n", k[i], hash_find(tp, k[i]);
}
