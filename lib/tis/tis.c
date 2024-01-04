#include <oraccsys.h>
#include <tis.h>

struct tis_n_is
{
  size_t n;
  char **is;
  int pct;
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

static int
pct(float amount, float total)
{
  float pct = (amount / total) * 100;
  return (int) (pct+0.5);
}

const char **
tis_sort(Tisp tp)
{
  int nk;
  const char **k = hash_keys2(tp, &nk);
  qsort(k,nk,sizeof(const char *), cmpstringp);
  int i;
  for (i = 0; k[i]; ++i)
    {
      Memo *mp = hash_find(tp, (uccp)k[i]);
      size_t mn;
      char **mk = memo_keys(mp, &mn);
      static float last_total = 0.0;
      
      qsort(mk, mn, sizeof(const char *), cmpstringp);
      memo_term(mp);
      struct tis_n_is *tnis = memo_new(m_n_is);
      tnis->n = mn;
      tnis->is = mk;
      if (strchr(k[i], '|'))
	{
	  tnis->pct = pct((float)mn, last_total);
	}
      else
	{
	  tnis->pct = 100;
	  last_total = (float)mn;
	}
      hash_add(tp, (uccp)k[i], tnis);
    }
  return k;
}

void
tis_dump(FILE *fp, Tisp tp, const char **k)
{
  int i;
  for (i = 0; k[i]; ++i)
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
	  fprintf(fp, "%ld\t%d\t", tnis->n, tnis->pct);
	  for (j = 0; tnis->is[j]; ++j)
	    {
	      if (j)
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

