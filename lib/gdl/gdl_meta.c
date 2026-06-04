#include <stdlib.h>
#include <memo.h>
#include <mesg.h>
#include <tree.h>
#include <lng.h>
#include "gdl.h"
#include "gdlstate.h"
#define GDLLTYPE Mloc
#include "gdl.tab.h"

struct lang_context *gdl_lang_context;
const char *gdl_pending_varo = NULL;
const char *word_lang_tag = "sux";

extern Node *lgp; /* last grapheme node pointer */

extern int gdllineno, gdltrace, gdlflextrace;
extern const char *currgdlfile, *gdlfile;

Bracket bracket_data[] = {
  [e_L_squ] = { '[', gs_lost_o, gs_lost, "[" },
  [e_L_uhs] = { L_uhs, gs_damaged_o, gs_damaged, "⸢" },
  [e_L_lhs] = { L_lhs, gs_damaged_o, gs_damaged, "⸤" },

  [e_R_squ] = { ']', gs_lost_c, gs_lost, "]" },
  [e_R_uhs] = { R_uhs, gs_damaged_c, gs_damaged, "⸣" },
  [e_R_lhs] = { R_lhs, gs_damaged_c, gs_damaged, "⸥" },

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
void
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
	bit_set(*lst, gs_f_hash|gs_damaged);
	if (!bit_get(rst, gs_damaged))
	  {
	    rs_on(gs_damaged);
	    bit_set(*lst, gs_f_hash|gs_damaged_o);	    
	  }
	dst = lst;
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
gdl_hc(int force)
{
  if (dst && (force || !bit_get(*lst, gs_damaged)))
    {
      bit_set(*dst, gs_damaged_c);
      if (dst != lst)
	bit_off(*lst, gs_damaged);
      rs_no(gs_damaged);
      dst = NULL;
    }
}

void
gdl_break_o(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/o: %d=%s\n", bt, bp->str);
  (void)gdl_balance_break(gdllloc, bp->tok);
  ps_on(bp->oc);
  rs_on(bp->s);
}

void
gdl_break_c(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/c: %d=%s\n", bt, bp->str);
  intptr_t st = gdl_balance_break(gdllloc, bp->tok);
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
			mesg_verr(&gdllloc, "gdl_break_c: strange: no xml:id on np or np->kids near `%s'\n", np->kids->text);
		      else
			mesg_verr(&gdllloc, "gdl_break_c: strange: no xml:id on np or np->kids\n");
		    }
		  else
		    gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakEnd", idp->u.k->v);
		}
	      else
		mesg_verr(&gdllloc, "gdl_break_c: strange: no xml:id on np type %s\n", np->name);
	    }
	  else
	    gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakEnd", idp->u.k->v);
	}
    }
  bit_set(*lst,bp->oc);
  rs_no(bp->s);
}

void
gdl_state_o(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: STATE/o: %d=%s\n", bt, bp->str);
  (void)gdl_balance_state(gdllloc, bp->tok);
  ps_on(bp->oc);
  rs_on(bp->s);
}

void
gdl_state_c(Bracket_e bt)
{
  Bracket *bp = &bracket_data[bt];
  if (gdltrace)
    fprintf(stderr, "gt: STATE/c: %d=%s\n", bt, bp->str);
  intptr_t st = gdl_balance_state(gdllloc, bp->tok);
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
	    mesg_verr(&gdllloc, "could not set g:statusEnd--no xml:id on %s node\n", np->name);
	}
    }
  bit_set(*lst,bp->oc);
  rs_no(bp->s);
}

int
lgp_guard(const char *s)
{
  if (!lgp)
    {
      mesg_verr(&gdllloc, "can't apply %s because last-grapheme-pointer is NULL", s);
      return 1;
    }
  return 0;
}

void
gdl_indent(void)
{
  if (!lgp_guard("';' = newline"))
    {
      /* if the is a ; e put the g:newline on the word; if it's a;-e
	 put it on the grapheme */
      if (lgp->rent != lgp->tree->curr)
	gdl_prop_kv(lgp->rent, GP_ATTRIBUTE, PG_GDL_INFO, "g:newline", ";");
      else
	gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:newline", ";");
    }
}

void
gdl_note_mark(const char *n)
{
  if (!lgp_guard("^...^ = note mark"))
    gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:notemark", n);    
}

void
gdl_var_mark(Bracket_e type, char *v)
{
  if (e_L_var == type)
    {
      v[strlen(v)-1] = '\0';
      gdl_pending_varo = v+1;
    }
  else if (e_R_var == type)
    {
      v[strlen(v)-1] = '\0';
      if (!lgp_guard(":) = right var mark"))
	gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:varc", v+1);
    }
  else
    {
      v = v+1;
      char *x = strchr(v, ':');
      *x = '\0';
      Tree *gtp = gdl_get_tree();
      Node *vari = tree_node(gtp, NS_GDL, "g:nonw", gtp->curr->depth+1, NULL);
      gdl_prop_kv(vari, GP_ATTRIBUTE, PG_GDL_INFO, "type", "vari");
      vari->text = v;
      (void)node_before(gtp->curr, vari);
    }
}

/* TODO: check lang patterns include trailing /n or /g */
void
gdl_lang_tag(const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: LANG: %s\n", data);
  gdl_lang_context = lang_switch(gdl_lang_context, data, NULL, gdllloc.file, gdllloc.line);
  word_lang_tag = gdl_lang_context->fulltag;
}

/* _..._ -- TO DO */
void
gdl_lang_flip(void)
{
}

#if 0

lang:
		LANG				       	{ ynp = gdl_lang(&@1, ytp,
					       				 gdllval.text); }
	| 	LANG_FLIP	       			/*TODO; ALSO #atf: lang akk _%s_ vel sim*/
	;

#endif
