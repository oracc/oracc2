#include <oraccsys.h>
#include "cbd.h"

/* COF processing needs to be invoked after the set of glossaries in a
 * project has been read because COFs can cross glossaries.  It is a
 * requirement that all members of a COF are present in a project's
 * own glossary files.
 */

void
cbd_cofs(void)
{
  const char **heads = hash_keys(csetp->cof_heads);
  int i;
  for (i = 0; heads[i]; ++i)
    {
      List *lheads = hash_find(csetp->cof_heads, (uccp)heads[i]);
      int j;
      Cof *hcp;
      for (hcp = list_first(lheads); hcp; hcp = list_next(lheads))
	{
	  hcp->f->cof_id = hcp->f;
	  BIT_SET(hcp->f->flags, FORM_FLAGS_COF_HEAD);
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
}

static Cof *
cbd_cof_sig(Form *f2p)
{
  char buf[strlen((ccp)f2p->norm)+1];
  int index = 0, found_self = (*f2p->norm == '(' ? 0 : 1);
  char *b = buf, *s = buf, *newnorm = (found_self ? buf : NULL);
  strcpy(buf, (ccp)f2p->norm);
  Cof *cofp = memo_new(csetp->cofmem);
  
  while (*s)
    {
      if ('$' == *s)
	{
	  *b++ = *s++;
	  if ('(' == *s)
	    {
	      ++s;
	      if (!found_self)
		++index;
	    }
	  else
	    {
	      found_self = 1;
	      newnorm = b;
	      *b++ = *s++;
	    }
	}
      else if ('(' == *s)
	{
	  if (!found_self)
	    ++index;
	  ++s;
	}
      else if (')' == *s)
	++s;
      else
	*b++ = *s++;
    }
  *b = '\0';  
  fprintf(stderr, "cbd_cof_sig: %s => %s [%d]\n", f2p->norm, buf, index);
  cofp->e = f2p->entry;
  cofp->f = f2p;
  cofp->i = index;
  cofp->s = pool_copy((ucp)buf, csetp->pool);

  s = newnorm;
  while (!isspace(*s))
    ++s;
  if (*s)
    *s = '\0';
  f2p->norm = pool_copy((ucp)newnorm, csetp->pool);
  
  return cofp;
}

void
cbd_cof_register(Form *f2p)
{
  Cof *cofp = cbd_cof_sig(f2p);
  List *lp;
  if (cofp->i)
    {
      if (!csetp->cof_tails[i])
	{
	  csetp->cof_tails = realloc(csetp->cof_tails, i * sizeof(Hash*));
	  int j;
	  for (j = csetp->ntails; j < i; ++j)
	    if (!csetp->cof_tails[j])
	      csetp->cof_tails[j] = hash_create(12);
	  csetp->ntails = i;
	}
      lp = hash_find(csetp->cof_tails[i-1], cofp->s);
      if (!lp)
	{
	  lp = list_create(LIST_SINGLE);
	  hash_add(csetp->cof_tails[i-1], cofp->s, lp);
	}
    }
  else
    {
      lp = hash_find(csetp->cof_heads, cofp->s);
      if (!lp)
	{
	  lp = list_create(LIST_SINGLE);
	  hash_add(csetp->cof_heads, cofp->s, lp);
	}
    }
  list_add(lp, cofp);
}
