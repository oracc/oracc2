#include <stdlib.h>
#include <memo.h>
#include <mesg.h>
#include <tree.h>
#include <stck.h>
#include "gdl.h"
#include "gdlstate.h"
#include "gdl.tab.h"

int gdl_break_pending = 0, gdl_state_pending = 0;

/* gdl_break_XXX() are in gdl.h */

#define gdl_state_peek() stck_peek(state_stack)
#define gdl_state_pop() stck_pop(state_stack)
#define gdl_state_push(x) stck_push(state_stack,x)

static Memo *mgstck;

static int o_c_map[] = 
  { '<',	'>',
    L_ang_par,	R_ang_par,
    L_cur_par,	R_cur_par,
    L_dbl_ang,	R_dbl_ang,
    L_dbl_cur,	R_dbl_cur,
    '[',       	']',
    L_uhs,     	R_uhs,
    L_lhs,     	R_lhs,
    '(',	')',
    CLP, 	CRP,
    '{',	'}',
    -1
  };

static struct s_o_c { int tok; const char *str; } s_o_c_map[] =
  {
    { '<', "<" },
    { '>', ">" },
    { '{', "{" },
    { '}', "}" },
    { L_ang_par, "<(" },
    { R_ang_par, ")>" },
    { L_cur_par, "{(" },
    { R_cur_par, ")}" },
    { L_dbl_ang, "<<" },
    { R_dbl_ang, ">>" },
    { L_dbl_cur, "{{" },
    { R_dbl_cur, "}}" },
    { '[', "[" },
    { ']', "]" },
    { L_uhs, "[#" },
    { R_uhs, "#]" },
    { L_lhs, "#[" },
    { R_lhs, "]#" },
    { '(', "(" },
    { ')', ")" },
    { CLP , "(" },
    { CRP , ")" },
    { -1 , NULL }
  };

static int c_of_o[END];
static int o_of_c[END];
static const char *s_of_oc[END];

Stck *state_stack = NULL, *break_stack = NULL;

#define OC_ALLOC	4

void
gdl_balance_init(void)
{
  int i;

  if (state_stack)
    {
      stck_reset(break_stack);
      stck_reset(state_stack);
      memo_reset(mgstck);
      return;
    }
    
  for (i = 0; o_c_map[i] > 0; ++i)
    {
      if (i%2)
	o_of_c[o_c_map[i]] = o_c_map[i-1];
      else
	c_of_o[o_c_map[i]] = o_c_map[i+1];      
    }
  
  for (i = 0; s_o_c_map[i].tok != -1; ++i)    
    s_of_oc[s_o_c_map[i].tok] = s_o_c_map[i].str;

  break_stack = stck_init(OC_ALLOC);
  state_stack = stck_init(OC_ALLOC);
  if (!mgstck)
    mgstck = memo_init(sizeof(Gstck), 16);
}

void
gdl_balance_term(void)
{
  stck_term(break_stack);
  stck_term(state_stack);
  memo_term(mgstck);
  mgstck = NULL;
  break_stack = state_stack = NULL;
}

Gstck*
gstck_new(int i)
{
  Gstck *gp = memo_new(mgstck);
  gp->i = i;
  return gp;
}

void
gdl_break_node(Node *np)
{
#if 0
  /* Now that break is handled in lex we need to let this go on the
     g:[rR] and then move it up to g:n later if necessary */
  if ('r' == np->name[2] || 'R' == np->name[2])
    np = np->rent;
#endif
  intptr_t p = gdl_break_peek();
  if (p > 0)
    gstck_np(p) = np;
  gdl_break_pending = 0;
}

void
gdl_state_node(Node *np)
{
  int i;
  for (i = state_stack->top; i >= 0; --i)
    {
      intptr_t p = state_stack->stack[i];
      if (gstck_np(p) == NULL)
	gstck_np(p) = np;	  
      else
	break;
    }
}

/* return 0 on OK; 1 on error */
intptr_t
gdl_balance_break_c(Mloc mlp, int tok)
{
  intptr_t ret = 0;
  /* if it's a closer, check the stack for a match */
  if (o_of_c[tok])
    {
      intptr_t p = gdl_break_peek();
      if (-1 == p)
	{
	  /* nothing on the stack, superfluous closer */
	  if (gdl_legacy_pending && bracket_data[gdl_legacy_o].tok == o_of_c[tok])
	    {
	      gdl_legacy_pending = 0;
	      gdl_legacy_o = 0;
	      rs_no(gs_damaged|gs_lost);
	    }
	  else
	    {
	      mesg_verr(&mlp, "unopened closer '%s'", s_of_oc[tok]);
	    }
	  ret = -1;
	}
      else if (gstck_i(p) != o_of_c[tok])
	{
	  /* mismatched opener/closer */
	  mesg_verr(&mlp, "mismatched brackets: found '%s' but expected '%s' to close '%s'",
		    s_of_oc[tok], s_of_oc[c_of_o[gstck_i(p)]], s_of_oc[gstck_i(p)]);
	  ret = -1;
	}
      else
	ret = gdl_break_pop();
    }
  else
    {
      fprintf(stderr, "gdl_balance_break_c: internal error: called with tok=%c\n", tok);
    }
  return ret;
}

/* return -1 on error; Gstck* as intptr_t on success */
intptr_t
gdl_balance_state(Mloc mlp, int tok)
{
  intptr_t ret = 0;
  /* if it's a closer, check the stack for a match */
  if (o_of_c[tok])
    {
      intptr_t p = gdl_state_peek();

      if (-1 == p) /* don't use gstck_i(p) here because -1 is a flag value from gdl_state_peek */
	{
	  /* nothing on the stack, superfluous closer */
	  mesg_verr(&mlp, "unopened closer '%s'", s_of_oc[tok]);
	  ret = -1;
	}
      else if (gstck_i(p) != o_of_c[tok])
	{
	  /* mismatched opener/closer */
	  mesg_verr(&mlp, "mismatched brackets: found closer '%s' but expected '%s'",
		    s_of_oc[tok], s_of_oc[c_of_o[gstck_i(p)]]);
	  ret = -1;
	}
      else
	ret = gdl_state_pop();
    }
  else
    {
      /* for openers push the new opener on the stack */
      gdl_state_push((intptr_t)gstck_new(tok));
      gdl_state_pending = 1;
    }
  return ret;
}

void
gdl_balance_flush(Mloc mlp)
{
  intptr_t tok;
  while ((tok = gdl_break_pop()) != -1)
    mesg_verr(&mlp, "unclosed break opener '%s' [tok=%d]", s_of_oc[gstck_i(tok)], gstck_i(tok));
  while ((tok = gdl_state_pop()) != -1)
    mesg_verr(&mlp, "unclosed state opener '%s' [tok=%d]", s_of_oc[gstck_i(tok)], gstck_i(tok));
  stck_reset(break_stack);
  stck_reset(state_stack);
  memo_reset(mgstck);
}
