#include <oraccsys.h>
#include <tis.h>

Tisp
tis_init(void)
{
  return hash_create(1024);
}

void
tis_term(Tisp tp)
{
  hash_free(tp, NULL);
}

void
tis_add(Tisp tp, const char *key, const char *wid)
{
  Memo *mp = hash_find(tp, (uccp)key);
  if (!mp)
    {
      mp = memo_init(sizeof(char**), 128);
      hash_add(tp, (uccp)key, mp);
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
      Memo *mp = hash_find(tp, (uccp)k[i]);
      size_t nk;
      char **tk = memo_keys(mp, &nk);
      qsort(tk, nk, sizeof(const char *), cmpstringp);
      memo_term(mp);
      hash_add(tp, (uccp)k[i], tk);
    }
}

void
tis_dump(FILE *fp, Tisp tp)
{
  int nk;
  const char **k = hash_keys2(tp, &nk);
  qsort(k,nk,sizeof(const char *), cmpstringp);
  int i;
  for (i = 0; i < nk; ++i)
    {
      fputs(k[i], fp);
      fputc('\t', fp);
      char **is = hash_find(tp, (uccp)k[i]);
      if (is)
	{
	  int j;
	  for (j = 0; is[j]; ++j)
	    {
	      fputc(' ', fp);
	      fputs(is[j], fp);
	    }
	}
      else
	fprintf(stderr, "no is data found for key %s\n", k[i]);
      fputc('\n', fp);
    }
}
