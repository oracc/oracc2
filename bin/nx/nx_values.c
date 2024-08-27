#include <oraccsys.h>
#include "nx.h"

static int nx_aev_d(nx_step *nxs);
static unsigned long long nx_aev_n(nx_step *nxs);
static nx_num nx_div_num(nx_num divide, nx_num by);
static nx_num nx_sum_aevs(nx_num np);

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

	  /* Compute the aev for each step-inst in the number */
	  nx_inst_aevs(np);
	  

	  nx_num sum = nx_sum_aevs(np);
	  
	  /* Divide the sum by the base fraction */
	  np->aev = nx_div_num(sum, base_step->aev);
	  
	  printf("nx_values: %llu/%d ÷ %llu/%d = %llu/%d\n",
		 sum.n, sum.d,
		 base_step->aev.n, base_step->aev.d,
		 np->aev.n, np->aev.d);
	  
	  /* Render result as mev */
	  
	}
    }  
}

static nx_num
nx_sum_aevs(nx_num np)
{
  /* Sum the numerators for the steps */
  nx_step *s;
  nx_num sum;
  sum.d = nx_aev_d(np->steps);
  for (s = np->steps; s; s = s->next)
    sum.n += nx_aev_n(s);
  return sum;
}

static void
nx_set_inst_aev(ns_inst *ni)
{
  nx_num *saev = &ni->step->aev;
  nx_num tmp;

  /* ensure common denominator */
  tmp.d = saev->d * ni->count->d;
  tmp.n = saev->n * ni->count->d;

  /* multiply inst numerator by count numerator */
  tmp.n *= ni->count->n;

  /* This ni->aev may not have the same denominator as the sys base */
  ni->aev = tmp;
}

static void
nx_set_num_aev(nx_num np)
{
  nx_set_inst_aevs(np);
  nx_num sum = nx_sum_aevs(np);
  
  /* this could be a plain S or it could be S nw -- if the latter,
     multiply by the nw */
  if (np->unit)
    {
      nx_num *saev = &nxs->tok.inst->step->aev;
      if (sum.d != saev->d)
	{
	  int d = sum.d * saev->d;
	  unsigned long long n = sum.n * saev->d;
	  sum.d = d;
	  sum.n = n * saev->n;
	}
      else
	sum.n *= saev->n;
    }

  np->aev = sum;
}

static void
nx_inst_aevs(nx_num *np)
{
  nx_step *nxs;
  for (ns = np->steps; ns; ns = ns->next)
    {
      if (nxs->type == NX_STEP_TOK)
	nx_set_inst_aev(nxs->tok.inst);
      else
	nx_set_num_aev(nxs->num);
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

static nx_num *
nx_step_aev(nx_step *nxs)
{
  if (nxs->type == NX_STEP_TOK)
    return &nxs->tok.inst->step->aev;
  else
    return &nxs->num->unit->tok.inst->step->aev;
}

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
