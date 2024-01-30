#include <oraccsys.h>

static int
pct(float amount, float total)
{
  float pct = (amount / total) * 100;
  return (int) (pct+0.5);
}

int
main(int argc, char *const *argv)
{
  char buf[1024], *s;
  Hash *pri = NULL;
  Hash *sec = NULL;
  Hash *tid = NULL;
  List *seq = NULL;
  Pool *p = NULL;
  size_t total = 0;

  p = pool_init();
  pri = hash_create(1024);
  sec = hash_create(1024);
  tid = hash_create(1024);
  seq = list_create(LIST_SINGLE);

  while ((s = fgets(buf,1024,stdin)))
    {
      s[strlen(s)-1] = '\0';
      char **f = tab_split(buf);
      unsigned char *id = pool_copy((ucp)f[0], p);
      unsigned char *t = pool_copy((ucp)f[1], p);

      hash_add(tid,t,id);

      int count = atoi(f[2]);

      if ('.' == t[strlen((ccp)t)-1] && '.' == t[strlen((ccp)t)-2])
	{
	  total += count;
	  hash_add(pri, t, (void*)(uintptr_t)count);
	  list_add(seq, t);
	}
      else
	{
	  hash_add(sec, t, (void*)(uintptr_t)count);
	  list_add(seq, t);
	}
    }

  printf("Total instances of primary tokens: %ld\n", total);

  const char **k = hash_keys(pri);
  int i;
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(pri, (uccp)k[i]);
      printf("%s\t%s\t%d\t%d\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct(count,total));
    }

  k = hash_keys(sec);
  for (i = 0; k[i]; ++i)
    {
      int count = (uintptr_t)hash_find(sec, (uccp)k[i]);
      char *parent = strdup(k[i]), *e;
      /* The parent consists of all but the final segment of a secondary key */
      e = strrchr(parent,'.');
      if (*e)
	e[1] = '\0';
      int pcount = (uintptr_t)hash_find(pri, (uccp)parent);
      if (pcount)
	printf("%s\t%s\t%d\t%d\n", (char*)hash_find(tid,(uccp)k[i]), k[i], count, pct(count,pcount));
      else
	fprintf(stderr, "token %s should have parent %s but doesn't\n", k[i], parent);
    }
}
