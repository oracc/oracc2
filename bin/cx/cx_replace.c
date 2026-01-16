#include <oraccsys.h>
#include "cx.h"

static const char *
cx_merper_wrapper(const char *s)
{
  return (cx_merper(s, strlen(s)))->name;
}

void
cx_merge_periods(Cx *c)
{
  cx_replace(c, "period", cx_merper_wrapper);
}			

/* General purpose routine to support merge_periods; replace the
 * values in the given field with the replacements via the given
 * function.
 */
void
cx_replace(Cx *c, const char *field, Repfunc r)
{
  int f = cx_roco_field_index(c, field);
  if (f >= 0)
    {
      int i;
      for (i = 1; i < c->r->nlines; ++i)
	{
	  const char *rep = r((ccp)c->r->rows[i][f]);
	  if (rep)
	    c->r->rows[i][f] = (ucp)rep;
	}
    }
}
