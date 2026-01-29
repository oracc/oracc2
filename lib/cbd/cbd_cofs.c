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
  
}

static unsigned char *
cbd_cof_sig(Form *f2p)
{
  char buf[strlen((ccp)f2p->norm)+1];
  char *b = buf, *s = buf;
  int index = 0, found_self = (*f2p->norm == '(' ? 0 : 1);
  strcpy(buf, (ccp)f2p->norm);
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
  return pool_copy((ucp)buf, ((Entry*)f2p->entry)->owner->pool);
}

void
cbd_cof_register(Form *f2p)
{
  unsigned char *cofsig = (ucp)cbd_cof_sig(f2p);
}
