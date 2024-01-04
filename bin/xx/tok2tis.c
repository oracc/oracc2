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

static void
tok_m(Tisp tis, Hash *m, const char *w, unsigned const char *tok)
{
  const char **kk = hash_keys(m);
  if (kk)
    {
      int i;
      for (i = 0; kk[i]; ++i)
	{
	  const char *v = hash_find(m, (ucp)kk[i]);
	  if (v && *v)
	    {
	      char mtok[strlen((ccp)tok)+strlen(kk[i])+strlen(v)+4];
	      sprintf(mtok, "%s|%s:%s",tok,kk[i],v);
	      tis_add(tis, (ccp)pool_copy((ucp)mtok,p), w);
	    }
	}
      free(kk);
    }
}

int
main(int argc, char **argv)
{
  size_t nbytes;
  unsigned char *lp;
  Hash *m = hash_create(10);
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
	  else if ('K' == *lp)
	    {
	      unsigned char *k, *v;
	      k = (ucp)strchr((ccp)lp,'\t');
	      if (k)
		{
		  *k++ = '\0';
		  v = (ucp)strchr((ccp)k, '\t');
		  if (v)
		    {
		      *v++ = '\0';
		      if (hash_find(m, k))
			hash_add(m, k, pool_copy(v,p));
		      else
			hash_add(m, pool_copy(k,p), pool_copy(v,p));
		    }
		}
	    }
	}
      else
	{
	  if (tl.W)
	    {
	      char *tab = strchr((ccp)lp, '\t');
	      if (tab)
		*tab = '\0';
	      tis_add(t, (ccp)pool_copy(lp,p), tl.W);
	      tok_m(t, m, tl.W, lp);
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
