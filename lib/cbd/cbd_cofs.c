#include <oraccsys.h>
#include "cbd.h"

/* COF processing needs to be invoked after the set of glossaries in a
 * project has been read because COFs can cross glossaries.  It is a
 * requirement that all members of a COF are present in a project's
 * own glossary files.
 */

static Cof *
cbd_cof_norm_sig(Cform *cfp)
{
  char buf[strlen((ccp)cfp->f.norm)+1];
  int index = 0, found_self = (*cfp->f.norm == '(' ? 0 : 1);
  char *b = buf, *s = buf;
  strcpy(buf, (ccp)cfp->f.norm);
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
	      found_self = 2;
	      /*newnorm = b;*/
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
  fprintf(stderr, "cbd_cof_norm_sig: %s => %s [%d]\n", cfp->f.norm, buf, index);

  cofp->e = cfp->entry;
  cofp->f = f2p;
  cofp->i = index;
  cofp->s = pool_copy((ucp)buf, csetp->pool);

  if (found_self == 1)
    {
      BIT_SET(cfp->f.flags, FORM_FLAGS_COF_HEAD);
      cfp->f.norm = cofp->s;
    }
  else
    {
      BIT_SET(cfp->f.flags, FORM_FLAGS_COF_TAIL);
      /* can't set cof_id here because COFs read from glossary don't identify their COF_HEAD */
    }

  return cofp;
}

void
cbd_cof_register(Cform *cfp)
{
  Cof *cofp = cbd_cof_norm_sig(f2p);
  List *lp;
  if (cofp->i)
    {
      if (!csetp->cof_tails[cofp->i-1])
	{
	  csetp->cof_tails = realloc(csetp->cof_tails, (cofp->i+1) * sizeof(Hash*));
	  int j;
	  for (j = csetp->ntails; j < cofp->i; ++j)
	    if (!csetp->cof_tails[j])
	      csetp->cof_tails[j] = hash_create(12);
	  csetp->cof_tails[j] = NULL;
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
