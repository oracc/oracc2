#include <oraccsys.h>
#include "nx.h"

/* traverse the results performing conversion to modern values
 *
 * if a result has multiple possibilities report ambiguity only if two
 * or more results come from different base systems
 *
 * if there is more than one possible result for the same base system
 * (e.g., multiple different %C from different places/times) use the
 * first one.
 */
void
nx_values(nx_result *r)
{
  int i;
  for (i = 0; i < r->nr; ++i)
    {
      if (r->r[i].type == NX_NU)
	{
	  /* soon: check for ambiguity */
	  /* we only use the first match for computing the value */
	  nx_number *np = r->r[i].nu[0];
	  if (!np->sys->aev_done)
	    nx_sys_aevs(np->sys);
	}
    }  
}
