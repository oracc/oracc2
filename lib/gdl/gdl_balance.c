#include <stdlib.h>
#include <memo.h>
#include <mesg.h>
#include <tree.h>
#include <stck.h>
#include "gdl.h"
#include "gdl.tab.h"

int gdl_break_pending = 0, gdl_state_pending = 0;

#define gdl_break_peek() stck_peek(break_stack)
#define gdl_break_pop() stck_pop(break_stack)
#define gdl_break_push(x) stck_push(break_stack,x)

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

#if 1
Stck *state_stack = NULL, *break_stack = NULL;
#else
int *state_stack = NULL, *break_stack = NULL;
static int break_alloced = 0, state_alloced = 0;
static int break_top = -1, state_top = -1;
#endif

#define OC_ALLOC	4

void
gdl_balance_init(void)
{
  int i;

  if (state_stack)
    {
#if 1
      stck_reset(break_stack);
      stck_reset(state_stack);
      memo_reset(mgstck);
#else
      break_top = state_top = -1;
#endif
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

#if 1
  break_stack = stck_init(OC_ALLOC);
  state_stack = stck_init(OC_ALLOC);
  if (!mgstck)
    mgstck = memo_init(sizeof(Gstck), 16);
#else
  break_stack = calloc((break_alloced = OC_ALLOC), sizeof(int));
  state_stack = calloc((state_alloced = OC_ALLOC), sizeof(int));
#endif
}

void
gdl_balance_term(void)
{
#if 1
  stck_term(break_stack);
  stck_term(state_stack);
  memo_term(mgstck);
  mgstck = NULL;
#else
  free(break_stack);
  free(state_stack);
#endif
  break_stack = state_stack = NULL;
#if 0
  break_top = state_top = -1;
#endif
}

static Gstck*
gstck_new(int i)
{
  Gstck *gp = memo_new(mgstck);
  gp->i = i;
  return gp;
}

void
gdl_break_node(Node *np)
{
  intptr_t p = gdl_break_peek();
  if (p > 0)
    gstck_np(p) = np;
  gdl_break_pending = 0;
}

void
gdl_state_node(Node *np)
{
  intptr_t p = gdl_state_peek();
  if (p > 0)
    gstck_np(p) = np;
  gdl_state_pending = 0;
}

/* return 0 on OK; 1 on error */
intptr_t
gdl_balance_break(Mloc mlp, int tok)
{
  intptr_t ret = 0;
  /* if it's a closer, check the stack for a match */
  if (o_of_c[tok])
    {
      intptr_t p = gdl_break_peek();
      if (-1 == gstck_i(p))
	{
	  /* nothing on the stack, superfluous closer */
	  mesg_verr(&mlp, "unopened closer '%s'", s_of_oc[tok]);
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
      /* for openers push the new opener on the stack */
      gdl_break_push((intptr_t)gstck_new(tok));
      gdl_break_pending = 1;
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

      if (-1 == gstck_i(p))
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
    mesg_verr(&mlp, "unclosed opener '%s' [tok=%d]", s_of_oc[gstck_i(tok)], gstck_i(tok));
  while ((tok = gdl_state_pop()) != -1)
    mesg_verr(&mlp, "unclosed opener '%s' [tok=%d]", s_of_oc[gstck_i(tok)], gstck_i(tok));
#if 1
  stck_reset(break_stack);
  stck_reset(state_stack);
  memo_reset(mgstck);
#else
  break_top = state_top = -1;
#endif
}
