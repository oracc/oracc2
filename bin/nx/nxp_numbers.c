#include <oraccsys.h>
#include "nx.h"

int parse_trace = 0;
int test_data = 0;

const char *nxt_str[] = { "no" , "ng" , "nw" , "nv", "nd" , "nc" , "na" , "nz" , "ne" , "gc" , NULL };

static int nxp_add_inst(nx_number **cand, ns_inst *ip, nx_numtok type, const void *data);
static int nxp_add_step(nx_number **cand, nx_numtok type, const uchar *tok, const void *data);
static void nxp_badnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data);
/*static nx_number **nxp_candidates(nx_numtok type, const uchar *tok, const void *data, int *ncand);*/
static nx_number **nxp_candidates_inst(ns_inst *ip, nx_numtok type, const void *data, int *ncand);
static int nxp_ends_with_guru7(nx_result *r);
static nx_number ** nxp_implicit_gur(nx_result *r, ns_inst *ip, nx_numtok type, const void *data, int *ncand);
static int nxp_last_has_Sa(nx_result *r);
static nx_number **nxp_merge_unit(nx_result *r, ns_inst *ip, nx_numtok type, const void *data, int *ncand);
static nx_step *nxp_nx_step(ns_inst *ip, nx_step_type type, const uchar *tok, nx_numtok toktype, const void *data, nx_number *num);
static nx_number **nxp_remove_invalid(nx_number **c, int *ncand);
static void nxp_stash_result(nx_result *r, nx_number **cand);
static int nxp_system_C(ns_inst *ip);
static int nxp_sys_step_ok(ns_inst *left, ns_inst *next);
static void nxp_unxnum(nx_result *r, nx_numtok type, const uchar *tok, const void *data);
static int nxp_add_det(nx_number **cand, const uchar *tok, const void *data);

void
nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to)
{
  if (parse_trace)
    nxd_show_start_toks(toks, nptoks, from, to);

  /* the candidate systems for the current */
  nx_number **cand = NULL;
  int ncand = 0, orig_from = from;
  
  while (from <= to)
    {
      const void *d = (data ? data[from] : NULL);
      /* get the system data for the token */
      if (!cand)
	{
	  if (nptoks[from] == nxt_ng)
	    {
	      /* if this is the first token initialize the candidate systems
		 and add an nx_step to each candidate for the token */
	      ns_inst *ip = hash_find(nxp->ir, toks[from]);
	      if (ip)
		{
		  if (parse_trace)
		    nxd_show_inst(ip->text, ip);
		  
		  if (nxp_system_C(ip) && nxp_last_has_Sa(r))
		    cand = nxp_implicit_gur(r, ip, nptoks[from], d, &ncand);
		  else
		    cand = nxp_candidates_inst(ip, nptoks[from], d, &ncand);
		}
	      else
		nxp_badnum(r, nptoks[from], toks[from], d);
	      /*if (!(cand = nxp_candidates(nptoks[from], toks[from], d, &ncand)))*/
	      ++from;
	    }
	  else if (nptoks[from] == nxt_nw)
	    {
	      ns_inst *ip = hash_find(nxp->ir, toks[from]);
	      if (ip)
		{
		  if (parse_trace)
		    nxd_show_inst(toks[from], ip);
		  nx_number **m;
		  /* if the last result was a number that can qualify this, merge the two */
		  if (ip->step->a_or_d && (m = nxp_merge_unit(r, ip, nptoks[from], d, &ncand)))
		    {
		      cand = m;
		      /*printf("nxp: unit %s merged with num\n", toks[from]);*/
		    }
		  else
		    nxp_unxnum(r, nptoks[from], toks[from], d);
		}
	      else
		nxp_unxnum(r, nptoks[from], toks[from], d);
	      ++from;
	    }
	  else if (nptoks[from] == nxt_nz)
	    ++from;
	  else
	    {
	      nxp_unxnum(r, nptoks[from], toks[from], d);
	      ++from;
	    }
	}
      else
	{
	  if (nptoks[from] == nxt_nd)
	    {
	      int good = nxp_add_det(cand, toks[from], d);
	      if (good)
		{
		  ++from;
		  cand = nxp_remove_invalid(cand, &ncand);
		}
	      else
		{
		  int i;
		  for (i = 0; cand[i]; ++i)
		    cand[i]->invalid = 0;
		  nxp_stash_result(r, cand);
		  cand = NULL;
		  /* don't increment from */
		}
	    }
	  else if (nptoks[from] == nxt_nz)
	    {
	      if (cand)
		{
		  nxp_stash_result(r, cand);
		  cand = NULL;
		}
	      ++from;
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
    }
  if (cand)
    {
      nxp_stash_result(r, cand);
      /* free(cand); */
    }

  if (test_data)
    {
      if (nxp->testfp)
	nxr_testdata(nxp->testfp, r, nptoks, toks, data, orig_from, to);
    }
}

static nx_number **
nxp_implicit_gur(nx_result *r, ns_inst *ip, nx_numtok type, const void *data, int *ncand)
{
  nx_number **m = NULL;

  /*printf("nxp_implicit_gur\n");*/
  
  /* remove all the non-Sa steps from the preceding number */
  nx_number **nu = r->r[r->nr-1].nu;
  --r->nr; /* remove the Sa from the results */
  
  int i;
  for (i = 0; nu[i]; ++i)
    {
      if (nu[i]->sys->name[1] != 'S' || nu[i]->sys->name[2] != 'a')
	nu[i]->invalid = 1;
    }
  nu = nxp_remove_invalid(nu, ncand);
    
  /* get a virtual gur step and merge it with the preceding number */
  ns_inst *gp = hash_find(nxp->ir, (ucp)"(gur)");

  /* create a cand list from (gur) */
  m = nxp_candidates_inst(gp, type, data, ncand);

  /* now create a NUM nx_step for the number with (gur) */
  nx_step *nnum = nxp_nx_step(ip, NX_STEP_NUM, ip->text, type, data, nu[0]);

  /* set the unit for all the Sa-cand to (gur) nx_step */
  for (i = 0; nu[i]; ++i)
    nu[i]->unit = m[0]->last;

  /* if there is a previous guru₇ attach nnum to that */
  if (nxp_ends_with_guru7(r))
    {
      /* discard the cand we made for (gur) */
      free(m);
      /* printf("found guru7\n"); */
      /* pull the cand off result to be the new cand */
      m = r->r[r->nr-1].nu;
      --r->nr;
      /* for each ncand attach the nnum as the last step and reset last ptr */
      for (i = 0; m[i]; ++i)
	m[i]->steps->next = m[i]->last = nnum;
    }
  else
    {
      /* set the steps and last for all the cand to the Sa+(gur) nx_step */
      for (i = 0; m[i]; ++i)
	m[i]->steps = m[i]->last = nnum;
    }
  
  /* Now we have re-headed cand to start with the guru₇ or (gur); add
     the C-step that started this off */
  int good = nxp_add_inst(m, ip, type, data);
  if (good)
    m = nxp_remove_invalid(m, ncand);
  /* should probably be more careful--it's unlikely that this could
     fail but is it possible? */
  
  /* return for further parsing */
  return m;
}

static int
nxp_ends_with_guru7(nx_result *r)
{
  if (r->nr > 0 && r->r[r->nr-1].type == NX_NU)
    {
      nx_number **nu = r->r[r->nr-1].nu;
      int i;
      for (i = 0; nu[i]; ++i)
	{
	  if (nu[i]->sys->name[1] == 'C'
	      && !strcmp((ccp)nu[i]->last->num->unit->tok.tok, "guru₇"))
	    return 1;
	}
    }
  return 0;
}
  
static int
nxp_last_has_Sa(nx_result *r)
{
  if (r->nr > 0 && r->r[r->nr-1].type == NX_NU)
    {
      nx_number **nu = r->r[r->nr-1].nu;
      int i;
      for (i = 0; nu[i]; ++i)
	{
	  if (nu[i]->sys->name[1] == 'S' && nu[i]->sys->name[2] == 'a')
	    return 1;
	}
    }
  return 0;
}
  
static int
nxp_system_C(ns_inst *ip)
{
  ns_inst *jp;
  for (jp = ip; jp; jp = jp->next)
    if ('C' == jp->step->sys->name[1])
      return 1;
  return 0;
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
nxp_add_inst(nx_number **cand, ns_inst *ip, nx_numtok type, const void *data)
{
  int ok = 0, i;
  for (i = 0; cand[i]; ++i)
    {
      ns_inst *jp;
      for (jp = ip; jp; jp = jp->ir_next)
	{
	  ns_inst *left = (cand[i]->last->type==NX_STEP_TOK
			   ? cand[i]->last->tok.inst
			   : cand[i]->last->num->unit->tok.inst
			   );
	  if (nxp_sys_step_ok(left, jp))
	    {
	      nx_step *n = nxp_nx_step(jp, NX_STEP_TOK, jp->text, type, data, NULL);
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
      return nxp_add_inst(cand, ip, type, data);
    }
  else
    return 0;
}

static int
nxp_add_det(nx_number **cand, const uchar *tok, const void *data)
{
  int i, ok = 0;
  nx_step *n = NULL;
  for (i = 0; cand[i]; ++i)
    {
      if (cand[i]->sys->det && !strcmp((ccp)cand[i]->sys->det, (ccp)tok))
	{
	  if (!n)
	    n = nxp_nx_step(NULL, NX_STEP_TOK, tok, nxt_nd, data, NULL);
	  cand[i]->last->next = n;
	  cand[i]->last = n;
	  ++ok;
	}
      else
	cand[i]->invalid = 1;
    }
  return ok;
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
nxp_candidates_inst(ns_inst *ip, nx_numtok type, const void *data, int *ncand)
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
      spp[i]->steps = nxp_nx_step(jp, NX_STEP_TOK, ip->text, type, data, NULL);
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

#if 0
static nx_number **
nxp_candidates(nx_numtok type, const uchar *tok, const void *data, int *ncand)
{
  ns_inst *ip = hash_find(nxp->ir, tok);
  if (ip)
    {
      if (parse_trace)
	nxd_show_inst(tok, ip);
      return nxp_candidates_inst(ip, type, data, ncand);
    }
  return NULL;
}
#endif

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
nxp_nx_step(ns_inst *ip, nx_step_type type, const uchar *tok, nx_numtok toktype, const void *data, nx_number *num)
{
  nx_step *sp = memo_new(nxp->m_nx_step);
  sp->type = type;

  if (NX_STEP_TOK == type)
    {
      sp->tok.tok = tok;
      sp->tok.data = data;
      sp->tok.type = toktype;
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
nxp_merge_unit(nx_result *r, ns_inst *ip, nx_numtok type, const void *data, int *ncand)
{
  if (r->nr > 0)
    {
      if (r->r[r->nr-1].type == NX_NU)
	{
	  nx_number **nu = r->r[r->nr-1].nu;
	  int i;
	  char A_OR_D = tolower(ip->step->a_or_d);
	  for (i = 0; nu[i]; ++i)
	    {
	      if (nu[i]->sys->name[1] == 'S' && nu[i]->sys->name[2] == A_OR_D)
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
		      int ret = nxp_add_inst(ret_nu, ip, type, data);
		      if (ret)
			{
			  ret_nu = nxp_remove_invalid(ret_nu, ncand);
			  nu[0]->unit = ret_nu[0]->last;
			  r->nr -= 2; /* remove the number and the system from the list */
			  nx_step *nnum = nxp_nx_step(ip, NX_STEP_NUM, ip->text, type, data, nu[0]);
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
		  --r->nr; /* remove the number from the results */

		  /* create a cand list from the unit-word */
		  nx_number **m = nxp_candidates_inst(ip, type, data, ncand);

		  /* set the unit for all the NUM-cand to (gur) nx_step */
		  for (i = 0; nu[i]; ++i)
		    nu[i]->unit = m[i]->last;

		  /* now create a NUM nx_step for the number and make
		     it the first step in all the cand we got from the
		     unit-word */
		  nx_step *nnum = nxp_nx_step(ip, NX_STEP_NUM, ip->text, type, data, nu[0]);

		  for (i = 0; m[i]; ++i)
		    m[i]->steps = m[i]->last = nnum;

		  return m;
		}
	    }
	}
    }
  return NULL;
}
