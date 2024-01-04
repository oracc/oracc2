#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <tis.h>

struct tokloc
{
  const char *P;
  const char *L;
  const char *W;
};

Hash *langs = NULL;
Pool *p;
struct tokloc tl;

static unsigned char *
langtok(const char *lang, unsigned char *tok)
{
  char lt[strlen(lang)+strlen((ccp)tok)+2];
  sprintf(lt, "%s%s", tok, lang);
  return pool_copy((ucp)lt, p);
}

static const char *
pw()
{
  char PW[strlen(tl.P)+strlen(tl.W)+2];
  sprintf(PW, "%s:%s", tl.P, tl.W);
  return (ccp)pool_copy((ucp)PW,p);
}

static const char *
sig_lang(unsigned char *sig)
{
  unsigned char *pct = NULL;
  if (sig && (pct = (ucp)strchr((ccp)sig, '%')))
    {
      unsigned char *col = (ucp)strchr((ccp)pct, ':');
      if (col)
	*col = '\0';
      char *hlang = hash_find(langs, pct);
      if (!hlang)
	{
	  hlang = (char*)pool_copy(pct, p);
	  (void)hash_add(langs, (ucp)hlang, hlang);
	}
      return hlang;
    }
  else
    return NULL;
}

static void
tok_m(Tisp tis, Hash *m, unsigned const char *tok)
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
	      sprintf(mtok, "%s|%s=%s%s",tok,kk[i],v,tl.L);
	      tis_add(tis, (ccp)pool_copy((ucp)mtok,p), pw());
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
  Tisp t = tis_init();
  
  langs = hash_create(10);
  p = pool_init();

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
	  else if ('T' == *lp)
	    {
	      char *project = strchr((ccp)lp, '\t');
	      if (project)
		{
		  char *tab = strchr((ccp)++project, '\t');
		  if (tab)
		    *tab = '\0';
		  if (!tl.P || strcmp(tl.P, project))
		    tl.P = (ccp)pool_copy((ucp)project,p);
		}
	      else
		fprintf(stderr, "tok2tis: T line with no PROJECT: %s\n", lp);
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
		{
		  *tab = '\0';
		  tl.L = sig_lang((ucp)++tab);
		  tis_add(t, (ccp)pool_copy(langtok(tl.L,lp),p), pw());
		  tok_m(t, m, lp);
		}
	      else
		fprintf(stderr, "tok2tis: no sig in line: %s\n", lp);
	    }
	  else
	    {
	      fprintf(stderr, "tok2tis: no W set: %s\n", lp);
	      exit(1);
	    }	
	}
    }
  tis_dump(stdout, t, tis_sort(t));
}
