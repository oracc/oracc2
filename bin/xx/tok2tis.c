#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>

#include <tis.h>

struct tokloc
{
  const char *W;
};

struct tokloc tl;
Pool *p;

void
xtis_sort(Tisp tp)
{
  const char **k = hash_keys(tp);
  fprintf(stderr, "tis:sort k[0] = %s\n", k[0]);
  int i;
  for (i = 0; k[i]; ++i)
    {
      Memo *mp = hash_find(tp, (uccp)k[i]);
      size_t nk;
      char **tk = memo_keys(mp, &nk);
      fprintf(stderr, "tis:sort tk[0] = %s\n", tk[0]);
      qsort(tk, nk, sizeof(const char *), cmpstringp);
      memo_term(mp);
      hash_add(tp, (uccp)k[i], tk);
    }
}

void
xtis_dump(FILE *fp, Tisp tp)
{
  int nk;
  const char **k = hash_keys2(tp, &nk);
  fprintf(stderr, "before qsort\n");
  qsort(k,nk,sizeof(const char *), cmpstringp);
  fprintf(stderr, "after qsort\n");
  int i;
  for (i = 0; i < nk; ++i)
    {
      fprintf(stderr, "i=%d; k=%s\n", i, k[i]);
      fputs(k[i], fp);
      fputc('\t', fp);
      char **is = hash_find(tp, (uccp)k[i]);
      if (is)
	{
	  int j;
	  for (j = 0; is[j]; ++j)
	    {
	      fprintf(stderr, "j=%d; is[j] = %s\n", j, is[j]);
	      fputc(' ', fp);
	      fputs(is[j], fp);
	    }
	}
      else
	fprintf(stderr, "no is data found for key %s\n", k[i]);
      fputc('\n', fp);
    }
}

int
main(int argc, char **argv)
{
  size_t nbytes;
  unsigned char *lp;
  p = pool_init();
  Tisp t = tis_init();
  while ((lp = loadoneline(stdin,&nbytes)))
    {
      if (isupper(*lp))
	{
	  if ('W' == *lp)
	    {
	      unsigned char *wid = (ucp)strchr((ccp)lp,'\t');
	      if (wid)
		tl.W = (ccp)pool_copy(++wid,p);
	    }
	}
      else
	{
	  if (tl.W)
	    {
	      tis_add(t, (ccp)pool_copy(lp,p), tl.W);
	    }
	  else
	    {
	      fprintf(stderr, "no W set\n");
	      exit(1);
	    }
	}
    }
  tis_sort(t);
  tis_dump(stdout, t);
}
