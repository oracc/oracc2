#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

/* FIXME: the list creation here should use memo */

static unsigned char *
cof_tail_norm(const unsigned char *n, Pool *p)
{
  unsigned char buf[strlen((ccp)n)], *b = buf;
  const unsigned char *s = n;
  while (*s)
    {
      if (isspace(*s) && '$' == s[1] && '(' != s[2])
	{
	  s += 2;
	  while (*s && !isspace(*s))
	    *b++ = *s++;
	  *b = '\0';
	  return pool_copy(buf, p);
	}
      else
	++s;
    }
  return NULL;
}

static List *
cof_permute(List *heads, List *tails, Pool *p)
{
  List *nheads = list_create(LIST_SINGLE);
  const unsigned char *h;
  for (h = list_first(heads); h; h = list_next(heads))
    {
      Cof *cfp;
      for (cfp = list_first(tails); cfp; cfp = list_next(tails))
	{
	  Cform t = *cfp->f;
	  t.f.form = (uccp)"";
	  t.f.norm = cof_tail_norm(cfp->f->f.norm, p);
	  
	  /* iterate over the entry's senses duplicating the COF for each */
	  Sense *sp;
	  for (sp = list_first(cfp->e->senses); sp; sp = list_next(cfp->e->senses))
	    {
	      t.f.sense = sp->mng;
	      t.f.epos = sp->pos;
	      const unsigned char *tsig = form_sig(p, &t.f);
	      char s[strlen((ccp)h)+strlen("&&0") + strlen((ccp)tsig)];
	      sprintf(s, "%s&&%s", h, tsig);
	      list_add(nheads, pool_copy((uccp)s, p));
	    }
	}
    }

  return nheads;
}

List*
cof_sigs(Cform *f2p, Pool *p)
{
  List *sigs = list_create(LIST_SINGLE);
  Cform f = *f2p;

  /* f2p->norm is the norm sig; generate the head sig with only the first element of the norm */
  f.f.norm = (ucp)strdup((ccp)f2p->f.norm);
  char *s = (char*)f.f.norm;
  while (!isspace(*s))
    ++s;
  *s = '\0';
  unsigned char *head = form_sig(p, &f.f);
  list_add(sigs, head);
  int j;
  for (j = 0; j < csetp->ntails; ++j)
    {
      List *ltp = hash_find(csetp->cof_tails[j], (uccp)f2p->f.norm);
      if (ltp)
	sigs = cof_permute(sigs, ltp, p);
      else
	break;
    }
  return sigs;
}
