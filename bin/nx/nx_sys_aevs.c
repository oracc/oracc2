#include <oraccsys.h>
#include "nx.h"

static void nx_set_aev_l(ns_step *s);
static void nx_set_aev_r(ns_step *s);

void
nx_sys_aevs(ns_sys *sys)
{
  sys->aev_done = 1;
  if (sys->base)
    {
      /* find the base */
      ns_step *s; /* step */
      for (s = sys->steps; s; s = s->next)
	if (!strcmp((ccp)s->unit, (ccp)sys->base))
	  break;

      /* error if base not found */
      
      /* traverse down to the smallest fraction, i.e., the largest
	 denominator we need */
      ns_step *f; /* fraction */
      for (f = s->next; f; f = f->next)
	{
	  nx_set_aev_l(f);
	  if (!f->next)
	    break;
	}
      
      /* traverse up setting all the n/d to have the same
	 denominator */
      ns_step *b; /* bigger */
      for (b = f->prev; b; b = b->prev)
	nx_set_aev_r(b);
    }
}

/* Set the aev field of the step based on the step to the left.  The
 * mult field gives the number of the current step that make up the
 * left step.
 *
 * 1 * x = 60 * y = 3 * z => 1/1 = 1/60 = 1/3*1/60 = 1/180
 */
static void
nx_set_aev_l(ns_step *s)
{
  ns_step *l = s->prev;
  unsigned long long n = s->mult.n * l->mult.n;
  int d = s->mult.d * l->mult.d;
  s->aev.n = n;
  s->aev.d = d;
  printf("%llu/%d %s in %llu/%d %s\n", n, d, s->unit, l->mult.n, l->mult.d, l->unit);
}

static void
nx_set_aev_r(ns_step *s)
{
  ns_step *l = s->next;
  unsigned long long n = s->mult.n * l->mult.n;
  int d = s->mult.d * l->mult.d;
  s->aev.n = n;
  s->aev.d = d;
  printf("%llu/%d %s in %llu/%d %s\n", n, d, s->unit, l->mult.n, l->mult.d, l->unit);
}
