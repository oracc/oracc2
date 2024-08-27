#include <oraccsys.h>
#include "nx.h"

static int
nx_aev_d(nx_step *nxs)
{
  if (nxs->type == NX_STEP_TOK)
    return nxs->tok.inst->step->aev.d;
  else
    return nxs->num->unit->tok.inst->step->aev.d;
}

static unsigned long long
nx_aev_n(nx_step *nxs)
{
  if (nxs->type == NX_STEP_TOK)
    {
      if (nxs->tok.inst)
	return nxs->tok.inst->step->aev.n;
      else
	return 0;
    }
  else
    return nxs->num->unit->tok.inst->step->aev.n;
}

static nx_num nx_div_num(nx_num divide, nx_num by);

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
	  ns_step *base_step = NULL;
	  if (!np->sys->aev_done)
	    base_step = nx_sys_aevs(np->sys);

	  /* Sum the numerators for the steps */
	  nx_step *s;
	  nx_num sum;
	  sum.d = nx_aev_d(np->steps);
	  for (s = np->steps; s; s = s->next)
	    sum.n += nx_aev_n(s);

	  /* Divide the sum by the base fraction */
	  nx_num res = nx_div_num(sum, base_step->aev);
	  
	  printf("nx_values: %llu/%d ÷ %llu/%d = %llu/%d\n", sum.n, sum.d, base_step->aev.n, base_step->aev.d, res.n, res.d);
	  
	  /* Render result as mev */
	  
	}
    }  
}

static nx_num
nx_div_num(nx_num divide, nx_num by)
{
  nx_num res;
  int d = by.d;
  by.d = (int)by.n;
  by.n = d;
  res.n = divide.n * by.n;
  res.d = divide.d * by.d;
  return res;
}
