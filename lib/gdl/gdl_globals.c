#include <oraccsys.h>
#include <stck.h>
#include "gdllib.h"
#include "gdltokens.h"

/* multiple gdl instances share this */
int gdlflextrace = 0;
static int gdl_initted = 0;

const char *gdl_lexnames[128] = {
  ['"'] = "!pr",
  ['#'] = "!sv",
  ['='] = "!eq",
  ['@'] = "!cs",
  ['^'] = "!wp",
  ['|'] = "!sn",
  ['~'] = "!sg",
};

int gdl_lexfld[128] = {
  ['"'] = LF_QUOTE,
  ['#'] = LF_HASH,
  ['='] = LF_EQUALS,
  ['@'] = LF_AT,
  ['^'] = LF_CARET, 
  ['|'] = LF_VBAR,
  ['~'] = LF_TILDE,
};

gdl_grapheme_type_p gdl_grapheme_type;
static int gdl_grapheme_type_l(Mloc *mp, char *gt, char **cp);
static int gdl_grapheme_type_s(Mloc *mp, char *g, char **cp);

gdl_delim_p gdl_delim;
gdl_graph_node_p gdl_graph_node;
Node *gdl_delim_l(Tree *ytp, const char *data);
Node *gdl_delim_s(Tree *ytp, const char *data);

gdl_graph_node_p gdl_graph_node;
Node *gdl_graph_node_l(Mloc *locp, Tree *ytp, const char *name, const char *data);
Node *gdl_graph_node_s(Mloc *locp, Tree *ytp, const char *name, const char *data);

/* Public function to call with (0) arg at start of text and with (1)
   arg when #atf: use legacy is encountered */
void
gdl_legacy_lexer(int legacy)
{
  if (legacy)
    {
      gdl_grapheme_type = gdl_grapheme_type_l;
      gdl_delim = gdl_delim_l;
      gdl_graph_node = gdl_graph_node_l;
    }
  else
    {
      gdl_grapheme_type = gdl_grapheme_type_s;
      gdl_delim = gdl_delim_s;
      gdl_graph_node = gdl_graph_node_s;
    }
}	

/* Version of gdl_grapheme_type for use when atf mode is strict, i.e., non-legacy */
static int
gdl_grapheme_type_s(Mloc *mp, char *g, char **cp)
{
  *cp = gdlsave(g);

  if (strlen(g) == 1
      && ('n' == *g || 'N' == *g))
      return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
  else if (isdigit(*g))
    {
      char *d = g+1;
      while (*d && isdigit(*d))
        ++d;
      if (*d == '(')
        return NUMBER;
      else if (!strncmp(g, "00", 2))
	return ZERO;
      else
	return BARENUM;
    }
  else if (sll_is_signlist(g))
    return LISTNUM;
  else
    return GRAPHEME;
}

/* Version of gdl_grapheme_type for use when atf: use legacy is set */
static int
gdl_grapheme_type_l(Mloc *mp, char *gt, char **cp)
{
  char *l = gt;
  char *g = (char*)gdl_unlegacy_str(mp, (uccp)l);
  if (g)
    {
      *cp = gdlsave(g);

      if (strcmp(g,l))
	gdl_legacy_prop = (uccp)l;
      if (strlen(g) == 1
	  && ('n' == *g || 'N' == *g))
	return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
      else if (isdigit(*g))
	{
	  char *d = g+1;
	  while (*d && isdigit(*d))
	    ++d;
	  if (*d == '(')
	    return NUMBER;
	  else if (!strncmp(g, "00", 2))
	    return ZERO;
	  else
	    return BARENUM;
	}
      else if (sll_is_signlist(g))
	return LISTNUM;
      else
	return GRAPHEME;
    }
  else
    {
      return IGNORE;
    }
}

Node *
gdl_delim_l(Tree *ytp, const char *data)
{
  return gdl_delim_s(ytp, data);
}

Node *
gdl_delim_s(Tree *ytp, const char *data)
{
  Node *np = NULL;

  if (gdltrace)
    fprintf(stderr, "gt: DELIM: %s\n", data);

  /* Deferred clean-ups done by gdl_delim */
  if (gdl_group_attach && !c_processing)
    {
      tree_curr(gdl_group_attach->rent);
      gdl_group_attach = NULL;
    }

  gdl_hc(0);
  
  if (c_processing)
    {
      np = tree_add(ytp, NS_GDL, "g:o", ytp->curr->depth, NULL);
      np->mloc = ytp->curr->mloc;
      np->text = data;

      Node *d_attach = lgp ? lgp : np->prev;
      if (d_attach)
	{
	  /* 2026-08-11: don't use g:delim inside compounds because delims are in <g:o> nodes*/
	  /*gdl_prop_kv(d_attach, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", (ccp)data);*/
	  if (!strcmp(np->rent->name, "g:gp") && '.' == *data)
	    gdl_prop_kv(d_attach, GP_ATTRIBUTE, PG_GDL_INFO, "g:sigdelim", "+");	
	}
      ++c_delim_sentinel;
      if (strcmp(np->rent->name, "g:gp")) /* implicit group */
	{
	  if ('.' != *data && (!c_last_implicit_delim || '%' != *data)
	      && '-' != *data && ':' != *data && '+' != *data)
	    {
	      /* c_last_implicit_delim avoids adding successive delims
		 of the same kind to the implicit group list. So,
		 |AŠ&AŠ&AŠ| will only generate one implicit group node
		 to process.  During processing, the group collects
		 all following DELIM+ARG sequences so that AŠ&AŠ&AŠ
		 become the children of the group.  If this is the
		 entire sign no group will be generated because the
		 prev/next ptrs of the first/last elements will be
		 NULL. but in |ŠU₂.AŠ&AŠ&AŠ| a single implicit group
		 will be created for the AŠ sequence. */
	      if (!c_last_implicit_delim || strcmp(c_last_implicit_delim, data))
		{
		  if (!c_implicit_gps)
		    c_implicit_gps = list_create(LIST_SINGLE);
		  list_add(c_implicit_gps, np);
		  c_last_implicit_delim = data;
		}
	      else if (c_last_implicit_delim && !strcmp(c_last_implicit_delim, "×"))
		{
		  /* If this is a second × sign reset the last list
		     entry to point to this so in GU×DIM×KUR the
		     DIM×KUR will be grouped.  Need to check PCSL/OSL
		     to see if deeper × nesting needs to be handled */
		  if (!c_implicit_times_reset)
		    {
		      c_implicit_times_reset = 1;
		      if (c_implicit_gps)
			(void)list_pop(c_implicit_gps);
		      if (!c_implicit_gps)
			c_implicit_gps = list_create(LIST_SINGLE);			
		      list_add(c_implicit_gps, np);
		    }
		}
	    }
	  else
	    c_last_implicit_delim = NULL;
	}
      else /* explicit group */
	{
	  if ('.' != *data && '-' != *data && ':' != *data && '+' != *data)
	    {
	      if (!c_last_explicit_group_node || c_last_explicit_group_node != np->rent)
		{
		  if (!c_explicit_gps)
		    c_explicit_gps = list_create(LIST_SINGLE);
		  list_add(c_explicit_gps, np->rent);
		  c_last_explicit_group_node = np->rent;
		}
	    }
	}
    }
  else
    {
      if (lgp)
	{
	  np = lgp;
	  gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", (ccp)data);
	}
      else
	{
	  np = ytp->curr;
	  mesg_verr(np->mloc, "orphan delimiter '%s'", data);
	}
    }

  return np;
}

Node *
gdl_graph_node_s(Mloc *locp, Tree *ytp, const char *name, const char *data)
{
  Node *np = NULL;
  if (ytp->curr && !ytp->curr->mloc)
    ytp->curr->mloc = locp;
  np = tree_add(ytp, NS_GDL, name, ytp->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  lgp = np;
  lst = prop_state(np, pst|rst);

  if (gs_is(rst, gs_surro))
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:sur", curr_sur_id);

  /*  if (gdl_xmlids && 'r' != name[2] && 'R' != name[2])*/
  if (!gdl_no_xml_ids)
    {
      sprintf(gid_insertp, ".%d", grapheme_id++);
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id",
		  (ccp)pool_copy((uccp)gdl_word_id, gdlpool));
    }
  pst = 0L;
  np->mloc = mloc_mloc(locp);
  /* For r-nodes, leave gdl_pending_varo set and let g:n take it later */
  if (gdl_pending_varo && (('r' != np->name[2] && 'R' != np->name[2])))
    {
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:varo", gdl_pending_varo);
      gdl_pending_varo = NULL;
    }
  /* 20260602: now that we are lifting state from g:r/g:v up to g:n we
     let break/state apply to g:r nodes */
  if (gdl_break_pending/* && (('r' != np->name[2] && 'R' != np->name[2]))*/)
    {
      gdl_break_node(np);
    }
  if (gdl_state_pending/* && 'r' != np->name[2] && 'R' != np->name[2]*/)
    gdl_state_node(np);
  return np;
}

Node *
gdl_graph_node_l(Mloc *locp, Tree *ytp, const char *name, const char *data)
{
  if (gdl_legacy_pending)
    {
      if (bit_get(gdl_legacy_pending, GLP_O2))
	{
	  /* Don't apply opener to the first grapheme because that is 'ba' in 'b[a ab' */
	  bit_off(gdl_legacy_pending, GLP_O2);
	  bit_set(gdl_legacy_pending, GLP_O1);
	}
      else if (bit_get(gdl_legacy_pending, GLP_O1))
	{
	  gdl_break_o(gdl_legacy_o); /* Now we are on the following grapheme, ab in 'b[a ab' */
	  gdl_legacy_pending = 0;
	}
    }

  Node *np = gdl_graph_node_s(locp, ytp, name, data);

  /* In legacy mode this behaves as though b]ad were ] bad# */
  if (gdl_legacy_hash)
    {
      gdl_lex_hash(lst);
      gdl_legacy_hash = 0;
    }

  return np;
}

List *
gdl_rem_nonw(List *wds)
{
  List *lp = list_create(LIST_SINGLE);
  Node *w;
  for (w = list_first(wds); w; w = list_next(wds))
    if (wordnode(w))
      list_add(lp, w);
  list_free(wds, NULL);
  return lp;
}

/* Call these before the first call to gdlparse_string (init) and
   after the last call (term) */
void
gdllib_init(void)
{
  if (gdl_initted++)
    return;

  gdl_balance_init();

  gdl_modq_init();

  gdl_stack_init();
}

void
gdllib_term(void)
{
  if (gdl_initted)
    {
      gdl_initted = 0;
      gdl_balance_term();
      gdl_modq_term();
      gdl_stack_term();
    }
}
