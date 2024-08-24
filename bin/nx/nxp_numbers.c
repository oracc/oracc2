#include <oraccsys.h>
#include "nx.h"

int parse_trace = 1;

const char *nxt_str[] = { "no" , "ng" , "nw" , "nd" , "nc" , "nz" , NULL };

static int nxp_add_step(nx_number **cand, nx_numtok type, const uchar *tok, const void *data);
static void nxp_badnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data);
static nx_number **nxp_candidates(nx_numtok type, const uchar *tok, const void *data, int *ncand);
static nx_number **nxp_merge_unit(nx_result *r, ns_inst *ip, const void *data, int *ncand);
static nx_step *nxp_nx_step(ns_inst *ip, nx_step_type type, const uchar *tok, const void *data, nx_number *num);
static nx_number **nxp_remove_invalid(nx_number **c, int *ncand);
static void nxp_stash_result(nx_result *r, nx_number **cand);
static int nxp_sys_step_ok(ns_inst *left, ns_inst *next);
static void nxp_unxnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data);

void
nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to)
{
  if (parse_trace)
    nxd_show_start_toks(toks, nptoks, from, to);

  /* the candidate systems for the current */
  nx_number **cand = NULL;
  int ncand = 0;
  
  while (from <= to)
    {
      const void *d = (data ? data[from] : NULL);
      /* get the system data for the token */
      if (!cand)
	{
	  if (nptoks[from] == nxt_nw)
	    {
	      ns_inst *ip = hash_find(nxp->ir, toks[from]);
	      if (ip)
		{
		  if (parse_trace)
		    nxd_show_inst(toks[from], ip);
		  nx_number **m;
		  /* if the last result was a number that can qualify this, merge the two */
		  if (ip->step->a_or_d && (m = nxp_merge_unit(r, ip, d, &ncand)))
		    {
		      cand = m;
		      printf("nxp: unit %s merged with num\n", toks[from]);
		    }
		  else
		    nxp_unxnum(r, nptoks[from], toks[from], d);
		}
	      else
		nxp_unxnum(r, nptoks[from], toks[from], d);
	      ++from;
	    }
	  else
	    {
	      /* if this is the first token initialize the candidate systems
		 and add an nx_step to each candidate for the token */
	      if (!(cand = nxp_candidates(nptoks[from], toks[from], d, &ncand)))
		{
		  if (nptoks[from] == nxt_ng)
		    nxp_badnum(r, nptoks[from], toks[from], d);
		  else
		    nxp_unxnum(r, nptoks[from], toks[from], d);
		}
	      ++from;
	    }
	}
      else
	{
	  /* otherwise test each candidate to see if this token fits:
	     if so, add an nx_step for the token to the list
	     if not, invalidate the candidate
	  */
	  int good = nxp_add_step(cand, nptoks[from], toks[from], d);
	  if (good)
	    {
	      ++from;
	      cand = nxp_remove_invalid(cand, &ncand);
	    }
	  else
	    {
	      /* unset the invalid flag because remain cand are valid
		 up to this point */
	      int i;
	      for (i = 0; cand[i]; ++i)
		cand[i]->invalid = 0;
	      nxp_stash_result(r, cand);
	      cand = NULL;
	      /* free(cand); *//* need a different way of freeing cand because they are stashed */
	      /* Don't increment from; we have stashed everything so
		 far and now we reread the current token */
	    }
	}
    }
  if (cand)
    {
      nxp_stash_result(r, cand);
      /* free(cand); */
    }
}

/* overwrite invalid entries with valid ones */
static nx_number **
nxp_remove_invalid(nx_number **c, int *ncand)
{
  nx_number **n = calloc(*ncand, sizeof(nx_number *));
  int i=0, nnew=0;
  
  while (i < *ncand)
    {
      if (!c[i]->invalid)
	n[nnew++] = c[i];
      ++i;
    }
  n[nnew] = NULL;
  *ncand = nnew;
  free(c);
  return n;
}

static int
nxp_sys_step_ok(ns_inst *left, ns_inst *next)
{
  if (parse_trace)
    {
      printf("sys_step: left="); nxd_show_inst(NULL, left);
      printf("sys_step: next="); nxd_show_inst(NULL, next);
      printf("sys_step: left name %s vs next name %s\n", left->step->sys->name, next->step->sys->name);
    }
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

/* We remove invalid cand as we go along so anything that is passed to
   nxp_add_inst is a valid cand up to here */
static int
nxp_add_inst(nx_number **cand, ns_inst *ip, const void *data)
{
  int ok = 0, i;
  for (i = 0; cand[i]; ++i)
    {
      ns_inst *jp;
      for (jp = ip; jp; jp = jp->ir_next)
	{
	  if (nxp_sys_step_ok(cand[i]->last->tok.inst, jp))
	    {
	      nx_step *n = nxp_nx_step(jp, NX_STEP_TOK, jp->text, data, NULL);
	      cand[i]->last->next = n;
	      cand[i]->last = n;
	      break;
	    }
	}
      /* if we exhausted jp then the new set of insts doesn't
	 have one that can belong to the cand currently being
	 tested. Invalidate it. */
      if (!jp)
	cand[i]->invalid = 1;
      else
	++ok;
    }
  return ok;
}

static int
nxp_add_step(nx_number **cand, nx_numtok type, const uchar *tok, const void *data)
{
  ns_inst *ip = hash_find(nxp->ir, tok);
  if (ip)
    {
      if (parse_trace)
	nxd_show_inst(tok, ip);
      return nxp_add_inst(cand, ip, data);
    }
  else
    return 0;
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
nxp_candidates_inst(ns_inst *ip, const void *data, int *ncand)
{
  nx_number **spp = NULL;
  int i;
  ns_inst *jp;
  for (i = 0, jp=ip; jp; jp = jp->ir_next)
    ++i;
  spp = calloc(i+1, sizeof(nx_number**));
  for (i = 0, jp=ip; jp; jp = jp->ir_next, ++i)
    {
      spp[i] = memo_new(nxp->m_nx_number);
      spp[i]->steps = nxp_nx_step(jp, NX_STEP_TOK, ip->text, data, NULL);
      spp[i]->last = spp[i]->steps;
      spp[i]->sys = spp[i]->steps->tok.inst->step->sys;
    }
  spp[i] = NULL;
  *ncand = i;
  if (parse_trace)
    {
      printf("ns_sys cand for %s:", ip->text);
      int j;
      for (j = 0; j < i; ++j)
	printf(" %s", spp[j]->sys->name);
      printf("\n\n");
    }
  return spp;
}

static nx_number **
nxp_candidates(nx_numtok type, const uchar *tok, const void *data, int *ncand)
{
  ns_inst *ip = hash_find(nxp->ir, tok);
  if (ip)
    {
      if (parse_trace)
	nxd_show_inst(tok, ip);
      return nxp_candidates_inst(ip, data, ncand);
    }
  return NULL;
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
nxp_stash_result(nx_result *r, nx_number **cand)
{
  nx_restok *res = &r->r[r->nr++];
  res->type = NX_NU;
  res->nu = cand;
}

/* This routine returns NULL if there is no merge.
 *
 * If there is a merge it returns an array of nx_numbers which is the
 * new candidate set.
 */
static nx_number **
nxp_merge_unit(nx_result *r, ns_inst *ip, const void *data, int *ncand)
{
  if (r->nr > 0)
    {
      if (r->r[r->nr-1].type == NX_NU)
	{
	  nx_number **nu = r->r[r->nr-1].nu;
	  int i;
	  char A_OR_D = toupper(ip->step->a_or_d);
	  for (i = 0; nu[i]; ++i)
	    {
	      if (nu[i]->sys->name[1] == A_OR_D)
		{
		  /* set the matching sys-number to be the only one in the array */
		  if (i > 0)
		    nu[1] = NULL;
		  else
		    {
		      nu[0] = nu[i];
		      nu[1] = NULL;
		    }
		  /* can the ip's unit belong to a penultimate number ? */
		  if (r->nr > 1 && r->r[r->nr-2].type == NX_NU)
		    {
		      nx_number **ret_nu = r->r[r->nr-2].nu;
		      int ret = nxp_add_inst(ret_nu, ip, data);
		      if (ret)
			{
			  ret_nu = nxp_remove_invalid(ret_nu, ncand);
			  nu[0]->unit = ret_nu[0]->last;
			  r->nr -= 2; /* remove the number and the system from the list */
			  nx_step *nnum = nxp_nx_step(ip, NX_STEP_NUM, ip->text, data, nu[0]);
			  int i;
			  /* Now overwrite all the last steps in the
			     remaining cand with the num step */
			  for (i = 0; ret_nu[i]; ++i)
			    {
			      /* if num->last were an ns_step** we wouldn't have to do this ... */
			      nx_step*np = ret_nu[i]->steps;
			      while (np->next && np->next->next)
				np = np->next;
			      np->next = ret_nu[i]->last = nnum;
			    }
			  return ret_nu;
			}
		    }
		  /* if we reach this the num+unit can't belong to
		   * a preceding num; create a new cand list from the
		   * unit, reset all the cand last steps to be
		   * num+unit and return it for further parsing
		   */
		  
		}
	    }
	}
    }
  return NULL;
}

