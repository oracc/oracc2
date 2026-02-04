#include <oraccsys.h>
#include <math.h>

int verbose = 0;

static double
pct(double amount, double total)
{
  double pct = (amount / total) * 100;
  if (pct == INFINITY)
    {
      if (verbose)
	fprintf(stderr, "tokpct: amount %g / total %g * 100 yields INFINITY\n", amount, total);
      pct = (double)0.0;
    }
  return pct;
}

/* Tokens are divided into three parts: sem(el), bis and ter.
 *
 * For g tokens, sem = sign; bis = form; ter = value.
 *
 * For sem the parent count is the total sem in the dataset.
 *
 * For bis the parent count is the count of the token's sem.
 *
 * For ter the parent count is the count of the token's bis if that is
 * non-empty, or of sem if bis is empty.
 */
int
main(int argc, char *const *argv)
{
  char buf[1024], *s;
  Hash *sem = NULL;
  Hash *bis = NULL;
  Hash *ter = NULL;
  Hash *mrg = NULL;
  Hash *frm = NULL;
  Hash *tid = NULL;
  /*List *seq = NULL;*/
  Pool *p = NULL;
  size_t total = 0;

  p = pool_init();
  sem = hash_create(1024);
  bis = hash_create(1024);
  ter = hash_create(1024);
  mrg = hash_create(1024);
  frm = hash_create(1024);
  tid = hash_create(1024);
  /*seq = list_create(LIST_SINGLE);*/

  while ((s = fgets(buf,1024,stdin)))
    {
      s[strlen(s)-1] = '\0';
      char **f = tab_split(buf);
      unsigned char *t = pool_copy((ucp)f[0], p), *e;
      unsigned char *id = pool_copy((ucp)f[1], p);

      hash_add(tid,t,id);
      int count = atoi(f[2]);
      e = t + strlen((ccp)t);
      if ('+' == e[-1])
	{
	  hash_add(mrg, t, (void*)(uintptr_t)count);
	}
      else if ('f' == e[-1])
	{
	  hash_add(frm, t, (void*)(uintptr_t)count);
	}
      else if ('.' == e[-1] && '.' == e[-2])
	{
	  total += count;
	  hash_add(sem, t, (void*)(uintptr_t)count);
	}
      else if ('.' == t[strlen((ccp)t)-1])
	{
	  hash_add(bis, t, (void*)(uintptr_t)count);
	}
      else
	{
	  hash_add(ter, t, (void*)(uintptr_t)count);
	}
    }

  printf("t\to\t%ld\t%%\n", total);

  const char **k = hash_keys(sem);
  int i;
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(sem, (uccp)k[i]);
      printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct((double)count,(double)total));
    }
  
  k = hash_keys(bis);
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(bis, (uccp)k[i]);
      char parent[strlen(k[i])+1], *e;
      strcpy(parent,k[i]);
      /* The parent consists of the first segment */
      e = strchr(parent,'.');
      if (*e)
	strcpy(e,"..");
      int pcount = (uintptr_t)hash_find(sem, (uccp)parent);
      printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct((double)count,(double)pcount));
    }
  
  k = hash_keys(ter);
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(ter, (uccp)k[i]);
      char parent[strlen(k[i])+1], *e;
      strcpy(parent,k[i]);
      /* The parent consists of all but the final segment of a secondary key */
      e = strrchr(parent,'.');
      if (*e)
	e[1] = '\0';
      int pcount = 0;
      if ('.' == e[-1])
	pcount = (uintptr_t)hash_find(sem, (uccp)parent);
      else
	pcount = (uintptr_t)hash_find(bis, (uccp)parent);

      if (pcount)
	printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct(count,pcount));
      else
	fprintf(stderr, "token %s should have parent %s but doesn't\n", k[i], parent);
    }

  k = hash_keys(mrg);
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(mrg, (uccp)k[i]);
      printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct((double)count,(double)total));
    }

  k = hash_keys(frm);
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(frm, (uccp)k[i]);
      printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct((double)count,(double)total));
    }

}
