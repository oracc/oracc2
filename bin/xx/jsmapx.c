#include <oraccsys.h>
#include <roco.h>
#include <list.h>

Hash *seen;
List *ll[26];

static void
one_ll(Roco *r, int i, List *lp)
{
  int len = list_len(lp);
  printf("const %c = {\n", i+'a');
  int j;
  for (j = 0; j < len; ++j)
    {
      int k = (uintptr_t)list_next(lp);
      if (!hash_find(seen, r->rows[k][0]))
	{
	  hash_add(seen, r->rows[k][0], "");
	  if (j)
	    fputs(",\n",stdout);
	  printf("\"%s\":\"%s\"", r->rows[k][0], r->rows[k][1]);
	}
    }
  printf("\n};\n");
}

int
main(int argc, char **argv)
{
  Roco *r = roco_load1("-");
  int i;
  int multi = 0;
  unsigned const char *memb = NULL, *ctnr = NULL, *ctnd = NULL;

  seen = hash_create(1024);  

  if (argv[1])
    multi = 1;

  if (multi)
    {
      for (i = 0; i < 26; ++i)
	ll[i] = list_create(LIST_SINGLE);
      for (i = 0; i < r->nlines; ++i)
	{
	  unsigned const char *k = r->rows[i][0];
	  if ('*' == k[strlen((ccp)k)-1])
	    {
	      if (!strcmp((ccp)k, "memb*"))
		memb = r->rows[i][1];
	      else if (!strcmp((ccp)k, "ctnr*"))
		ctnr = r->rows[i][1];
	      else if (!strcmp((ccp)k, "ctnd*"))
		ctnd = r->rows[i][1];
	    }	  
	  else
	    {
	      list_add(ll[k[0]-'a'], (void*)(uintptr_t)i);
	    }
	}
      for (i = 0; i < 26; ++i)
	one_ll(r, i, ll[i]);
      printf("const aa = [");
      for (i = 0; i < 26; ++i)
	{
	  if (i)
	    printf(",");
	  printf("%c",i+'a');
	}
      printf("];\n");
      if (memb)
	printf("const memb = \"%s\";\n", memb);
      if (ctnr)
	printf("const ctnr = \"%s\";\n", ctnr);
      if (ctnd)
	printf("const ctnd = \"%s\";\n", ctnd);
    }
  else
    {
      printf("const x = {\n");
      for (i = 0; i < r->nlines; ++i)
	{
	  if (!hash_find(seen, r->rows[i][0]))
	    {
	      hash_add(seen, r->rows[i][0], "");
	      if (i)
		fputs(",\n",stdout);
	      printf("\"%s\":\"%s\"", r->rows[i][0], r->rows[i][1]);
	    }
	}
      printf("\n};");
    }
}
