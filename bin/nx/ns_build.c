#include <oraccsys.h>
#include "nx.h"

int build_trace = 0;

int nsb_altflag;
extern int nslineno;

void
nsb_sys(uchar *t)
{
  nxp->sys = memo_new(nxp->m_ns_sys);
  nxp->sys->name = t+1;
  hash_add(nxp->hsys, nxp->sys->name, nxp->sys);
  list_add(nxp->lsys, nxp->sys->name);
  nxp->sys->e = hash_create(7);
  list_add(nxp->hashes, nxp->sys->e);
  nxp->sys->ilist = list_create(LIST_SINGLE);
  nxp->sys->lnum = nslineno;
  ++nxp->nsys;
  nsb_altflag = 0;  
  if (build_trace)
    printf("nsb_sys: sys %s is sys %d\n", nxp->sys->name, nxp->nsys);
}

void
nsb_det(uchar *d)
{
  nxp->sys->det = d;
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
nsb_env(uchar *e, uchar *v)
{
  ++e; /* skip $ */
  v[strlen((char*)v)-1] = '\0'; /* remove trailing " */
  memmove(v,v+1,strlen((char*)v)+1);
  hash_add(nxp->sys->e, e, v);
  if (build_trace)
    {
      char s[strlen((const char *)e)+strlen((const char *)v)+5];
      sprintf(s, "$%s=\"%s\"", e, v);
      printf("nsb_env: env %s has key %s and value '%s'\n", s, e, v);
    }
}

void
nsb_mult(nx_num*n, const uchar *m)
{
  n->n = atoi((char*)m);
  if (n->n == 0L)
    n->n = 1L;
  char *slash = strchr((char*)m, '/');
  if (slash)
    n->d = atoi(++slash);
  else
    n->d = 1;
  if (build_trace)
    printf("nsb_mult: num %s has numerator %llu and denominator %d\n", m, n->n, n->d);
}

void
nsb_step(uchar *a, uchar *m, uchar *u)
{
  ns_step *s = memo_new(nxp->m_ns_step);

  if (build_trace)
    printf("nsb_step: step has mult %s and unit %s; altflag=%d\n", m, u, nsb_altflag);
  
  nsb_mult(&s->mult, m);
  if (a)
    {
      s->a_or_d = tolower(*a);
      s->axis = (ccp)a;
    }
  s->unit = u;
  if (strchr((ccp)u+1, '('))
    s->type = nxt_nf;
  else
    s->type = nxp_tok_type(u);
  if (s->type == nxt_nw)
    {
      if (!(s->nwp = nw((ccp)u, strlen((ccp)u))))
	fprintf(stderr, "%s: no word data\n", u);
    }
  else if (s->type == nxt_nn)
    {
      fprintf(stderr, "%s: step unit is not a number.\n", u);
    }
  
  s->sys = nxp->sys;
  if (nsb_altflag)
    {
      nsb_altflag = 0;
      if (nxp->sys->last->alt)
	{
	  ns_step *a;
	  for (a = nxp->sys->last->alt; a->next; a = a->next)
	    ;
	  a->next = s;
	  s->prev = a;
	}
      else
	{
	  nxp->sys->last->alt = s;
	}
    }
  else
    {
      if (nxp->sys->steps)
	{
	  nxp->sys->last->next = s;
	  s->prev = nxp->sys->last;
	  nxp->sys->last = s;
	}
      else
	nxp->sys->last = nxp->sys->steps = s;  
    }
}

static void
nsb_inst_register(ns_inst *i)
{
  list_add(i->step->sys->ilist, i);
  ns_inst *r = hash_find(nxp->ir, i->text);
  if (r)
    {
      if (r->ir_last->ir_next)
	{
	  fprintf(stderr, "r->ir_last->ir_next not null\n");
	}
      else
	{
	  if (build_trace)
	    {
	      printf("inst_register: found %s in hash\n", i->text);
	      printf("inst_register: appending ");
	      nxd_show_inst(NULL, i);
	    }
	  r->ir_last->ir_next = i;
	  r->ir_last = i;
	}
    }
  else
    {
      if (build_trace)
	{
	  printf("inst_register: adding %s ", i->text);
	  nxd_show_inst(NULL, i);
	}
      i->ir_last = i;
      hash_add(nxp->ir, i->text, i);
    }
}

static void
nsb_inst_step(ns_inst *i)
{
  ns_step *sp;
  for (sp = nxp->sys->steps; sp; sp = sp->next)
    if (!strcmp((ccp)i->unit, (ccp)sp->unit))
      {
	i->step = sp;
	break;
      }
  if (!i->step)
    fprintf(stderr, "nsb_inst_step: inst unit %s not found in sys %s\n", i->unit, nxp->sys->name);
}

static void
nsb_inst_add(ns_inst *i, ns_inst_method meth)
{
  if (meth == NS_INST_AUTO)
    i->step = nxp->sys->last;
  else
    nsb_inst_step(i);
    
  /* keep a list of all the insts that can belong to this step of this
     sys; if some insts for this unit were given in the system
     definition data but others weren't the insts may not be in the
     correct sort order */
  if (nxp->sys->last->insts)
    {
      nxp->sys->last->last->next = i;
      i->prev = nxp->sys->last->last;
      nxp->sys->last->last = i;
    }
  else
    nxp->sys->last->last = nxp->sys->last->insts = i;
  
  Hash *h = i->step->sys->i;
  if (!h)
    {
      h = hash_create(100);
      list_add(nxp->hashes, h);
      h = i->step->sys->i = h;
    }
  hash_add(h, i->text, i);
  nsb_inst_register(i);
}

void
nsb_inst_frac(const char *f, ns_inst_method meth)
{
#if 1
  char *buf = (char*)pool_copy((uccp)f, nxp->p);
#else
  char buf[strlen(f)+1];
  strcpy(buf,f);
#endif
  char *slash, *paren;
  slash = strchr(buf, '/');
  if (slash)
    {
      ++slash;
      int n = atoi(buf);
      int d = atoi(slash);
      const char *u = NULL;
      paren = strchr(slash,'(');
      if (paren)
	{
	  u = ++paren;
	  paren = strchr(paren,')');
	  *paren = '\0';
	  if (build_trace)
	    printf("nsb_inst_frac: f=%s; n=%d; d=%d; u=%s\n", f, n, d, u);
	  ns_inst *i = memo_new(nxp->m_ns_inst);
	  i->text = (uccp)f;
	  i->unit = (uccp)u;
	  i->count.n = n;
	  i->count.d = d;
	  nsb_inst_add(i, meth);
	}
    }
}

void
nsb_inst_g(const uchar *g, const uchar *n, const uchar *u, ns_inst_method meth)
{
  ns_inst *i = memo_new(nxp->m_ns_inst);
  i->text = g;
  i->unit = u;
  nsb_mult(&i->count, n);

  nsb_inst_add(i, meth);
  
  if (build_trace)
    printf("nsb_inst_g: inst has text %s => count %llu/%d and unit %s\n", i->text, i->count.n, i->count.d, i->unit);
}

ns_inst *
nsb_inst_u(const uchar *x, uchar *g, uchar *u, ns_inst_method meth)
{
  ns_inst *i = memo_new(nxp->m_ns_inst);
  i->text = g;
  i->unit = u;
  i->axis = (ccp)x;
  i->a_or_d = tolower(*x);
  nsb_inst_add(i, meth);
  if (build_trace)
    printf("nsb_inst_u: inst has text %s with system %s=>%s and unit %s\n", i->text, x, i->axis, i->unit);
  return i;
}

static char *fixed_n[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
static int n_fixed = sizeof(fixed_n)/sizeof(const char *);
static void
nsb_auto_inst_g(ns_sys *sp, int m, const uchar *u)
{
  if (m < n_fixed)
    {
      const char *n = fixed_n[m];
      char g[strlen(n)+strlen((char*)u)+3];
      sprintf(g, "%s(%s)", n, u);
      if (!hash_find(sp->i, (uchar*)g))
	{
	  if (build_trace)
	    printf("nsb_auto_inst: adding %s\n", g);
	  
	  nsb_inst_g(pool_copy((uchar*)g, nspool), (const uchar *)n, u, NS_INST_AUTO);
	}
    }
  else
    {
      fprintf(stderr, "nx: fatal error: m=%d >= n_fixed=%d\n", m, n_fixed);
      exit(1);
    }
}

static void
nsb_wrapup_step(ns_step *stp)
{
  if (stp->a_or_d)
    {
      (void)nsb_inst_u((uccp)stp->axis, stp->unit, stp->unit, NS_INST_AUTO);
    }
  else
    {
      if (strchr((ccp)stp->unit, '/'))
	nsb_inst_frac((ccp)stp->unit, NS_INST_AUTO);
      else if (stp->mult.d == 1)
	{
	  int i = (int)stp->mult.n;
	  int m = 1;
	  do
	    nsb_auto_inst_g(nxp->sys, m, stp->unit);
	  while (++m < i); 
	}
    }
}

void
nsb_wrapup(void)
{
  ns_step *stp;
  for (stp = nxp->sys->steps; stp; stp = stp->next)
    {
      nxp->sys->last = stp;
      nsb_wrapup_step(stp);
      ns_step *alt;
      if (stp->alt)
	{
	  int d = stp->mult.d;
	  for (alt = stp->alt; alt->next; alt = alt->next)
	    {
	      d *= (int)alt->next->mult.d;
	      if (!alt->next)
		break;
	    }
	  
	  int m = d / stp->mult.d;
	  stp->mult.n *= m;
	  stp->mult.d = d;
	  for (alt = stp->alt; alt; alt = alt->next)
	    {
	      m = d / alt->mult.d;
	      alt->mult.n *= m;
	      alt->mult.d = d;
	      alt->aev = alt->mult;
	    }

	  for (alt = stp->alt; alt; alt = alt->next)
	    {
	      nxp->sys->last = alt;
	      nsb_wrapup_step(alt);
	    }
	}
    }
  nx_sys_aevs(nxp->sys);
}
