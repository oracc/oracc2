#include <oraccsys.h>
#include <tok.h>

/* Turn a list of sign-sigs into a signlist; sign sigs are
   SIGN-FORM-VALUE where FORM and VALUE can be empty */

Pool *p;

const char *
gsig_id(void)
{
  static int id = 0;
  char s[32];
  sprintf(s, "0x%05x", id++);
  return pool_copy(s, p);
}

int
main(int argc, char **argv)
{
  Hash *h = hash_create(1024);
  Hash *f = hash_create(1024);
  char l[1024], *lp;
  p = pool_init();
  
  while ((lp = fgets(l, 1024, stdin)))
    {
      Gsig *gp = gsig_parse_alloc(NULL);
      gp = gsig_parse(l);
      if (gp)
	{
	  const char *id = gsig_id();
	  hash_add(f, (uccp)id, pool_copy(gp->form));
	  List *fl = hash_find(f, gp->coresig);
	  if (!fl)
	    {
	      fl = list_create(LIST_SINGLE);
	      list_add(fl, pool_copy(gp->coresig));
	      hash_add(f, (ucp)gp->coresig, (void*)fl);
	    }
	  if (
	  if (*gp->sname)
	    {
	      Hash *hs, *hf;
	      if (!(hs = hash_find(h, (ucp)gp->sname)))
		hash_add(h, (ucp)gp->sname, hs = hash_create(32));
	      if (*gp->fname || *gp->value)
		{
		  if (!*gp->fname)
		    f = "-";
		  else
		    f = gp->fname;
		  if (!(hf = hash_find(hs, (ucp)f)))
		    hash_add(hs, (ucp)f, hf = hash_create(32));
		  if (*gp->value && !hash_find(hf, (ucp)gp->value))
		    hash_add(hf, (ucp)gp->value, "");
		}
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
