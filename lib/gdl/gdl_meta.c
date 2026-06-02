#include <stdlib.h>
#include <memo.h>
#include <mesg.h>
#include <tree.h>
#include "gdl.h"
#include "gdlstate.h"
#include "gdl.tab.h"

extern Mloc gdllloc;
#define mlp gdllloc

extern int gdllineno, gdltrace, gdlflextrace;
extern const char *currgdlfile, *gdlfile;

Bracket bracket_data[] = {
  [e_L_squ] = { '[', gs_lost_o, gs_lost, "[" },
  [e_L_uhs] = { L_uhs, gs_damaged_o, gs_damaged, "⸢" },
  [e_L_lhs] = { L_lhs, gs_damaged_o, gs_damaged, "" },

  [e_R_squ] = { ']', gs_lost_c, gs_lost, "]" },
  [e_R_uhs] = { R_uhs, gs_damaged_c, gs_damaged, "⸣" },
  [e_R_lhs] = { R_lhs, gs_damaged_c, gs_damaged, "" },

  [e_L_cur_par] = { L_cur_par, gs_glodoc_o, gs_glodoc, "{(" },
  [e_L_dbl_cur] = { L_dbl_cur, gs_glolin_o, gs_glolin, "{{" },
  [e_L_ang_par_s] = { L_ang_par_s, gs_surro_o, gs_empty, "<(=" },

  [e_R_cur_par] = { R_cur_par, gs_glodoc_c, gs_glodoc, ")}" },
  [e_R_dbl_cur] =  {R_dbl_cur, gs_glolin_c, gs_glolin, "}}" },
  [e_R_ang_par_s] = { R_ang_par_s, gs_surro_c, gs_empty, ")>" },

  [e_L_ang] = { '<', gs_supplied_o, gs_supplied, "<" },
  [e_L_par] = { '(', gs_maybe_o, gs_maybe, "(" },
  [e_L_ang_par] = { L_ang_par, gs_implied_o, gs_implied, "<(" },
  [e_L_dbl_ang] = { L_dbl_ang, gs_excised_o, gs_excised, "<<" },

  [e_R_ang] = { '>', gs_supplied_c, gs_supplied, ">" },
  [e_R_par] = { ')', gs_maybe_c, gs_maybe, ")" },
  [e_R_ang_par] = { R_ang_par, gs_implied_c, gs_implied, ")>" },
  [e_R_dbl_ang] = { R_dbl_ang, gs_excised_c, gs_excised,  ">>" }
};

/* 20260602: Flags are no longer passed to the parser; they are
   recorded here in the state property pointer of the last grapheme seen */
static void
gdl_lex_flag(unsigned const char *f)
{
  if (gdlflextrace)
      fprintf(stderr, "gdl_lex_flag: arg f=%s\n", f);
  if (!lst)
      {
        mesg_vwarning(gdlfile, gdllineno, "misplaced flag `%s'; no grapheme state to set", f);
	return;
      }
  switch (*f)
    {
    case '!':
	bit_set(*lst, gs_f_bang);
	break;
    case '?':
	bit_set(*lst, gs_f_query);
	break;
    case '#':
	bit_set(*lst, gs_f_hash);
	break;
    case '*':
	bit_set(*lst, gs_f_star);
	break;
    case 0xe2:
	switch (f[2])
	    {
	    case 0xa0:
		bit_set(*lst, gs_f_uf1);
		break;
	    case 0xa1:
		bit_set(*lst, gs_f_uf2);
		break;
	    case 0xa2:
		bit_set(*lst, gs_f_uf3);
		break;
	    case 0xa3:
		bit_set(*lst, gs_f_uf4);
		break;
	    default:
		/*can't happen*/
		break;
	    }
	break;
    default:
	mesg_vwarning(gdlfile, gdllineno, "lex field `%c' needs a space before it", *f);
	break;
    }
}

void
gdl_break_o(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/o: %d=%s\n", bt, bp->str);
  (void)gdl_balance_break(mlp, bp->tok);
  /*ret = gdl_meta_node(ytp, "g:z", data);*/
  ps_on(gs_o);
  rs_on(gs_run);
  return ret;
}

void
gdl_break_c(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/c: %d=%s\n", bt, bp->str);
  intptr_t st = gdl_balance_break(mlp, bp->tok);
  if (gstck_i(st) > 0)
    {
      Node *np = gstck_np(st);
      if ('r' == np->name[2])
	np = np->rent;
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakStart", "1");
      if (!gdl_no_xml_ids)
	{
	  Prop *idp = prop_find_kv(np->props, "xml:id", NULL);
	  if (!idp)
	    {
	      if (np->kids)
		{
		  idp = prop_find_kv(np->kids->props, "xml:id", NULL);
		  if (!idp)
		    {
		      if (np->kids->text)
			mesg_verr(&mlp, "gdl_break_c: strange: no xml:id on np or np->kids near `%s'\n", np->kids->text);
		      else
			mesg_verr(&mlp, "gdl_break_c: strange: no xml:id on np or np->kids\n");
		    }
		  else
		    gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakEnd", idp->u.k->v);
		}
	      else
		mesg_verr(&mlp, "gdl_break_c: strange: no xml:id on np type %s\n", np->name);
	    }
	  else
	    gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakEnd", idp->u.k->v);
	}
    }
  bit_set(*lst,gs_c);
  rs_no(gs_run);
}

void
gdl_state_o(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: STATE/o: %d=%s\n", bt, bp->str);
  (void)gdl_balance_state(mlp, bp->tok);
  /*ret = gdl_meta_node(ytp, "g:z", data);*/
  ps_on(gs_o);
  rs_on(gs_run);
  return ret;
}

void
gdl_state_c(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: STATE/c: %d=%s\n", bt, bp->str);
  /*ret =  gdl_meta_node(ytp, "g:z", data);*/
  intptr_t st = gdl_balance_state(mlp, bp->tok);
  if (st > 0) /* st can be -1 if nothing on stack */
    {
      Node *np = gstck_np(st);
      if (!strcmp(np->rent->name, "g:n"))
	np = np->rent;
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:statusStart", "1");
      if (!gdl_no_xml_ids)
	{
	  Prop *idp = prop_find_kv(np->props, "xml:id", NULL);
	  if (idp)
	    gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:statusEnd", idp->u.k->v);
	  else
	    mesg_verr(&mlp, "could not set g:statusEnd--no xml:id on %s node\n", np->name);
	}
    }
  bit_set(*lst,gs_c);
  rs_no(gs_run);
  /*gdl_update_state(lgp, gs_c);*/
  return ret;
}

#undef mlp
