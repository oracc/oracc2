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

/* Input is in three columns: token<TAB>ID<TAB>COUNT
 *
 * Tokens are divided into three parts: sem(el), bis and ter.
 *
 * For l tokens, sem = CGP; bis = CGPSE; ter = value.
 *
 * For sem the parent count is the total sem in the dataset.
 *
 * For bis the parent count is the count of the token's sem.
 *
 * For ter the parent count is the count of the token's bis if that is
 * non-empty, or of sem if bis is empty.
 *
 * g tokens are different from l tokens in that we generate entries
 * for sem and also for all bis of sem, i.e., we support calculation
 * based on word-count and also sense-count.
 */
int
main(int argc, char *const *argv)
{
  const char *file;
  FILE *in_fp;
  char *s;
  Hash *sem = NULL;
  Hash *bis = NULL;
  Hash *ter = NULL;
  Hash *tid = NULL;
  tid = hash_create(1024);
  Pool *p = NULL;
  size_t total = 0;

  p = pool_init();
  sem = hash_create(1024);
  bis = hash_create(1024);
  ter = hash_create(1024);
  tid = hash_create(1024);

  if (argv[optind])
    {
      file = argv[optind];
      in_fp = xfopen(file, "r");
      if (!in_fp)
	exit(1);
    }
  else
    {
      file = "-";
      in_fp = stdin;
    }

  while ((s = (char*)loadoneline(in_fp, NULL)))
    {
      if ('@' == *s)
	continue;
      
      char **f = tab_split(s);
      if (!f[1])
	continue;

      unsigned char *t = pool_copy((ucp)f[0], p), *e;
      unsigned char *id = pool_copy((ucp)f[1], p);
      hash_add(tid,t,id);
      int count = atoi(f[2]);

      e = t + strlen((ccp)t);
      if (1 == e[-1] && 1 == e[-2])
	{
	  total += count;
	  hash_add(sem, t, (void*)(uintptr_t)count);
	}
      else if (1 == e[-1])
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
      e = strchr(parent,1);
      if (*e)
	{
	  e[0] = 1;
	  e[1] = 1;
	  e[2] = '\0';
	}
      int pcount = (uintptr_t)hash_find(sem, (uccp)parent);
      printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct((double)count,(double)pcount));
    }
  
  k = hash_keys(ter);
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(ter, (uccp)k[i]);
      char parent[strlen(k[i])+1], *e;
      strcpy(parent,k[i]);
#if 1
      int pcount = 0;
      e = strchr(parent,1) + 1;
      if (1 == *e)
	{
	  ++e;
	  *e = '\0';
	  pcount = (uintptr_t)hash_find(sem, (uccp)parent);
	}
      else
	{
	  e = strchr(e, 1);
	  ++e;
	  *e = '\0';
	  pcount = (uintptr_t)hash_find(bis, (uccp)parent);
	}		     
#else
      /* The parent consists of all but the final segment of a secondary key */
      e = strrchr(parent,1);
      if (*e)
	e[1] = '\0';
#endif

      if (pcount)
	printf("%s\t%s\t%d\t%.3g\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct(count,pcount));
      else
	fprintf(stderr, "token %s should have parent %s but doesn't\n", k[i], parent);
    }
}

void help(void){}
int opts(int optch, const char*optarg){return 0;}
