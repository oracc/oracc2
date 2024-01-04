#include <oraccsys.h>
#include <tis.h>

struct tis_n_is
{
  size_t n;
  char **is;
};

Memo *m_n_is = NULL;

Tisp
tis_init(void)
{
  m_n_is = memo_init(sizeof(struct tis_n_is), 1024);
  return hash_create(1024);
}

void
tis_term(Tisp tp)
{
  memo_term(m_n_is);
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

char *
tis_id(const char *lang)
{
  static int idbase = 0;
  static char id[32];
  int i = 0;
  if (lang)
    {
      while (isalpha(lang[i]))
	{
	  id[i] = lang[i];
	  ++i;
	}
      id[i] = '\0';
    }
  sprintf((char*)id+i, ".r%05x", ++idbase);
  return id;
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
      struct tis_n_is *tnis = memo_new(m_n_is);
      tnis->n = nk;
      tnis->is = tk;
      hash_add(tp, (uccp)k[i], tnis);
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
      /* Retrieve the is data first */
      struct tis_n_is *tnis = hash_find(tp, (uccp)k[i]);

      /* Lang is appended to key as s.o0036652%qpc; remove it in the
	 .tis output because we are prepending it to the ID */
      char *lang = strchr(k[i], '%');
      if (lang)
	*lang++ = '\0';
      
      fputs(k[i], fp);
      fputc('\t', fp);
      fprintf(fp, "%s\t", tis_id(lang));
      if (tnis)
	{
	  int j;
	  fprintf(fp, "%ld\t", tnis->n);
	  for (j = 0; tnis->is[j]; ++j)
	    {
	      fputc(' ', fp);
	      fputs(tnis->is[j], fp);
	    }
	}
      else
	{
	  fputs("0\t", fp);
	  fprintf(stderr, "no is data found for key %s\n", k[i]);
	}
      fputc('\n', fp);
    }
}
