#include <oraccsys.h>
#include "cx.h"

static const char *
cx_merper_wrapper(const char *s)
{
  return (cx_merper(s, strlen(s)))->merge;
}

void
cx_merge_periods(Roco *r)
{
  cx_replace(r, "period", cx_merper_wrapper);
}			

/* General purpose routine to support merge_periods; replace the
 * values in the given field with the replacements via the given
 * function.
 */
void
cx_replace(Roco *r, const char *field, Repfunc repfunc)
{
  int f = cx_roco_field_index(r, field);
  if (f >= 0)
    {
      int i;
      for (i = 1; i < r->nlines; ++i)
	{
	  const char *rep = repfunc((ccp)r->rows[i][f]);
	  if (rep)
	    r->rows[i][f] = (ucp)rep;
	}
    }
}
