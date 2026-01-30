#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

/* FIXME: the list creation here should use memo */

static unsigned char *
tail_norm(const unsigned char *n, Pool *p)
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
permute(List *heads, List *tails, Pool *p)
{
  List *nheads = list_create(LIST_SINGLE);
  const unsigned char *h;
  for (h = list_first(heads); h; h = list_next(heads))
    {
      Cof *cfp;
      for (cfp = list_first(tails); cfp; cfp = list_next(tails))
	{
	  Form t = *cfp->f;
	  t.form = (uccp)"";
	  t.norm = tail_norm(cfp->f->norm, p);
	  
	  /* iterate over the entry's senses duplicating the COF for each */
	  Sense *sp;
	  for (sp = list_first(cfp->e->senses); sp; sp = list_next(cfp->e->senses))
	    {
	      t.sense = sp->mng;
	      t.epos = sp->pos;
	      const unsigned char *tsig = form_sig(p, &t);
	      char s[strlen((ccp)h)+strlen("&&0") + strlen((ccp)tsig)];
	      sprintf(s, "%s&&%s", h, tsig);
	      list_add(nheads, pool_copy((uccp)s, p));
	    }
	}
    }

  return nheads;
}

List*
cof_sigs(Form *f2p, Pool *p)
{
  List *sigs = list_create(LIST_SINGLE);
  Form f = *f2p;

  /* f2p->norm is the norm sig; generate the head sig with only the first element of the norm */
  f.norm = (ucp)strdup((ccp)f2p->norm);
  char *s = (char*)f.norm;
  while (!isspace(*s))
    ++s;
  *s = '\0';
  unsigned char *head = form_sig(p, &f);
  list_add(sigs, head);
  int j;
  for (j = 0; j < csetp->ntails; ++j)
    {
      List *ltp = hash_find(csetp->cof_tails[j], (uccp)f2p->norm);
      if (ltp)
	sigs = permute(sigs, ltp, p);
      else
	break;
    }
  return sigs;
}








 #if 0
  int j;
  Cof *hcp;
      
  for (hcp = list_first(lheads); hcp; hcp = list_next(lheads))
    {
      hcp->f->cof_id = hcp->f;
      hcp->parts = memo_new_array(csetp->formsmem, csetp->ntails);
      for (j = 0; j < csetp->ntails; ++j)
	{
	  List *ltails = hash_find(csetp->cof_tails[j], (uccp)heads[i]);
	  if (!ltails)
	    break;
	  else
	    {
	      /* append each member of this list of tail segments to
		 each member of the list of heads */
	      Cof *tcp;
	      for (tcp = list_first(ltails); tcp; tcp = list_next(ltails))
		{
		  tcp->f->cof_id = hcp->f;
		  BIT_SET(tcp->f->flags, FORM_FLAGS_COF_TAIL);
		  hcp->f->parts = tcp->f;
		}
	    }
	}
    }
#endif
