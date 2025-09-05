#include <oraccsys.h>
#include "cfy.h"

int espaces[ELT_LAST];

/* Set the breakage on space elements:
 *
 * When the same breakage is set on the non-space elements before or
 * after a run of one or more space elements, set the breakage on the
 * entire run of breakage elements to be match the environment
 */
void
cfy_breakage(Cfy *c, Elt **epp)
{
  int i;
  Btype last_breakage = BRK_NONE;
  for (i = 0; epp[i]; ++i)
    {
      if (espaces[epp[i]->etype])
	{
	  if (last_breakage != BRK_NONE)
	    {
	      int j = i+1;
	      while (epp[j] && espaces[epp[j]->etype])
		++j;
	      if (epp[j] && epp[j]->btype == last_breakage)
		{
		  while (i < j)
		    epp[i++]->btype = last_breakage;
		}
	    }
	}
      else
	last_breakage = epp[i]->btype;
    }
}
