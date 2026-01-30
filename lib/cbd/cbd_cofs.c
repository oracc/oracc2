#include <oraccsys.h>
#include "cbd.h"

/* COF processing needs to be invoked after the set of glossaries in a
 * project has been read because COFs can cross glossaries.  It is a
 * requirement that all members of a COF are present in a project's
 * own glossary files.
 */

static Cof *
cbd_cof_norm_sig(Form *f2p)
{
  char buf[strlen((ccp)f2p->norm)+1];
  int index = 0, found_self = (*f2p->norm == '(' ? 0 : 1);
  char *b = buf, *s = buf;
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
  fprintf(stderr, "cbd_cof_norm_sig: %s => %s [%d]\n", f2p->norm, buf, index);

  cofp->e = f2p->entry;
  cofp->f = f2p;
  cofp->i = index;
  cofp->s = pool_copy((ucp)buf, csetp->pool);

  if (found_self == 0)
    {
      BIT_SET(f2p->flags, FORM_FLAGS_COF_HEAD);
      f2p->norm = cofp->s;
    }
  else
    {
      BIT_SET(f2p->flags, FORM_FLAGS_COF_TAIL);
      /* can't set cof_id here because COFs read from glossary don't identify their COF_HEAD */
    }

  return cofp;
}

void
cbd_cof_register(Form *f2p)
{
  Cof *cofp = cbd_cof_norm_sig(f2p);
  List *lp;
  if (cofp->i)
    {
      if (!csetp->cof_tails[cofp->i])
	{
	  csetp->cof_tails = realloc(csetp->cof_tails, cofp->i * sizeof(Hash*));
	  int j;
	  for (j = csetp->ntails; j < cofp->i; ++j)
	    if (!csetp->cof_tails[j])
	      csetp->cof_tails[j] = hash_create(12);
	  csetp->ntails = cofp->i;
	}
      lp = hash_find(csetp->cof_tails[cofp->i-1], cofp->s);
      if (!lp)
	{
	  lp = list_create(LIST_SINGLE);
	  hash_add(csetp->cof_tails[cofp->i-1], cofp->s, lp);
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
