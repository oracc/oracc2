#include <oraccsys.h>
#include "nx.h"

static void nx_set_aev_r(ns_step *s, int d);

int trace_aev = 0;

void
nx_sys_aevs(ns_sys *sys)
{
  sys->aev_done = 1;
  ns_step *base_step = NULL;
  if (sys->base)
    {
      /* find the base */
      ns_step *s; /* step */
      for (s = sys->steps; s; s = s->next)
	if (!strcmp((ccp)s->unit, (ccp)sys->base))
	  break;

      /* error if base not found */
      if (s)
	base_step = s;

      /* compute common denominator of right-steps after base */
      ns_step *r;
      int d = 1;
      for (r = s; r->next; r = r->next)
	{
	  d *= (int)r->next->mult.n;
	  if (!r->next)
	    break;
	}

      r->aev.n = 1;
      r->aev.d = d;

      if (trace_aev)
	printf("nx_sys_aevs: common denominator = %d\n", d);
      
      /* traverse up setting all the aev to have the same
	 denominator */
      ns_step *b; /* bigger */
      for (b = base_step; b; b = b->prev)
	nx_set_aev_r(b, d);
    }
  sys->base_step = base_step;
}

static void
nx_set_aev_r(ns_step *s, int d)
{
  ns_step *r = s->next;
  unsigned long long n;
  if (r)
    n = r->aev.n * r->mult.n;
  else
    n = 1;
  s->aev.n = n;
  s->aev.d = d;
  if (trace_aev)
    printf("R: setting %s aev to %llu/%d\n", s->unit, n, d);
}
