#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <tis.h>

struct tokloc
{
  const char *P;
  const char *W;
};

Hash *langs = NULL;
Pool *p;
struct tokloc tl;

static unsigned char *
langtok(unsigned char *lang, unsigned char *tok)
{
  unsigned char lt[strlen(lang)+strlen(tok)+2];
  sprintf(lt, "%s:%s", lang, tok);
  return pool_copy(lt, p);
}

static unsigned char *
pw()
{
  unsigned char PW[strlen(tl.P)+strlen(tl.W)+2];
  sprintf(PW, "%s:%s", tl.P, tl.W);
  return pool_copy(PW,p);
}

static unsigned char *
sig_lang(unsigned char *sig)
{
  unsigned char *pct = NULL;
  if (sig && (pct = strchr(sig, '%')))
    {
      unsigned char *col = strchr(pct, ':');
      if (col)
	*col = '\0';
      unsigned char *hlang = hash_find(langs, pct);
      if (!hlang)
	{
	  hlang = pool_copy(pct, p);
	  (void)hash_add(langs, hlang, hlang);
	}
      return hlang;
    }
  else
    return NULL;
}

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
	      unsigned char *project = (ucp)strchr((ccp)lp, '\t');
	      if (project)
		{
		  unsigned char *tab = strchr(++project, '\t');
		  if (tab)
		    *tab = '\0';
		  if (!tl.P || strcmp(tl.P, project))
		    tl.P = (ccp)pool_copy(project,p);
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
	      char *lang = NULL;
	      if (tab)
		{
		  *tab = '\0';
		  lang = sig_lang(++tab);
		  tis_add(t, (ccp)pool_copy(langtok(lang,lp),p), pw());
		  tok_m(t, m, tl.W, lp);
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
  tis_sort(t);
  tis_dump(stdout, t);
}
