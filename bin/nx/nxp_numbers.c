#include <oraccsys.h>
#include "nx.h"

int parse_trace = 1;

const char *nxt_str[] = { "no" , "ng" , "nw" , "nd" , "nc" , "nz" , NULL };

static nx_number **nxp_add_step(nx_number **cand, nx_numtok type, const uchar *tok, const void *data);
static void nxp_badnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data);
static nx_number **nxp_candidates(nx_numtok type, const uchar *tok, const void *data);
static void nxp_unxnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data);
static nx_step *nxp_nx_step(ns_inst *ip, nx_step_type type, const uchar *tok, const void *data, nx_number *num);
static void nxp_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to);
static void nxp_stash_result(nx_result *r, nx_number **cand);
static int nx_sys_step_ok(ns_inst *left, ns_inst *next);

void
nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to)
{
  if (parse_trace)
    nxp_show_start_toks(toks, nptoks, from, to);

  /* the candidate systems for the current */
  nx_number **cand = NULL;
  
  while (from <= to)
    {
      const void *d = (data ? data[from] : NULL);
      /* get the system data for the token */
      if (!cand)
	{
	  /* if this is the first token initialize the candidate systems
	     and add an nx_step to each candidate for the token */
	  if (!(cand = nxp_candidates(nptoks[from], toks[from], d)))
	    {
	      if (nptoks[from] == nxt_ng)
		nxp_badnum(r, nptoks[from], toks[from], d);
	      else
		nxp_unxnum(r, nptoks[from], toks[from], d);
	    }
	  ++from;
	}
      else
	{
	  /* otherwise test each candidate to see if this token fits:
	     if so, add an nx_step for the token to the list
	     if not, invalidate the candidate
	  */
	  nx_number **good = nxp_add_step(cand, nptoks[from], toks[from], d);
	  if (good)
	    {
	      cand = good;
	      ++from;
	    }
	  else
	    {
	      nxp_stash_result(r, cand);
	      /* free(cand); *//* need a different way of freeing cand because they are stashed */
	      cand = NULL;
	      /*++from;*/
	    }
	}
    }
  if (cand)
    {
      nxp_stash_result(r, cand);
      /* free(cand); */
    }
}

static int
nx_sys_step_ok(ns_inst *left, ns_inst *next)
{
  if (!strcmp((ccp)left->step->sys->name, (ccp)next->step->sys->name))
    {
      ns_step *s;
      for (s = left->step->next; s; s = s->next)
	{
	  if (!strcmp((ccp)s->unit, (ccp)next->step->unit))
	    {
	      /* could/should also test s->mult >= next->step->mult */
	      return 1;
	    }
	}
    }
  return 0;
}

static nx_number **
nxp_add_step(nx_number **cand, nx_numtok type, const uchar *tok, const void *data)
{
  ns_inst *ip = hash_find(nxp->ir, tok);
  if (ip)
    {
      int ok = 0, i;
      for (i = 0; cand[i]; ++i)
	{
	  if (cand[i]->sys)
	    {
	      ns_inst *jp;
	      for (jp = ip; jp; jp = jp->ir_next)
		{
		  if (nx_sys_step_ok(cand[i]->last->tok.inst, jp))
		    {
		      nx_step *n = nxp_nx_step(jp, NX_STEP_TOK, tok, data, NULL);
		      cand[i]->last->next = n;
		      cand[i]->last = n;
		      break;
		    }
		}
	      /* if we exhausted jp then the new set of insts doesn't
		 have one that can belong to the cand currently being
		 tested. Invalidate it. */
	      if (!jp)
		cand[i]->sys = NULL;
	      else
		ok = 1;
	    }
	}
      return ok ? cand : NULL;
    }
  else
    return cand;
}

static void
nxp_badnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data)
{
  if (parse_trace)
    printf("nxp_badnum: tok %s typed as %s but not found in num registry\n", tok, nxt_str[type]);

  /* Should produce a diagnostic here */
  
  r->r[r->nr].type = NX_NA;
  r->r[r->nr].nb.type = type;
  r->r[r->nr].nb.tok = tok;
  if (data)
    r->r[r->nr].nb.data = data;
  ++r->nr;
}

static nx_number **
nxp_candidates(nx_numtok type, const uchar *tok, const void *data)
{
  nx_number **spp = NULL;
  ns_inst *ip = hash_find(nxp->ir, tok);
  if (ip)
    {
      int i;
      ns_inst *jp;
      for (i = 0, jp=ip; jp; jp = jp->ir_next)
	++i;
      spp = calloc(i+1, sizeof(nx_number**));
      for (i = 0, jp=ip; jp; jp = jp->ir_next, ++i)
	{
	  spp[i] = memo_new(nxp->m_nx_number);
	  spp[i]->steps = nxp_nx_step(jp, NX_STEP_TOK, tok, data, NULL);
	  spp[i]->last = spp[i]->steps;
	  spp[i]->sys = spp[i]->steps->tok.inst->step->sys;
	}
      spp[i] = NULL;

      if (parse_trace)
	{
	  printf("ns_sys cand for %s:", tok);
	  int j;
	  for (j = 0; j < i; ++j)
	    printf(" %s", spp[j]->sys->name);
	  printf("\n\n");
	}
    }
  return spp;
}

static void
nxp_unxnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data)
{
  if (parse_trace)
    printf("nxp_nonnum: unexpected to find tok %s typed as %s outside num\n", tok, nxt_str[type]);

  r->r[r->nr].type = NX_NA;
  r->r[r->nr].nb.type = type;
  r->r[r->nr].nb.tok = tok;
  if (data)
    r->r[r->nr].nb.data = data;
  ++r->nr;
}

static nx_step *
nxp_nx_step(ns_inst *ip, nx_step_type type, const uchar *tok, const void *data, nx_number *num)
{
  nx_step *sp = memo_new(nxp->m_nx_step);
  sp->type = type;

  if (NX_STEP_TOK == type)
    {
      sp->tok.tok = tok;
      sp->tok.data = data;
      sp->tok.inst = ip;
    }
  else
    {
      sp->num = num;
    }
  return sp;
}

static void
nxp_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to)
{
  int f = from;
  printf("nxp:\ntoks:");
  while (from <= to)
    printf("\t%s", toks[from++]);
  printf("\ntypes:");
  from = f;
  while (from <= to)
    printf("\t%s", nxt_str[nptoks[from++]]);
  printf("\n\n");
}

static void
nxp_stash_result(nx_result *r, nx_number **cand)
{
  nx_restok *res = &r->r[r->nr++];
  res->type = NX_NU;
  res->nu = cand;
}
