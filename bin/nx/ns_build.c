#include <oraccsys.h>
#include "ns.h"

int build_trace = 1;

void
nsb_sys(uchar *t)
{
  nxp->sys = memo_new(nxp->m_sys);
  nxp->sys->name = t;
  ++nxp->nsys;
  if (build_trace)
    printf("nsb_sys: sys %s is sys %d\n", nxp->sys->name, nxp->nsys);
}

void
nsb_equiv(uchar *b, uchar *c)
{
  nxp->sys->base = b;
  nxp->sys->conv = c;
  if (build_trace)
    printf("nsb_equiv: sys %s has base %s and conv %s\n", nxp->sys->name, nxp->sys->base, nxp->sys->conv);
}

void
nsb_mult(nx_num*n, uchar *m)
{
  n->n = atoi((char*)m);
  char *slash = strchr((char*)m, '/');
  if (slash)
    n->d = atoi(++slash);
  else
    n->d = 1;
  if (build_trace)
    printf("nsb_mult: num %s has numerator %llu and denominator %d\n", m, n->n, n->d);
}

void
nsb_step(uchar *m, uchar *u)
{
  nx_step *s = memo_new(nxp->m_step);

  if (build_trace)
    printf("nsb_step: step has mult %s and unit %s\n", m, u);
  
  nsb_mult(&s->mult, m);
  s->unit = u;
  s->sys = nxp->sys;
  if (nxp->sys->steps)
    {
      nxp->sys->last->next = s;
      s->prev = nxp->sys->last;
      nxp->sys->last = s;
    }
  else
    nxp->sys->last = nxp->sys->steps = s;  
}

static void
nsb_inst_add(nx_inst *i)
{
  if (nxp->sys->last->insts)
    {
      nxp->sys->last->last->next = i;
      i->prev = nxp->sys->last->last;
      nxp->sys->last->last = i;
    }
  else
    nxp->sys->last->last = nxp->sys->last->insts = i;
}

void
nsb_inst_g(uchar *g, uchar *n, uchar *u)
{
  nx_inst *i = memo_new(nxp->m_inst);
  i->text = g;
  i->unit = u;
  nsb_mult(&i->count, n);
  nsb_inst_add(i);
  if (build_trace)
    printf("nsb_inst_g: inst has text %s => count %llu/%d and unit %s\n", i->text, i->count.n, i->count.d, i->unit);
}

void
nsb_inst_u(uchar *x, uchar *g, uchar *u)
{
  nx_inst *i = memo_new(nxp->m_inst);
  i->text = g;
  i->unit = u;
  i->a_or_d = tolower(*x);
  nsb_inst_add(i);
  if (build_trace)
    printf("nsb_inst_u: inst has text %s with system %s=>%c and unit %s\n", i->text, x, i->a_or_d, i->unit);  
}
