#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include <prop.h>
#include <ctype128.h>
#include <atf2utf.h>
#include <sll.h>
#include "gdl.tab.h"
#include "gdlstate.h"
#include "gdl.h"
#include "gvl.h"

extern const char *currgdlfile;
extern int gdltrace, gdllineno, gdl_legacy;
extern void gdllex_destroy(void);
extern void gdl_validate(Tree *tp);
int curr_lang = 's';
int deep_parse = 1;

int gdl_xmlids = 1;

static int grapheme_id, nonw_found, wid_base, word_excisions;
static char gdl_line_id[1024], gdl_word_id[2048];
static char *gid_insertp;

static unsigned const char *gdl_times_prefix;

#if 1
/* New 2026-03-03: state is now handled with three variables, pst,
   lst, rst

   pst (pending-state) is for openers only: it is zeroed-out after it
   	is applied to a node

   lst (last-state) is a pointer to the state property of the most
   	recently seen node and is where closers are applied
	
   rst (running-state) is set by openers and unset by closers; it is
   	applied to nodes in addition to pending-state
 */
gdlstate_t pst, *lst, rst;
Node *lgp = NULL;   /* last grapheme node pointer */
#else
gdlstate_t gst; /* global gdl state */
#endif

int gdl_word_mode;

const char *c_last_implicit_delim = NULL;
int c_delim_sentinel;
int c_implicit_times_reset;
int c_processing;
List *c_dangling_gps = NULL;
List *c_explicit_gps = NULL;
List *c_implicit_gps = NULL;
static Node *c_last_explicit_group_node = NULL;
static const char *curr_word_lang = "sux";
static List *wd_list = NULL;

/***********************************************************************
 *
 * Functions for running gdl.y
 *
 ***********************************************************************/

/* TODO: keep list of determinatives so we can validate pre/post
 * determinatives w/r/t next node.
 *
 * @pos=pre && next && next->name == "g:d" = det w delim on both sides
 * @pos=post && next && next->name != "g:d" = det w delim on neither side
 *
 */
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
      if (!strcmp(d->prev->name, "g:d"))
	gdl_prop_kv(d, GP_ATTRIBUTE, PG_GDL_INFO, "g:pos", "pre");
      else if (!strcmp(d->prev->name, "g:det"))
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

void
gdlparse_deep(Node *np, void *mptr)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (gp->orig && '|' != *gp->orig && gp->sign && '|' == *gp->sign)
	{
	  Tree *tp = tree_init();
	  Mloc *m = mptr;
	  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
	  gdl_setup_buffer((char*)pool_copy((ucp)gp->sign, gdlpool));
	  /* do this before gdl_set_tree so that initial w-node gets the right line number */
	  if (m)
	    gdl_lex_init(m->file, m->line);
	  else
	    gdl_lex_init("<string>", 1);
	  gdl_set_tree(tp);
	  gdlparse();
	  gdllex_destroy();
	  gp->deep = tp->root->kids;
	}
    }
}

/* Print the ->text value of each of the child nodes of a g:w */
void
gdl_wf_nodes(Node *w, FILE *wfp)
{
  Node *c;
  for (c = w->kids; c; c = c->next)
    {
      if (!strcmp(c->name, "g:x"))
	{
	  if (!strcmp(c->text, "..."))
	    {
	      fputc('x', wfp);
	      Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	      if (d)
		fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	    }
	  else
	    {
	      Prop *p = prop_find_kv(c->props, "g:type", NULL);
	      if (!p
		  || (p->u.k->v && strcmp(p->u.k->v, "comment")
		      && strcmp(p->u.k->v, "dollar")))
		mesg_verr(w->mloc, "gdl_wf_nodes: unhandled g:x text `%s'\n", c->text);
	    }
	}
      else if (!strcmp(c->name, "g:det"))
	{
	  fputc('{', wfp);
	  Prop *p = prop_find_kv(c->props, "g:char", NULL);
	  if (p)
	    fputs(p->u.k->v, wfp);
	  gdl_wf_nodes(c, wfp);
	  fputc('}', wfp);
	  /* post-determinatives have g:delim on the g:det node */
	  Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	  if (d)
	    fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	}
      else if (!strcmp(c->name, "g:gg"))
	{
	  Prop *p = prop_find_kv(c->props, "g:type", NULL);
	  if (p)
	    {
	      if (!strcmp(p->u.k->v, "correction"))
		fputs(c->kids->text, wfp);
	      else
		gdl_wf_nodes(c, wfp);
	    }
	  else
	    gdl_wf_nodes(c, wfp);
	  Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	  if (d)
	    fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	}
      else if (strcmp(c->name, "g:d") && strcmp(c->name, "g:z"))
	{
	  gdlstate_t s = prop_get_state(c);
	  if (!gs_is(s,gs_excised))
	    {
	      fputs(c->text, wfp);
	      Prop *d = prop_find_kv(c->props, "g:delim", NULL);
	      if (d)
		fputs(':' == *d->u.k->v ? "-" : d->u.k->v, wfp);
	    }
	  else
	    {
	      ++word_excisions;
	    }
	}
    }
}

static void
gdl_force_nonw(Node *w, const char *t)
{
  Node *rent = w->rent;
  *w = *w->kids;
  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "type", t);
  w->name = "g:nonw";
  w->rent = rent;
  w->next = NULL;
  ++nonw_found;
}

static void
gdl_nonw_excised(Node *w)
{
  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "type", "excised");
  w->name = "g:nonw";
  ++nonw_found;
}

/* This routine assumes it is processing one word-node at a time */
void
gdl_word_attr(Node *w)
{
  if (w && w->kids && !w->kids->next && !strcmp(w->kids->name, "g:x"))
    {
      if (strcmp(w->kids->text, "..."))
	{
	  Prop *p = prop_find_kv(w->props, "g:type", NULL);
	  if (p)
	    prop_drop_kv(w->props, "g:type", NULL);
	  gdl_force_nonw(w, p ? p->u.k->v : "comment");
	  return;
	}
    }

  char *wf_buf = NULL;
  size_t wf_len = 0;
  word_excisions = 0;
  /*  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", curr_word_lang);*/
  FILE *wf_fp = open_memstream(&wf_buf, &wf_len);
  gdl_wf_nodes(w, wf_fp);
  fclose(wf_fp);
  if (wf_buf)
    {
      if (*wf_buf)
	gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "form", (ccp)pool_copy((uccp)wf_buf, gdlpool));
      else if (w->kids && !strcmp(w->kids->name, "g:x"))
	gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "form", "x");
      else
	{
	  if (word_excisions)
	    gdl_nonw_excised(w);
	  else
	    gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "form", "XYZZY");
	}
      free(wf_buf);      
    }
  if (w->next)
    gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", " ");    
}

static List *
gdl_rem_nonw(List *wds)
{
  List *lp = list_create(LIST_SINGLE);
  Node *w;
  for (w = list_first(wds); w; w = list_next(wds))
    if (!strcmp(w->name, "g:w"))
      list_add(lp, w);
  list_free(wds, NULL);
  return lp;
}

Tree *
gdlparse_string(Mloc *m, char *s)
{
  Tree *tp = tree_init();
  char *s2 = malloc(strlen(s)+2);

  strcpy(s2, s);
  strcat(s2, "\n");

  /* must come before gdl_set_tree() */
  if (gdl_word_mode)
    wd_list = list_create(LIST_SINGLE);
  
  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);

  if (tp->root)
    tp->root->text = (ccp)pool_copy((uccp)s, gdlpool);

  gdl_setup_buffer(s2);
  
  /* do this before gdl_set_tree so that initial w-node gets the right line number */
  if (m)
    gdl_lex_init(m->file, m->line);
  else
    gdl_lex_init("<string>", 1);

  gdl_set_tree(tp);

  gdlparse();
  gdllex_destroy();
  free(s2);

  Node *w;
  nonw_found = 0;

  for (w = list_first(wd_list); w; w = list_next(wd_list))
    gdl_word_attr(w);

  if (nonw_found)
    wd_list = gdl_rem_nonw(wd_list);

  if (deep_parse)
    {
      if (gdl_word_mode)
	{
	  List *saved_wl = wd_list;
	  wd_list = list_create(LIST_SINGLE);
	  tree_iterator(tp, m, gdlparse_deep, NULL);
	  list_free(wd_list, NULL);
	  wd_list = saved_wl;
	}
      else
	tree_iterator(tp, m, gdlparse_deep, NULL);
    }
  
  return tp;
}

void
gdlparse_init(void)
{
  gdl_init();
}

void
gdlparse_reset(void)
{
}

void
gdlparse_term(void)
{
  gdl_term();
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

#if 0
void
gdl_node_type(Node *np, enum gdlpropvals p)
{
  gdl_prop(np, p, PG_GDL_GROUP);
}
#endif

static Node *
gdl_graph_node(Mloc *locp, Tree *ytp, const char *name, const char *data)
{
  Node *np = NULL;
  if (ytp->curr && !ytp->curr->mloc)
    ytp->curr->mloc = locp;
  np = tree_add(ytp, NS_GDL, name, ytp->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  lgp = np;
  lst = prop_state(np, pst|rst);
  sprintf(gid_insertp, ".%d", grapheme_id++);
  /*  if (gdl_xmlids && 'r' != name[2] && 'R' != name[2])*/
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id",
		(ccp)pool_copy((uccp)gdl_word_id, gdlpool));
  pst = 0L;
  np->mloc = mloc_mloc(locp);
  if (gdl_break_pending && (('r' != np->name[2] && 'R' != np->name[2])))
    gdl_break_node(np);
  if (gdl_state_pending && 'r' != np->name[2] && 'R' != np->name[2])
    gdl_state_node(np);
  return np;
}

static Node *
gdl_meta_node(Tree *ytp, const char *name, const char *data)
{
  Node *np = NULL;
  np = tree_add(ytp, NS_GDL, name, ytp->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  return np;
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
    mesg_warning(currgdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

void
gdl_prop_kv(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp && v)
    prop_node_add(ynp, ptype, gtype, k, v);
  else if (!v)
    mesg_vwarning(currgdlfile, gdllineno, "gdl_prop passed NULL value for key %s", k);
  else
    mesg_warning(currgdlfile, gdllineno, "gdl_prop passed NULL ynp");
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

  if ('&' == *span)
    ++span;
  if (gdltrace)
    fprintf(stderr, "gt: CELL with SPAN = %s\n", span);

  if (!(ancestor = node_ancestor_or_self(ytp->curr, "g:cell")))
    {
      cp = tree_node(ytp, NS_GDL, "g:cell", ytp->root->depth+1, NULL);
      gdl_prop(cp, GP_IMPLICIT, PG_GDL_INFO);
      gdl_prop_kv(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", "1");
      /* NB: This assumes GDL parser will never be embedded in another grammar */
      (void)node_insert(ytp->root, cp);
      tree_curr(ytp->root);
    }
  else
    tree_curr(ancestor->rent);
  cp = tree_add(ytp, NS_GDL, "g:cell", ytp->root->depth+1, NULL);
  gdl_prop_kv(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", (ccp)pool_copy((uccp)span, ytp->tm->pool));
  tree_curr(cp);
}
void
gdl_c_init(void)
{
  c_delim_sentinel = 0;
  c_processing = 1;
}

void
gdl_c_term(void)
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
}

/* New behaviour 20260212: SPACE resets node to next node of last
   child of either the parent l-node or the tree root (for
   non-word-wrapped GDL */
Node *
gdl_new_word(Tree *ytp)
{
  if (gdl_word_mode)
    {
      Node *l = node_ancestor_or_self(ytp->curr, "g:w");
      if (l)
	tree_curr(l->rent);
      else
	tree_curr(ytp->root);
      Node *wp = gdl_push(ytp, "g:w");
      wp->mloc = mloc_file_line(currgdlfile, gdllineno);
      /* By definition, the word-lang is the one in effect when the
	 word begins; logogram lang switches need to be handled
	 carefully */
      gdl_prop_kv(wp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", curr_word_lang);
      
      /* IF FIELD NOT IN SPARSE LEM HASH */
      list_add(wd_list, wp);
      sprintf(gdl_word_id, "%s%d", gdl_line_id, wid_base++);
      gid_insertp = gdl_word_id+strlen(gdl_word_id);
      gdl_prop_kv(wp, GP_ATTRIBUTE, PG_GDL_INFO, "xml:id",
		  (ccp)pool_copy((uccp)gdl_word_id, gdlpool));
      grapheme_id = 0;
      return wp;
    }
  return ytp->curr;
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
gdl_set_word_id(const char *wid)
{
  strcpy(gdl_word_id, wid);
  gid_insertp = gdl_word_id+strlen(gdl_word_id);
}

void
gdl_prefix(unsigned const char *p)
{
  gdl_times_prefix = p;
  ++c_delim_sentinel;
}

Node *
gdl_delim(Tree *ytp, const char *data)
{
  Node *np = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: DELIM: %s\n", data);
  np = tree_add(ytp, NS_GDL, "g:d", ytp->curr->depth, NULL);
  np->mloc = ytp->curr->mloc;
  Node *d_attach = lgp ? lgp : np->prev;
  if (d_attach)
    gdl_prop_kv(d_attach, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", (ccp)data);

  if (c_processing)
    {
      ++c_delim_sentinel;
      if (strcmp(np->rent->name, "g:gp")) /* implicit group */
	{
	  if ('.' != *data && '-' != *data && ':' != *data && '+' != *data)
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
  np->text = data;
  return np;
}

void
gdl_field(Tree *ytp, const char *ftype)
{
  Node *fp = NULL;
  Node *ancestor = NULL;

  if ('!' == *ftype)
    ++ftype;
  if (gdltrace)
    fprintf(stderr, "gt: FIELD with TYPE = %s\n", ftype);

  if (!(ancestor = node_ancestor_or_self(ytp->curr, "g:field")))
    {
      Node *cellp = node_ancestor_or_self(ytp->curr, "g:cell");
      
      fp = tree_node(ytp, NS_GDL, "g:field", ytp->root->depth+1, NULL);
      gdl_prop(fp, GP_IMPLICIT, PG_GDL_INFO);
      gdl_prop_kv(fp, GP_ATTRIBUTE, PG_GDL_INFO, "field", "default");

      /* NB: This assumes GDL parser will never be embedded in another grammar */
      if (!cellp)
	cellp = ytp->root;
      (void)node_insert(cellp, fp);
      tree_curr(cellp);
    }
  else
    tree_curr(ancestor->rent);
  fp = tree_add(ytp, NS_GDL, "g:field", ytp->root->depth+1, NULL);
  gdl_prop_kv(fp, GP_ATTRIBUTE, PG_GDL_INFO, "field", (ccp)pool_copy((uccp)ftype, ytp->tm->pool));
  tree_curr(fp);
}

Node *
gdl_graph(Mloc *locp, Tree *ytp, const char *data)
{
  extern int g_literal_flag, g_logoforce_flag;
  const char *gname = NULL;
  uccp gatf = NULL;
  Node *ret = NULL;

  if (gdl_times_prefix)
    {
      char *xdata = (char*)pool_alloc(strlen((ccp)gdl_times_prefix)+strlen(data)+1, ytp->tm->pool);
      strcpy(xdata, (ccp)gdl_times_prefix);
      strcat(xdata, data);
      data = xdata;
      gdl_times_prefix = NULL;
    }
  
  if (ytp->curr->kids && 'R' == ytp->curr->kids->name[2])
    gname = "g:N";
  else
    gname = "g:g";

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
  if (gatf)
    gdl_prop_kv(ret, GP_ATTRIBUTE, PG_GDL_INFO, "atf", (ccp)gatf);

  if (g_literal_flag)
    {
#if 1
      bit_set(*lst, gs_force|gs_g_undefined);
#else
      gdl_update_state(ret, gs_force|gs_g_undefined);
#endif
      g_literal_flag = 0;
    }
  else if (g_logoforce_flag)
    {
#if 1
      bit_set(*lst, gs_force|gs_g_flogo1);
#else
      gdl_update_state(ret, gs_force|gs_g_flogo1);
#endif
      g_logoforce_flag = 0;
    }
  return ret;
}

Node *
gdl_lang(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: LANG: %s\n", data);
  if (strstr(data, "/n"))
    curr_lang = 'n';
  else
    curr_lang = data[1];
  curr_word_lang = data+1;
  return gdl_meta_node(ytp, "g:z", data);
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
  return gdl_graph_node(locp, ytp, "g:p", data);
}

Node *
gdl_break_o(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_o, gdlstate_t gs_run, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/o: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok);
  ret = gdl_meta_node(ytp, "g:z", data);
  ps_on(gs_o);
  rs_on(gs_run);
  return ret;
}

Node *
gdl_break_c(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_c, gdlstate_t gs_run, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/c: %d=%s\n", tok, data);
  intptr_t st = gdl_balance_break(mlp, tok);
  if (gstck_i(st) > 0)
    {
      Node *np = gstck_np(st);
      if ('r' == np->name[2])
	np = np->rent;
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakStart", "1");
      Prop *idp = prop_find_kv(np->props, "xml:id", NULL);
      gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:breakEnd", idp->u.k->v);
    }
  bit_set(*lst,gs_c);
  rs_no(gs_run);
  /*gdl_update_state(lgp, gs_c);*/
  return gdl_meta_node(ytp, "g:z", data);
}

/* If data is /{{[0-9]+:/ the digits are a stream code */
Node *
gdl_gloss_o(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_o, gdlstate_t gs_run, const char *data)
{
  int stream = -1;
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/o: %d=%s\n", tok, data);
  if (tok == L_cur_par && data[2])
    {
      stream = atoi(data+2);
      if (stream <= 0 || stream > 99)
	{
	  mesg_vwarning(currgdlfile, gdllineno, "stream out of range in '%s'", data);
	  stream = -1;
	}
      data = "{{";
    }
  (void)gdl_balance_state(mlp, tok);
  (void)gdl_push(ytp, "g:glo");
  ps_on(gs_o);
  rs_on(gs_run);
  ret = gdl_meta_node(ytp, "g:z", data);
  prop_node_add(ret, GP_STREAM, PG_GDL_STATE, (void*)(uintptr_t)stream, NULL);
  return ret;
}

Node *
gdl_gloss_c(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_c, gdlstate_t gs_run, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/c: %d=%s\n", tok, data);
  ret = gdl_meta_node(ytp, "g:z", data);
  if (-1 != gdl_balance_state(mlp, tok))
    gdl_pop(ytp, data);
  bit_set(*lst,gs_c);
  rs_no(gs_run);
  /*gdl_update_state(lgp, gs_tok);*/
  return ret;
}
Node *
gdl_state_o(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_o, gdlstate_t gs_run, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/o: %d=%s\n", tok, data);
  (void)gdl_balance_state(mlp, tok);
  ret = gdl_meta_node(ytp, "g:z", data);
  ps_on(gs_o);
  rs_on(gs_run);
  return ret;
}

Node *
gdl_state_c(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_c, gdlstate_t gs_run, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/c: %d=%s\n", tok, data);
  ret =  gdl_meta_node(ytp, "g:z", data);
  intptr_t st = gdl_balance_state(mlp, tok);
  if (st > 0) /* st can be -1 if nothing on stack */
    {
      Node *np = gstck_np(st);
      if (!strcmp(np->rent->name, "g:n"))
	np = np->rent;
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:statusStart", "1");
      Prop *idp = prop_find_kv(np->props, "xml:id", NULL);
      gdl_prop_kv(lgp, GP_ATTRIBUTE, PG_GDL_INFO, "g:statusEnd", idp->u.k->v);
    }
  bit_set(*lst,gs_c);
  rs_no(gs_run);
  /*gdl_update_state(lgp, gs_c);*/
  return ret;
}

void
gdl_clear_gg(Tree *ytp)
{
  while (!strcmp(ytp->curr->name, "g:gg"))
    (void)gdl_pop(ytp, "g:gg");
}

void
gdl_group(Mloc mp, Node *lft, int type, Node *rt)
{
  Node *gp = NULL;
  const char *delim = (type=='/'?"/":(type==':'?":":"+"));
  if (!lft)
    {
      /* lft was a g:gg; if it is a different type create a new g:gg */
      const char *t = (type == '/' ? "alternation" : (type == ':' ? "reordering" : "ligature"));
      const char *lt = prop_find_kv(rt->tree->curr->props, "g:type", NULL)->u.k->v;
      if (strcmp(t, lt))
	{
	  const char *pt = NULL;
	  /* if there is a parent group that matches the incoming type pop the current group */
	  if (!strcmp(rt->tree->curr->rent->name, "g:gg"))
	    pt = prop_find_kv(rt->tree->curr->rent->props, "g:type", NULL)->u.k->v;
	  if (pt && !strcmp(pt, t))
	    {
	      Node *r2 = kids_rem_last(rt->tree);
	      gdl_prop_kv(r2->prev, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", delim);
	      gdl_pop(rt->tree, "g:gg");
	      kids_add_node(rt->tree, r2);
	    }
	  else
	    {
	      /* otherwise push a new group */
	      gp = node_group(rt->prev, rt->prev, rt);
	      gp->name = "g:gg";
	      gdl_prop_kv(gp, GP_ATTRIBUTE, PG_GDL_INFO, "g:type", t);
	      gdl_prop_kv(gp->kids, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", delim);
	      tree_curr(gp);
	    }
	}
      else
	gdl_prop_kv(rt->prev, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", delim);
    }
  else
    {
      gp = node_group(lft, lft, rt);
      gp->name = "g:gg";
      const char *v = (type == '/' ? "alternation" : (type == ':' ? "reordering" : "ligature"));
      gdl_prop_kv(gp, GP_ATTRIBUTE, PG_GDL_INFO, "g:type", v);
      gdl_prop_kv(gp->kids, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", delim);
      /*tree_curr(gp);*/ /* attach point remains unchanged after group insertion */
    }
}

#if 0
void
gdl_update_state(Node *np, gdlstate_t gs_tok)
{
  bit_set(np->props->u.s, gs_tok);
  gs_clear_closers();
}
#endif

