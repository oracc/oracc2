#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include <prop.h>
#include <lang.h>
#include <ctype128.h>
#include <atf2utf.h>
#include <sll.h>
#include "gdllib.h"
#include "gdlr.h"
#include "gvl.h"

#include "lex_st_enum.h"

/* This is application-global and doesn't vary by GDL instance */
extern struct lang_context *gdl_global_context;

/* These are specific to gdl.l so they can't go in gdllib.h and are
   not for use outside of gdl.l/gdl.y */
extern const char *gdlfile;
extern int gdllineno;

int gdl_no_xml_ids = 0;
int gdl_xmlids = 1;

static Hash *lzr_sparse = NULL;
static int gdl_excision_type = 'e';

/***********************************************************************
 *
 * Functions for running gdl.y
 *
 ***********************************************************************/

void
gdl_pop_wd_list(void *vp)
{
  if (list_len(wd_list))
    if (!vp || vp  == (void*)wd_list->last->data)
      list_pop(wd_list);  
}

/* TODO: keep list of determinatives so we can validate pre/post
 * determinatives w/r/t next node.
 *
 * @pos=pre && next && next->name == "g:d" = det w delim on both sides
 * @pos=post && next && next->name != "g:d" = det w delim on neither side
 *
 */
void
gdl_gloss_props(Node *d)
{
  /* FIXME: test more but this block is now the same as in gdl_det_props to choose pre/post */
  if (!d->prev)
    {
      gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "pre");
    }
  else
    {
      Prop *pd = prop_find_kv(d->prev->props, "g:delim", NULL);
      if (pd)
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "pre");
      else if (!strcmp(d->prev->name, "g:d"))
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos",
		    prop_find_kv(d->prev->props, "g:pos", NULL)->u.k->v);
      else
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "post");
    }
}

void
gdl_det_props(Node *d)
{
  if (gs_is(rst, gs_g_phond))
    {
      gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:role", "phonetic");
      gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:char", "+");
    }
  else
    {
      gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:role", "semantic");
      if (gs_is(rst, gs_g_semd_e))
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:char", "-");
    }
  if (!d->prev)
    {
      gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "pre");
    }
  else
    {
      /* we know d->prev is non-NULL */
      Prop *pd = prop_find_kv(d->prev->props, "g:delim", NULL);
      if (pd)
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "pre");
      else if (!strcmp(d->prev->name, "g:d"))
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos",
		    prop_find_kv(d->prev->props, "g:pos", NULL)->u.k->v);
      else
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "post");
    }
}

List *
gdl_get_word_list(void)
{
  return wd_list;
}

static void
gdl_force_nonw(Node *w, const char *t)
{
  Node *rent = w->rent;
  Node *prev = w->prev;
  Node *next = w->next;
  *w = *w->kids;
  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "type", t);
  prop_drop_kv(w->props, "g:type", NULL);
  w->name = "g:nonw";
  w->rent = rent;
  w->prev = prev;
  w->next = next;
  ++nonw_found;
}

static void
gdl_nonw_excised(Node *w)
{
  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "type", ('e' == gdl_excision_type) ? "excised" : "comment");
  prop_drop_kv(w->props, "g:type", NULL);
  gdl_excision_type = 'e';
  w->name = "g:nonw";
  ++nonw_found;
}

static void
gdl_nonw_punct(Node *w)
{
  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "type", "punct");
  prop_drop_kv(w->props, "g:type", NULL);
  w->name = "g:nonw";
  ++nonw_found;
}

/* Return 0 if any g:l, g:n, g:s, g:v, or g:x children are not excised */
static int
gdl_word_is_excised(Node *w)
{
  Node *n = w;
  int ret = 1;
  int skip_kids = 0;
  for (n = n->kids; n; n = n->next)
    {
      if (strlen(n->name) == 3
	  && ('l' == n->name[2] || 's' == n->name[2] || 'v' == n->name[2] || 'n' == n->name[2] || 'q' == n->name[2]))
	{
	  if (n->props)
	    {
	      gdlstate_t s = prop_get_state(n);
	      if (!gs_is(s,gs_excised))
		return 0;
	      else
		skip_kids = 1;
	    }	  
	}
      else if (!strcmp(n->name, "g:x"))
	{
	  Prop *p = prop_find_kv(n->props, "g:type", NULL);
	  if (p && (!strcmp(p->u.k->v, "dollar") || !strcmp(p->u.k->v, "comment")))
	    gdl_excision_type = 'c';
	  else if (strcmp(p->u.k->v, "linebreak"))
	    return 0;
	}
      if (skip_kids)
	skip_kids = 0;
      else
	{
	  if (n->kids)	
	    if (!(ret = gdl_word_is_excised(n)))
	      return 0;
	}
    }
  return ret;
}

/* This routine assumes it is processing one word-node at a time and
 * is called in the context of the list of words, i.e., after an
 * entire line of ATF has been scanned.
 *
 * NOTE: GDL is not allowed to set w->user !
 */
void
gdl_word_attr(Node *w)
{
  if (w)
    {
      if (w->kids)
	{
	  if ( !w->kids->next
	       && (!strcmp(w->kids->name, "g:x")
		   || !strcmp(w->kids->name, "g:p")))
	    {
	      if (strcmp(w->kids->text, "..."))
		{
		  if ('p' == w->kids->name[2])
		    {
		      Prop *p = prop_find_kv(w->props, "g:type", NULL);
		      if (p)
			prop_drop_kv(w->props, "g:type", NULL);
		      gdl_nonw_punct(w);
		    }
		  else if ('/' == *w->kids->text || ';' == *w->kids->text)
		    {
		      /* promote the g:x child to the g:w spot */
		      Node *gx = w->kids;
		      w->name = gx->name;
		      w->props = gx->props;
		      w->kids = NULL; /* orphan the original g:x node; these
					 are not numerous so it's not too
					 wasteful */
		      ++nonw_found; /* trigger gdl_rem_nonw */
		    }
		  else
		    {
		      Prop *p = prop_find_kv(w->props, "g:type", NULL);
		      if (p)
			prop_drop_kv(w->props, "g:type", NULL);
		      gdl_force_nonw(w, p ? p->u.k->v : "comment");
		    }
		  return;
		}
	    }
	  else if (gdl_word_is_excised(w))
	    gdl_nonw_excised(w);

	  /* How to handle C10E word forms? Always create them? */
	  unsigned char *wf_buf = gdl_render(w, gdlr_wfo_config);

	  if (wf_buf)
	    {
	      if (*wf_buf)
		gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "form", (ccp)pool_copy((uccp)wf_buf, gdlpool));
	      else if (w->kids && !strcmp(w->kids->name, "g:x"))
		gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "form", "x");
	      else
		{
		  /* we are now ignoring ($...$) and (#...#) in gdl_word_is_excised() */
		  /*gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "form", "XYZZY");*/
		}
	      free(wf_buf);
	    }
	  if (gdl_ascii)
	    {
	      unsigned char *af_buf = gdl_render(w, gdlr_wfa_config);
	      if (af_buf && *af_buf)
		gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "atf:form",
			    (ccp)pool_copy((uccp)af_buf, gdlpool));
	    }
	  if (w->next)
	    gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", " ");
	}
      else
	{
	  /* this will get the empty word removed later */
	  w->name = "g:empty";
	  ++nonw_found;
	}
    }
}

Tree*
gdl_literal(Mloc *m, char *s)
{
  Tree *tp = tree_init();
  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
  tp->root->text = s;
  return tp;
}

/***********************************************************************
 *
 * Utility routines for the tree-builder
 *
 ***********************************************************************/

void
gdl_set_ids(const char *lid, int wid)
{
  strcpy(gdl_line_id, lid);
  strcat(gdl_line_id, ".");
  wid_base = wid;
}

void
gdl_gp_type(Tree *ytp, enum gdlpropvals p)
{
  gdl_prop(ytp->curr, p, PG_GDL_GROUP);
}

const char *
gdl_grapheme_id(void)
{
  sprintf(gid_insertp, ".%d", grapheme_id++);
  return gdl_word_id;
}

Node *
gdl_pop(Tree *ytp, const char *s)
{
  Node *np = tree_pop(ytp);
  if (!strcmp(np->name, s))
    /* error */;
  return np;
}

Node *
gdl_push(Tree *ytp, const char *s)
{
  tree_add(ytp, NS_GDL, s, ytp->curr->depth, NULL);
  return tree_push(ytp);
}

Node *
gdl_push_l(Mloc *ml, Tree *ytp, const char *s)
{
  Node *np = gdl_push(ytp, s);
  np->mloc = mloc_mloc(ml);
  return np;
}

void
gdl_prop(Node *ynp, int ptype, int gtype)
{
  if (ynp)
    ynp->props = prop_add(ynp->tree->tm->prop_mem, ynp->props, ptype, gtype);
  else
    mesg_warning(gdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

void
gdl_prop_kv(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp && v)
    prop_node_add(ynp, ptype, gtype, k, v);
  else if (!v)
    mesg_vwarning(gdlfile, gdllineno, "gdl_prop passed NULL value for key %s", k);
  else
    mesg_warning(gdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

void
gdl_remove_q_error(Mloc m, Node *ynp)
{
  int rem = 0;
  (void)((rem = mesg_remove_error(m.file, m.line, "must be qualified"))
	 || (rem = mesg_remove_error(m.file, m.line, "unknown value"))
	 || (rem = mesg_remove_error(m.file, m.line, "unknown sign")));
  if (rem)
    {
      if (gdltrace)
	mesg_err(&m, "gdl_remove_q_error succeeded");
      ((gvl_g*)(ynp->user))->mess = NULL;
    }
}

static void
gdl_unword(Tree *ytp)
{
  /* Remove the parent g:w if there is one (extra credit: keep the
     node around to use after the field) */
  if (gdl_word_mode && wordnode(ytp->curr) && !ytp->curr->kids)
    {
      tree_curr(ytp->curr->rent);
      gdl_recycled_word = kids_rem_last(ytp);

      /* This is an empty word followed by a field code that may have
	 been been added to wd_list before the field code changed;
	 check it's valid, and if it isn't then remove it */
      if (list_len(wd_list)
	  && gdl_recycled_word == (void*)wd_list->last->data
	  /*&& (!lzr_sparse || !hash_find(lzr_sparse, (uccp)curr_field))*/)
	(void)list_pop(wd_list);      
    }
}

/***********************************************************************
 *
 * Tree-building routines called from gdl.y
 *
 ***********************************************************************/

/* Used when a number is not followed by a '('; need to sexify nums here */
Node *
gdl_barenum(Mloc *locp, Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt BARENUM: %s\n", data);
  return gdl_graph_node(locp, ytp, "g:B", data);
}

void
gdl_cell(Tree *ytp, const char *span)
{
  Node *cp = NULL;
  Node *ancestor = NULL;

  gdl_unword(ytp);
  
  ++gdl_cell_count;
  
  if ('&' == *span)
    ++span;
  if (gdltrace)
    fprintf(stderr, "gt: CELL with SPAN = %s\n", span);

  if (gdl_word_mode && wordnode(ytp->curr) && !ytp->curr->kids)
    {
      tree_curr(ytp->curr->rent);
      gdl_recycled_word = kids_rem_last(ytp);
      /*(void)list_pop(wd_list);*/
    }

  if (!(ancestor = node_ancestor_or_self(ytp->curr, "c")))
    {
      cp = tree_node(ytp, NS_GDL, "c", ytp->root->depth+1, NULL);
      gdl_prop(cp, GP_IMPLICIT, PG_GDL_INFO);
      gdl_prop_kv(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", "1");
      /* NB: This assumes GDL parser will never be embedded in another grammar */
      (void)node_insert(ytp->root, cp);
      tree_curr(ytp->root);
    }
  else
    tree_curr(ancestor->rent);
  cp = tree_add(ytp, NS_GDL, "c", ytp->root->depth+1, NULL);
  gdl_prop_kv(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", (ccp)pool_copy((uccp)span, ytp->tm->pool));
  tree_curr(cp);
}

void
gdl_c_init(Node *ycp)
{
  c_delim_sentinel = 0;
  c_processing = 1;
  lgp = ycp;
  lst = prop_state(ycp, pst|rst);
  pst = 0L;
  if (!gdl_no_xml_ids)
    {
      sprintf(gid_insertp, ".%d", grapheme_id++);
      gdl_prop_kv(ycp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id",
		  (ccp)pool_copy((uccp)gdl_word_id, gdlpool));
    }
  if (gdl_break_pending)
    gdl_break_node(ycp);
  if (gdl_state_pending)
    gdl_state_node(ycp);
}

Node *
gdl_c_term(Tree *ytp)
{
  c_implicit_times_reset = c_processing = 0;
  c_last_explicit_group_node = NULL;
  c_last_implicit_delim = NULL;
  if (c_dangling_gps)
    {
      list_free(c_dangling_gps, NULL);
      c_dangling_gps = NULL;
    }
  if (c_explicit_gps)
    {
      list_free(c_explicit_gps, NULL);
      c_explicit_gps = NULL;
    }
  if (c_implicit_gps)
    {
      list_free(c_implicit_gps, NULL);
      c_implicit_gps = NULL;
    }
  if (!strcmp(ytp->curr->name, "g:sur"))
    tree_curr(ytp->curr->rent);
  return ytp->curr;
}

/* New behaviour 20260212: SPACE resets node to next node of last
   child of either the parent l-node or the tree root (for
   non-word-wrapped GDL) */
Node *
gdl_new_word(Tree *ytp)
{
  Node *retnode = NULL;
  if (gdl_word_mode)
    {
      /* we are going to reset the attach point so it's enough to NULL
	 out group attach here */
      if (gdl_group_attach)
	gdl_group_attach = NULL;

      if (gdl_recycled_word)
	{
	  /* This is a word that is being recycled when starting a new
	     cell or field so we know either of those is the parent */
	  Node *w = gdl_recycled_word;
	  gdl_recycled_word = NULL;
	  w->prev = w->next = NULL;
	  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", word_lang_tag);
	  assert(word_lang_tag != NULL);
	  w->rent = ytp->curr;
	  ytp->curr->last = ytp->curr->kids = w;
	  tree_curr(w);
	  retnode = ytp->curr;
	  /* IF FIELD NOT IN SPARSE LEM HASH */
	  if (!lzr_sparse || (curr_field && hash_find(lzr_sparse, (uccp)curr_field)))
	    list_add(wd_list, w);
	}
      else if (wordnode(ytp->curr) && !ytp->curr->kids)
	{
	  /* reuse empty words */
	  assert(word_lang_tag != NULL);
	  gdl_prop_kv(ytp->curr, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", word_lang_tag);
	  retnode = ytp->curr;
	  /* If this is an empty word followed by a field it may have
	     been added to wd_list before the field code changed;
	     check it's valid, and if it isn't then remove it */
	  if (lzr_sparse && curr_field && !hash_find(lzr_sparse, (uccp)curr_field))
	    (void)list_pop(wd_list);
	}
      else
	{
	  /* The tree needs a new g:w node */

	  /* If there is an active gloss, attach to that */
	  Node *l = gdl_gloss_curr();
      
	  /* Else if there is a f ancestor, attach to that */
	  if (!l)
	    l = node_ancestor_or_self(ytp->curr, "f");

	  /* Else if there is a c ancestor, attach to that */
	  if (!l)
	    l = node_ancestor_or_self(ytp->curr, "c");

	  /* Else if there is a g:w ancestor, attach to the parent of that node */
	  if (!l)
	    {
	      l = node_ancestor_or_self(ytp->curr, "g:w");
	      if (!l)
		l = node_ancestor_or_self(ytp->curr, "g:n");
	      if (l)
		l = l->rent;
	      else
		l = ytp->root;
	    }

	  /* Now l is the node where the g:w should attach */
	  tree_curr(l);

	  Node *wp = gdl_push(ytp, gdl_lang_context->tag->scode == stc_SAC ? "g:w" : "n:w");
	  wp->mloc = mloc_file_line(gdlfile, gdllineno);

	  /* By definition, the word-lang is the one in effect when the
	     word begins; logogram lang switches need to be handled
	     carefully */
	  assert(word_lang_tag != NULL);
	  gdl_prop_kv(wp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", word_lang_tag);
      
	  /* IF FIELD NOT IN SPARSE LEM HASH */
	  if (!lzr_sparse || (curr_field && hash_find(lzr_sparse, (uccp)curr_field)))
	    list_add(wd_list, wp);
	  sprintf(gdl_word_id, "%s%d", gdl_line_id, wid_base++);
	  gid_insertp = gdl_word_id+strlen(gdl_word_id);
	  if (!gdl_no_xml_ids)
	    gdl_prop_kv(wp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id",
			(ccp)pool_copy((uccp)gdl_word_id, gdlpool));
	  grapheme_id = 0;

	  retnode = wp;
	}
    }
  return retnode;
}

void
gdl_auto_id(void)
{
  static char buf[16];
  static int gid = 1;
  sprintf(buf, "a%04d", gid++);
  gdl_set_word_id(buf);
}

void
gdl_set_lang(Mloc *mp, const char *tag, struct lang_context *ctxt)
{
  if (!tag)
    {
      fprintf(stderr, "gdl_set_lang: internal error: called with null tag argument\n");
      return;
    }
  word_lang_tag = tag;
  gdl_lang_context = lang_switch(ctxt, tag, NULL, mp ? mp->file : NULL, mp ? mp->line : 0);
  if (!gdl_global_context)
    gdl_global_context = gdl_lang_context;
}

void
gdl_set_word_id(const char *wid)
{
  strcpy(gdl_word_id, wid);
  gid_insertp = gdl_word_id+strlen(gdl_word_id);
}

void
gdl_prefix(Tree *ytp, unsigned const char *p)
{
  Node *np = tree_add(ytp, NS_GDL, "g:o", ytp->curr->depth, NULL);
  gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:type", "repeated");
  np->mloc = ytp->curr->mloc;
  np->text = (ccp)p;
  if (c_processing)
    {
      ++c_delim_sentinel;
      if (!strcmp(np->rent->name, "g:gp"))
	{
	  if (!c_explicit_gps)
	    c_explicit_gps = list_create(LIST_SINGLE);
	  list_add(c_explicit_gps, np->rent);
	  c_last_explicit_group_node = np->rent;
	}
    }
}

Node *
gdl_field(Tree *ytp, const char *ftype)
{
  Node *fp = NULL;
  Node *ancestor = NULL;

  if ('!' == *ftype)
    ++ftype;
  if (gdltrace)
    fprintf(stderr, "gt: FIELD with TYPE = %s\n", ftype);
  curr_field = (ccp)pool_copy((uccp)ftype, ytp->tm->pool);
  
  gdl_unword(ytp);
  
  if (!(ancestor = node_ancestor_or_self(ytp->curr, "f")))
    {
      Node *cellp = node_ancestor_or_self(ytp->curr, "c");
      
      fp = tree_node(ytp, NS_GDL, "f", ytp->root->depth+1, NULL);
      gdl_prop(fp, GP_IMPLICIT, PG_GDL_INFO);
      gdl_prop_kv(fp, GP_ATTRIBUTE, PG_GDL_INFO, "type", "default");

      /* NB: This assumes GDL parser will never be embedded in another grammar */
      if (!cellp)
	cellp = ytp->root;
      (void)node_insert(cellp, fp);
      tree_curr(cellp);
    }
  else
    tree_curr(ancestor->rent);
  fp = tree_add(ytp, NS_GDL, "f", ytp->root->depth+1, NULL);
  tree_curr(fp);
  gdl_prop_kv(fp, GP_ATTRIBUTE, PG_GDL_INFO, "type", curr_field);
  return tree_push(ytp);
}

Node *
gdl_normseg(Mloc *locp, Tree *ytp, const char *data)
{
  Node *ret = NULL;

#if 0
  /* Do we ever have ascii norm? I doubt it ... */
  /* Don't ascify unless gdl_unicode is 0 and the data is not "00~" */
  if (!gdl_unicode && ('0' != data[0] || '0' != data[1] || (data[2] && '~' != data[2])))
    {
      uccp guni = atf2utf(locp,(uccp)data,0);
      if (strcmp((ccp)guni,(ccp)data))
	{
	  gatf = (uccp)data;
	  data = (ccp)pool_copy(guni,ytp->tm->pool);
	}
    }
#endif

  if (gdltrace)
    fprintf(stderr, "gt: NORMSEG[%s]: %s\n", "n:s", data);

  ret = gdl_graph_node(locp, ytp, "n:s", data);

#if 0
  /* If gatf is to be included in XML output then GP_ATTRIBUTE is
     okay--otherwise some other prop-group will be needed */
  if (gdl_ascii && gatf)
    gdl_prop_kv(ret, GP_ATTRIBUTE, PG_GDL_INFO, "atf:ascii", (ccp)gatf);

  if (g_literal_flag)
    {
      bit_set(*lst, gs_force|gs_g_undefined);
      g_literal_flag = 0;
    }
  else if (g_logoforce_flag)
    {
      bit_set(*lst, gs_force|gs_g_flogo1);
      g_logoforce_flag = 0;
    }
#endif
  return ret;
}

Node *
gdl_graph(Mloc *locp, Tree *ytp, const char *data)
{
  const char *gname = NULL;
  uccp gatf = NULL;
  Node *ret = NULL;

  if (ytp->curr->kids && 'R' == ytp->curr->kids->name[2])
    gname = "g:N";
  else
    gname = "g:g";

  /* Don't ascify unless gdl_unicode is 0 and the data is not "00~" */
  if (!gdl_unicode && ('0' != data[0] || '0' != data[1] || (data[2] && '~' != data[2])))
    {
      uccp guni = atf2utf(locp,(uccp)data,0);
      if (strcmp((ccp)guni,(ccp)data))
	{
	  gatf = (uccp)data;
	  data = (ccp)pool_copy(guni,ytp->tm->pool);
	}
    }
  
  if (gdltrace)
    fprintf(stderr, "gt: GRAPH[%s]: %s\n", gname, data);

  ret = gdl_graph_node(locp, ytp, gname, data);

  /* If gatf is to be included in XML output then GP_ATTRIBUTE is
     okay--otherwise some other prop-group will be needed */
  if (gdl_ascii && gatf)
    gdl_prop_kv(ret, GP_ATTRIBUTE, PG_GDL_INFO, "atf:ascii", (ccp)gatf);

  if (g_literal_flag)
    {
      bit_set(*lst, gs_force|gs_g_undefined);
      g_literal_flag = 0;
    }
  else if (g_logoforce_flag)
    {
      bit_set(*lst, gs_force|gs_g_flogo1);
      g_logoforce_flag = 0;
    }
  return ret;
}

Node *
gdl_nnum(Mloc *locp, Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: LISTNUM: %s\n", data);
  return gdl_graph_node(locp, ytp, "g:N", data);
}

Node *
gdl_listnum(Mloc *locp, Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: LISTNUM: %s\n", data);
  return gdl_graph_node(locp, ytp, "g:l", data);
}

Node *
gdl_nongraph(Mloc *locp, Tree *ytp, const char *data, const char *type)
{
  if (gdltrace)
    fprintf(stderr, "gt: NONGRAPH: %s\n", data);
  Node *i = gdl_graph_node(locp, ytp, "g:x", data);
  gdl_prop_kv(i, GP_ATTRIBUTE, PG_GDL_INFO, "g:type", type);
  return i;
}

/* This is triggered by [0-9]/( so we know its a repetition number */
Node *
gdl_number(Mloc *locp, Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: NUMBER: %s\n", data);
  return gdl_graph_node(locp, ytp, "g:R", data);
}

Node *
gdl_punct(Mloc *locp, Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: PUNCT: %s\n",data);
  Node *p =  gdl_graph_node(locp, ytp, "g:p", data);
  gdl_prop_kv(p, GP_ATTRIBUTE, PG_GDL_INFO, "g:type", data);
  return p;
}

void
gdl_clear_gg(Tree *ytp)
{
  while (!strcmp(ytp->curr->name, "g:gg"))
    (void)gdl_pop(ytp, "g:gg");
}

void
gdl_line_wrapup(Mloc m)
{
  Tree *ytp = gdl_get_tree();

  if (ytp)
    {
      /* Line ends with ',' or '&' and no following text */
      if (gdl_recycled_word)
	{
	  gdl_recycled_word = NULL;
	  if (gdl_recycled_word == (void*)wd_list->last->data)
	    (void)list_pop(wd_list);
	}

      /* We haven't done gdl_word_attr yet so the empty word is g:w not g:empty */
      if (!strcmp(ytp->curr->name, "g:w") && !ytp->curr->kids)
	{
	  tree_curr(ytp->curr->rent);
	  (void)kids_rem_last(ytp);
	}

      if (lst)
	{
	  gdl_hc(1);
	  lst = NULL;
	}

      gdl_lex_closers();
      gdl_balance_flush(&m);
    }

  pst = rst = 0L;
  curr_field = NULL;
  gdl_group_attach = lgp = NULL;
}

void
gdl_set_lzr_sparse(Hash *l)
{
  lzr_sparse = l;
}
