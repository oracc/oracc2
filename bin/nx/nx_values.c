#include <oraccsys.h>
#include "nx.h"

#if 0
static int nx_aev_d(nx_step *nxs);
static unsigned long long nx_aev_n(nx_step *nxs);
#endif

static nx_num nx_sum_aevs(nx_number *np);
static void nx_inst_aevs(nx_number *np);
static void nx_inst_aevs(nx_number *np);
static nx_num *nx_set_inst_aev(ns_inst *ni);
static nx_num *nx_set_num_aev(nx_number *np);
static void nx_calculate_mev(nx_number *np, const char **meup);

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

	  /* Compute the aev for each step-inst in the number */
	  nx_inst_aevs(np);

	  nxd_show_aevs(np);

	  nx_num sum = nx_sum_aevs(np);
	  nx_simplify(&sum);
	  nxd_show_sum(&sum);
	  
	  /* Divide the sum by the base fraction; the mev is expressed
	     in terms of the base, so this step ensures that np->aev
	     is ready for conversion to mev */
	  np->aev = nx_div_num(sum, np->sys->base_step->aev);

	  printf("nx_values: %llu/%d ÷ %llu/%d = %llu/%d\n",
		 sum.n, sum.d,
		 np->sys->base_step->aev.n, np->sys->base_step->aev.d,
		 np->aev.n, np->aev.d);

	  /* calculate the mev from the conv value; result in np->mev */
	  const char *meu = NULL;
	  nx_calculate_mev(np, &meu);
	  
	  /* render mev result */
	  printf("mev = %s\n", nx_modern(&np->mev, meu));
	}
    }
}

static void
nx_calculate_mev(nx_number *np, const char **meup)
{
  if (!strcmp((ccp)np->sys->conv, "1u"))
    {
      *meup = (const char *)np->sys->conv+1;
      np->mev.n = 1;
      np->mev.d = 1;
      nx_mul_frac(&np->mev, &np->aev);
    }
  else
    {
      const char *m = (ccp)np->sys->conv;
      while (*m && !isalpha(*m))
	++m;
      *meup = m;
      if (strchr((ccp)np->sys->conv, '.'))
	fprintf(stderr, "nx_values.c: please don't use '.' in mev\n");
      else
	{
	  np->mev.n = strtoll((ccp)np->sys->conv, NULL, 10);
	  np->mev.d = 1;
	  nx_mul_frac(&np->mev, &np->aev);
	}
    }
}

static void
nx_inst_aevs(nx_number *np)
{
  nx_step *ns;
  if (!np->sys->aev_done)
    nx_sys_aevs(np->sys);
  for (ns = np->steps; ns; ns = ns->next)
    {
      if (ns->type == NX_STEP_TOK)
	ns->aev = nx_set_inst_aev(ns->tok.inst);
      else
	ns->aev = nx_set_num_aev(ns->num);
    }
}

static nx_num
nx_sum_aevs(nx_number *np)
{
  /* Sum the numerators for the steps */
  nx_step *nxs;
  nx_num sum = { 0, 0 };
  for (nxs = np->steps; nxs; nxs = nxs->next)
    if (nxs->aev)
      nx_add_frac(&sum, nxs->aev);
  return sum;
}

static nx_num *
nx_set_inst_aev(ns_inst *ni)
{
  if (!ni)
    return NULL;
  
  nx_num *saev = &ni->step->aev;
  nx_num tmp;

  /* ensure common denominator */
  tmp.d = saev->d * ni->count.d;
  tmp.n = saev->n * ni->count.d;

  /* multiply inst numerator by count numerator */
  tmp.n *= ni->count.n;

  /* This ni->aev may not have the same denominator as the sys base */
  ni->aev = tmp;
  return &ni->aev;
}

static nx_num *
nx_set_num_aev(nx_number *np)
{
  nx_inst_aevs(np);
  nx_num sum = nx_sum_aevs(np);

  /* This is the total of the Snum */
  nx_simplify(&sum);
  np->aev = sum;

  /* this could be a plain S or it could be S nw -- if the latter,
     multiply by the nw */
  if (np->unit)
    {
      nx_num *saev = &np->unit->tok.inst->step->aev;
      nx_mul_frac(&sum, saev);
      /* Now sum contains the value for the nx_step that has the aev
	 ptr to the product of the Snum and the unit if any */
    }
  /* this is a memory leak--fix it */
  nx_num *sump = malloc(sizeof(nx_num));
  *sump = sum;
  return sump;
}

#if 0
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
#endif
