#include <oraccsys.h>

/* Turn a list of sign-sigs into a signlist; sign sigs are
   SIGN-FORM-VALUE where FORM and VALUE can be empty */

int
main(int argc, char **argv)
{
  Hash *h = hash_create(1024);
  char l[1024], *lp;
  while ((lp = fgets(l, 1024, stdin)))
    {
      if ('-' == *lp)
	continue;
      char *s, *f, *v, *r;
      if ('@' == *lp || '\n' == *lp)
	{
	  fputs(lp, stdout);
	  continue;
	}
      if ('\n' == lp[strlen(lp)-1])
	lp[strlen(lp)-1] = '\0';
      s = r = strdup(lp);
      while (*r && '-' != *r)
	++r;
      *r++ = '\0';
      f = r;
      while (*r && '-' != *r)
	++r;
      *r++ = '\0';
      v = r;
      if (*s && strcmp(s, "X") || (v[1] || ('-' != *v && 'x' != *v)))
	{
	  Hash *hs, *hf;
	  if (!(hs = hash_find(h, (ucp)s)))
	    hash_add(h, (ucp)s, hs = hash_create(32));
	  if (*f || *v)
	    {
	      if (!*f)
		f = "-";
	      if (!(hf = hash_find(hs, (ucp)f)))
		hash_add(hs, (ucp)f, hf = hash_create(32));
	      if (*v && !hash_find(hf, (ucp)v))
		hash_add(hf, (ucp)v, "");
	    }
	}
    }
  int i;
  char **sk = (char**)hash_keys(h);
  for (i = 0; sk[i]; ++i)
    {
      printf("@sign %s\n", sk[i]);
      Hash *f = hash_find(h, (ucp)sk[i]);
      Hash *v = hash_find(f, (ucp)"-");
      if (v)
	{
	  char **vk = (char **)hash_keys(v);
	  int v;
	  for (v = 0; vk[v]; ++v)
	    printf("@v %s\n", vk[v]);
	}
      char **fk = (char**)hash_keys(f);
      int j;
      for (j = 0; fk[j]; ++j)
	if (strcmp(fk[j], "-"))
	  {
	    printf("@form %s\n", fk[j]);
	    Hash *v = hash_find(f, (ucp)fk[j]);
	    if (v)
	      {
		char **vk = (char**)hash_keys(v);
		int v;
		for (v = 0; vk[v]; ++v)
		  printf("@v %s\n", vk[v]);
	      }
	    printf("@@\n");
	  }
      printf("@end sign\n\n");
    }
}
