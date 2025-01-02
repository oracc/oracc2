#include <oraccsys.h>
#include "nx.h"

typedef enum nx_xstep { NX_ASS , NX_COM , NX_DET } nx_xstep;

int parse_trace = 0;
int token_trace = 0;
int test_data = 0;

const char *nxt_str[] = { "nn" , "ng" , "nw" , "nv", "nd" , "nc" , "na" , "nz" , "ne" , "gc" , "gw" , "no",  NULL };

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
static void nxp_append_step(nx_number **cand, nx_xstep where, const uchar *text, nx_numtok type, const void *data);
static int nxp_curve_match(nx_number *n, const uchar *sys);

void
nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to)
{
  if (token_trace)
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
		    cand = m;
		  else
		    nxp_unxnum(r, nptoks[from], toks[from], d);
		}
	      else
		nxp_unxnum(r, nptoks[from], toks[from], d);
	    }
	  else if (nptoks[from] != nxt_nz)
	    nxp_unxnum(r, nptoks[from], toks[from], d);
	  ++from;
	}
      else
	{
	  if (nptoks[from] == nxt_nd
	      || (nptoks[from] == nxt_nw && !strcmp((ccp)toks[from], "gur")))
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
	  else if (nptoks[from] == nxt_nc || nptoks[from] == nxt_gc)
	    {
	      nxp_append_step(cand, NX_COM, toks[from], nptoks[from], data[from]);
	      ++from;
	    }
	  else if (nptoks[from] == nxt_na)
	    {
	      nxp_append_step(cand, NX_ASS, toks[from], nptoks[from], data[from]);
	      ++from;
	    }
	  else if (nptoks[from] == nxt_gw) /* gur-words */
	    {
	      nxp_append_step(cand, NX_DET, toks[from], nptoks[from], data[from]);
	      ++from;
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

  /* remove all the non-Sa steps from the preceding number */
  nx_number **nu = r->r[r->nr-1].nu;
  --r->nr; /* remove the Sa from the results */

  if (parse_trace)
    {
      fprintf(stderr, "\nnxp_implicit_gur entered\n");
      nxd_show_nxnu(nu[0]);
    }
  
  /* Only allow 'a' axis */
  int i;
  for (i = 0; nu[i]; ++i)
    {
      if (nu[i]->sys->name[0] != 'S' || nu[i]->sys->name[1] != 'a')
	nu[i]->invalid = 1;
    }
  nu = nxp_remove_invalid(nu, ncand);

  ns_inst *gp = NULL;

  /* If there is a {gur} det because the text was OB style 1(aš) gur 3(diš) sila₃,
     remove it and create an explicit gur step */
  if (nu[0]->det && !strcmp((ccp)nu[0]->det->tok.tok, "{gur}"))
    gp = hash_find(nxp->ir, (ucp)"gur");
  else
    /* Else get a virtual gur step and merge it with the preceding number */
    gp = hash_find(nxp->ir, (ucp)"(gur)");

  /* create a cand list from gur/(gur) */
  int mcand;
  m = nxp_candidates_inst(gp, type, data, &mcand);

  /* trim the gur cand list by curved/cuneiform aš-num */
  for (i = 0; nu[i]; ++i)
    {
      int j;
      int ok = 0;
      for (j = 0; m[j]; ++j)
	{
	  if (nxp_curve_match(nu[i],m[j]->sys->name))
	    {
	      ok = 1;
	      break;
	    }
	}
      if (!ok)
	m[j]->invalid = 1;
    }
  m = nxp_remove_invalid(m, &mcand);
  
  /* now create a NUM nx_step for the number with gur/(gur) */
  nx_step *nnum = nxp_nx_step(ip, NX_STEP_NUM, ip->text, type, data, nu[0]);

  /* set the unit for all the Sa-cand to gur/(gur) nx_step */
  for (i = 0; nu[i]; ++i)
    nu[i]->unit = m[0]->last;

  /* if there is a previous guru₇ attach nnum to that */
  if (nxp_ends_with_guru7(r))
    {
      /* discard the cand we made for (gur) */
      free(m);
      /* fprintf(stderr, "found guru7\n"); */
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
  
  if (parse_trace)
    {
      nxd_show_nxnu(m[0]);
      fprintf(stderr, "\nnxp_implicit_gur exited\n");
    }
  
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
	  if (nu[i]->sys->name[0] == 'C'
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
	  if (nu[i]->sys->name[0] == 'S' && nu[i]->sys->name[1] == 'a')
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
    if ('C' == jp->step->sys->name[0])
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
      fprintf(stderr, "sys_step: left="); nxd_show_inst(NULL, left);
      fprintf(stderr, "sys_step: next="); nxd_show_inst(NULL, next);
      fprintf(stderr, "sys_step: left name %s vs next name %s\n", left->step->sys->name, next->step->sys->name);
    }
  if (!strcmp((ccp)left->step->sys->name, (ccp)next->step->sys->name))
    {
      ns_step *s;
      for (s = left->step->next; s; s = s->next)
	{
	  if (!strcmp((ccp)s->unit, (ccp)next->step->unit))
	    {
	      /* could/should also test s->mult >= next->step->mult */
	      if (parse_trace)
		fprintf(stderr, "sys_step: match found\n");
	      return 1;
	    }
	}
    }
  if (parse_trace)
    fprintf(stderr, "sys_step: no match\n");
  return 0;
}

static void
nx_append_sub(nx_step **top, nx_step *new)
{
  if (*top)
    {
      nx_step *t = *top;
      while (t->next)
	t = t->next;
      t->next = new;
    }
  else
    *top = new;
}

static void
nxp_append_step(nx_number **cand, nx_xstep where, const uchar *text, nx_numtok type, const void *data)
{
  int i;
  for (i = 0; cand[i]; ++i)
    {
      nx_step *n = nxp_nx_step(NULL, NX_STEP_TOK, text, type, data, NULL);
      nx_step **s = (where == NX_DET ? &cand[i]->det : (where == NX_COM ? &cand[i]->com : &cand[i]->ass));
      nx_append_sub(s, n);
    }
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
	  if (!strcmp((ccp)cand[i]->sys->name,(ccp)jp->step->sys->name)  && nxp_sys_step_ok(left, jp))
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
  char *xtok = (char*)tok;
  if ('{' != *tok)
    {
      xtok = (char*)pool_alloc(strlen((ccp)tok)+3, nxp->p);
      sprintf(xtok, "{%s}", tok);
    }
  for (i = 0; cand[i]; ++i)
    {
      if (cand[i]->sys->det && !strcmp((ccp)cand[i]->sys->det, xtok))
	{
	  if (!n)
	    n = nxp_nx_step(NULL, NX_STEP_TOK, (uccp)xtok, nxt_nd, data, NULL);
	  cand[i]->det = n;
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
  if (1/*parse_trace*/)
    fprintf(stderr, "nxp_badnum: tok %s typed as %s but not found in num registry\n", tok, nxt_str[type]);

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
      fprintf(stderr, "ns_sys cand for %s:", ip->text);
      int j;
      for (j = 0; j < i; ++j)
	fprintf(stderr," %s", spp[j]->sys->name);
      fprintf(stderr, "\n\n");
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
    fprintf(stderr, "nxp_nonnum: unexpected to find tok %s typed as %s outside num\n", tok, nxt_str[type]);

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
  if (parse_trace)
    fprintf(stderr, "nxp_stash_result\n---\n");
  nx_restok *res = &r->r[r->nr++];
  int i;
  for (i = 0; cand[i]; ++i)
    {
      if (cand[i]->steps->type == NX_STEP_TOK)
	{
	  cand[i]->axis = cand[i]->steps->tok.inst->step->sys->name;
	  cand[i]->a_or_d = cand[i]->axis[1];
	}
    }
  res->type = NX_NU;
  res->nu = cand;
}

static int
nxp_curve_match(nx_number *n, const uchar *sys)
{
  if (sys && n->axis)
    {
      int sysc = (strstr((ccp)sys,"@c") ? 1 : 0);
      int axisc = (strstr((ccp)n->axis, "@c") ? 1 : 0);
      return sysc == axisc;
    }
  else if (sys || n->axis)
    return 1;
  else
    return 0;
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
	      if (nu[i]->sys->name[0] == 'S' && nu[i]->sys->name[1] == A_OR_D)
		{
		  /* set the matching sys-number to be the only one in the array */
		  if (parse_trace)
		    fprintf(stderr, "merge_unit: found match\n");
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

		  /* trim cand list to matching axis entries */
		  int i;
		  nx_number **mdest = m;
		  for (i = 0; m[i]; ++i)
		    {
		      int j;
		      int ok = 0;
		      for (j = 0; nu[j]; ++j)
			{
			  if (nu[j]->a_or_d == m[i]->steps->tok.inst->a_or_d)
			    {
			      ok = nxp_curve_match(nu[j],m[i]->sys->name);
			      break;
			    }
			}
		      if (ok)
			*mdest++ = m[i];
		    }
		  *mdest = NULL;

		  /* set the unit for all the NUM-cand to the unit's
		     nx_step; this includes NU that aren't referenced
		     by m but that's ok */
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
  if (parse_trace)
    fprintf(stderr, "merge_unit: no match\n");
  return NULL;
}
