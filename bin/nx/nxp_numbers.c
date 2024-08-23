#include <oraccsys.h>
#include "nx.h"

int parse_trace = 1;
static void nxp_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to);

void
nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to)
{
  if (parse_trace)
    nxp_show_start_toks(toks, nptoks, from, to);

  /* the candidate systems for the current */
  nx_steps **cand = NULL;
  
  while (from < to)
    {
      /* get the system data for the token */
      if (!cand)
	{
	  /* if this is the first token initialize the candidate systems
	     and add an nx_step to each candidate for the token */
	  if (!(cand = nxp_candidates(nptoks[from], toks[from], data[from])))
	    {
	      nxp_add_bad(nptoks[from], toks[from], data[from]);
	      ++from;
	    }
	}
      else
	{
	  /* otherwise test each candidate to see if this token fits:
	     if so, add an nx_step for the token to the list
	     if not, invalidate the candidate
	  */
	  nx_steps **good = nxp_add_step(cand, nptoks[from], toks[from], data[from]);
	  if (good)
	    {
	      cand = good;
	      ++from;
	    }
	  else
	    {
	      /* we are at the end of a number; stash it in the result and
		 then continue if there are more tokens */
	      from = nxp_stash_result(r, cand, from);
	      free(cand);
	      cand = NULL;
	    }
	}
    }
  
}

static nx_steps **
nxp_candidates(nx_numtok type, const uchar *tok, const void *data)
{
  nx_step **spp = NULL;
  ns_inst *ip = hash_find(nxp->ir, tok);
  if (ip)
    {
      int i;
      for (i = 0; ip; ip = ip->next)
	++i;
      spp = calloc(i+1, sizeof(nx_step*));
      for (i = 0; ip; ip = ip->next)
	spp[i++] = nxp_nx_step(ip, NX_STEP_TOK, tok, data, NULL);
    }
  return spp;
}

static nx_step *
nxp_nx_step(ns_inst *ip, nx_step_type type, const uchar *tok, const void *data, nx_number *num)
{
  nx_step *sp = memo_new(nxp->m_nx_step);
  sp->type = type;

  if (NX_STEP_TOK == nx_step_type)
    {
      sp.tok = memo_new(nxp->m_nx_step_tok);
      sp.tok->tok = tok;
      sp.tok->data = data;
      sp.tok->inst = ip;
    }
  else
    {
      sp.num = num;
    }
  return sp;
}

static const char *nxt_str[] = { "no" , "ng" , "nw" , "nd" , "nc" , "nz" , NULL };
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
