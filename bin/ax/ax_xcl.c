#include <ctype128.h>
#include <string.h>
#include <unistd.h>
#include <mesg.h>
#include <prop.h>
#include <ilem.h>
#include <ilem_props.h>
#include <atf.h>
#include "../../lib/atf/atf.tab.h"
#include "ax.h"

#define cc(x) (ccp)(x)
#define ucc(x) (uccp)(x)

#define xpool_copy(s) (pool_copy(ucc(s)))

extern const char *project;
int ignore_plus = 0;
int lem_status = 0;
extern FILE *f_lemma;
static Hash *lemtab = NULL;
extern Hash *word_form_index;
extern int lem_standalone, line_is_unit;

extern int lem_props_yes;

static const char *lem_meta_id(void);
static const char *const*lem_text_meta(void);

extern int lemmata(struct xcl_context *xc, struct ilem_form *fp);

static void xtf2xcl_block(struct xcl_context *xc, struct node*n);
static void xtf2xcl_group(struct xcl_context *xc, struct node*n);
static void xtf2xcl_line(struct xcl_context *xc, struct node*n);
static void xtf2xcl_cells(struct xcl_context *xc, struct node*n);
static void xtf2xcl_fields(struct xcl_context *xc, struct node*n);
static void xtf2xcl_words(struct xcl_context *xc, struct node*n);

#define propxid(x) prop_find_kv(x->props, "xml:id", NULL)->u.k->v

void
lem_init(void)
{
  lemtab = hash_create(4999);
}

void
lem_term(void)
{
  hash_free(lemtab,NULL);
  lemtab = NULL;
}

struct xcl_context *
ax_xcl(struct run_context *run, struct node *text)
{
  struct xcl_context *xc = xcl_create();
  char *langs = NULL/*texttag_langs()*/;

  int saved_status = status;
  int saved_exit_status = exit_status;

  /*  xc->system = xcl_get_global_context()->system; */
  /*xc->cache = xcl_cache();*/
  xc->run = run;
  xc->curr = xc->root = NULL;
  xc->langs = (char*)pool_copy((unsigned char*)langs,xc->pool);
  free(langs);
  xc->project = (ccp)atfp->project;
  xc->textid = atfp->pqx;
  xc->file = file;
  xc->sigs = sig_context_init();
  if (lem_props_yes)
    ilem_props_init();
  xtf2xcl_block(xc,text);

  lem_status += status;
  status = saved_status;
  exit_status = saved_exit_status;

  return xc;
}

/* In the lib/atf implementation the tree is built using lib/std/tree
 * and all the tree nodes have a Block* in node->user unless
 * nodename is 'lg', in which case lg->user is a Group*.
 *
 * For Group* the LINE_MTS , LINE_NTS, and LINE_BIL are potentially of
 * interest for XCL; other line types are ignored.
 *
 * For Line* the np* is the GDL Cell/Field/Word tree.  In that tree,
 * g:w->user is non-NULL if the word is lemmatized; g:w->user is a
 * Lem*.
 *
 * Lemmatization is carried out during XCL tree build when a g:w with
 * non-NULL g:w->user is handled.
 */
static void
xtf2xcl_block(struct xcl_context *xc, struct node*n)
{
  /* fprintf(stderr, "xtfxcl: process invoked\n"); */
  if (!strcmp(n->name, "lg"))
    xtf2xcl_group(xc, n);
  else if (n->user)
    {
      switch (((Block*)n->user)->bt->bison)
	{
	case Y_COMPOSITE:
	case Y_SCORE:
	case Y_TRANSLITERATION:
	  xcl_chunk_id(atfp->pqx,xcl_c_text,xc);
	  xcl_chunk(xc, NULL, xcl_c_text);
	  xc->curr->meta = xcl_hash_lemm_meta(lem_text_meta(),
					      lem_meta_id(),
					      xc);
	  break;
	case Y_OBJECT:
	  /* fprintf(stderr, "xtfxcl: object found\n"); */
	  /* fix_context(xc,NULL); */
	  xcl_discontinuity(xc, propxid(n), xcl_d_object, NULL);
	  break;
	case Y_SURFACE:
	  /* fix_context(xc,NULL); */
	  xcl_discontinuity(xc, propxid(n), xcl_d_surface, NULL);
	  break;
	case Y_COLUMN:
	  /* fix_context(xc,NULL); */
	  {
	    const char *ref = propxid(n);
	    if (*ref)
	      xcl_discontinuity(xc, ref, xcl_d_column, NULL);
	    /* no discontinuity emitted for implicit column breaks */
	  }
	  break;
	case Y_M:
#if 0
	  if (!strcmp(n->name,"discourse"))
	    xcl_fix_context(xc,getAttr(n,"subtype"));
#endif
	  break;
	default:
	  break;
	}
    }
  else
    {
      fprintf(stderr, "xtf2xcl: unhandled node `%s'\n", n->name);
    }
 
  if (n->kids)
    {
      Node *k;
      for (k = n->kids; k; k = k->next)
	xtf2xcl_block(xc,k);
    }
}

void
xtf2xcl_group(XCL *xc, Node *n)
{
  if (line_is_unit && xc->curr && xc->curr->parent)
    xcl_insert_ub(n->mloc, xc, 0, xcl_c_sentence, 0);
  xcl_fix_context(xc,NULL);
  xcl_discontinuity(xc, propxid(n), xcl_d_line_start, NULL);
  Group *gp = n->user;
  int i;
  for (i = 0; i < gp->nlines; ++i)
    switch (gp->lines[i]->t)
      {
      case LINE_MTS:
	{
	  int j;
	  for (j = i+1; gp->lines[j]; ++j)
	    if (gp->lines[j]->t == LINE_NTS)
	      break;
	  if (j < gp->nlines)
	    xtf2xcl_line(xc, gp->lines[j]->np);
	  else
	    xtf2xcl_line(xc, gp->lines[i]->np);
	}
	break;
      case LINE_BIL:
	xtf2xcl_line(xc, gp->lines[i]->np);
	break;
      case LT_DOLLAR:
	xcl_discontinuity(xc, propxid(n), xcl_d_nonw, NULL);
	break;
      default:
	break;
      }
}

void
xtf2xcl_line(XCL *xc, Node *np)
{
  if (!strcmp(np->kids->name, "g:cell"))
    xtf2xcl_cells(xc, np);
  else if (!strcmp(np->kids->name, "g:field"))
    xtf2xcl_fields(xc, np);
  else
    xtf2xcl_words(xc, np);
}

void
xtf2xcl_cells(XCL *xc, Node *np)
{
  for (np = np->kids; np; np = np->next)
    {
      struct xcl_c *curr = xc->curr;
      xcl_discontinuity(xc, NULL, xcl_d_cell_start, NULL);
      if (np->kids)
	{
	  if (!strcmp(np->kids->name, "g:field"))
	    xtf2xcl_fields(xc, np);
	}
      else
	xtf2xcl_words(xc, np);
      xc->curr = curr;
      xcl_discontinuity(xc, NULL, xcl_d_cell_end, NULL);      
    }
}

void
xtf2xcl_fields(XCL *xc, Node *np)
{
  for (np = np->kids; np; np = np->next)
    {
      struct xcl_c *curr = xc->curr;
      xcl_discontinuity(xc, NULL, xcl_d_field_start, NULL);
      if (np->kids)
	xtf2xcl_words(xc, np);
      xc->curr = curr;
      xcl_discontinuity(xc, NULL, xcl_d_field_end, NULL);
    }
}

void
xtf2xcl_words(XCL *xc, Node *np)
{
  static int first_word = 0;

  for (np = np->kids; np; np = np->next)
    if (!strcmp(np->name, "g:w") || !strcmp(np->name, "n:w"))
      {
	xcl_fix_context(xc,NULL);
	ilem_parse(xc, np->user, first_word);
	first_word = 0;
      }
    else if (!strcmp(np->name, "g:nonw"))
      {
	xcl_discontinuity(xc, propxid(np), xcl_d_nonw, NULL);
	first_word = 0;
      }
}

static const unsigned char *
get_line_id(const char *ref)
{
  static char buf[64],*p;
  strcpy(buf,ref);
  p = strrchr(buf,'.');
  if (p)
    *p = '\0';
  return (uccp)buf;
}

static const char *
first_l(struct xcl_c *c)
{
  const char *ret = NULL;
  int i;
  for (i = 0; i < c->nchildren; ++i)
    {
      if (c->children[i].c->node_type == xcl_node_l)
	return cc(label_from_line_id(get_line_id(c->children[i].l->ref)));
      else if (c->children[i].c->node_type == xcl_node_c
	       && (ret = first_l(c->children[i].c)))
	return ret;
    }
  return ret;
}

static const char *
last_l(struct xcl_c *c)
{
  const char *ret = NULL;
  int i;
  for (i = c->nchildren-1; i >= 0; --i)
    {
      if (c->children[i].c->node_type == xcl_node_l)
	return cc(label_from_line_id(get_line_id(c->children[i].l->ref)));
      else if (c->children[i].c->node_type == xcl_node_c
	       && (ret = last_l(c->children[i].c)))
	return ret;
    }
  return ret;
}

void
xcl_sentence_labels(struct xcl_context *xc, struct xcl_c *c)
{
  const char *first = NULL, *last = NULL;
  int n_lems = 0;

  if (c->type != xcl_c_sentence || !c->nchildren)
    return;

  first = first_l(c);
  last = last_l(c);

  if (first)
    {
      if (last && first != last)
	{
	  char *l = malloc(strlen(first)+strlen(last)+4);
	  sprintf(l,"%s - %s",first,last);
	  c->label = cc(pool_copy(ucc(l), atfmp->pool));
	  free(l);
	}
      else
	c->label = cc(pool_copy(ucc(first), atfmp->pool));
    }
  else if (n_lems) /* sentences that don't have lemmata don't matter */
    {
      /*vwarning("couldn't compute label for sentence with id=%s",c->ref);*/
    }
}

static const char *
lem_meta_id(void)
{
  static char buf[16];
  sprintf(buf,"%s.m0",atfp->pqx);
  return buf;
}

static const char *const*
lem_text_meta(void)
{
  const char**m = NULL;
  if (atfp->xmd)
    {
      m = malloc(10*sizeof(char*));
      m[0] = "period";
      m[1] = hash_find(atfp->xmd,(unsigned char *)"period");
      m[2] = "provenience";
      m[3] = hash_find(atfp->xmd,(unsigned char *)"provenience");;
      m[4] = "genre";
      m[5] = hash_find(atfp->xmd,(unsigned char *)"genre");;
      m[6] = "subgenre";
      m[7] = hash_find(atfp->xmd,(unsigned char *)"subgenre");;
      m[8] = m[9] = NULL;
    }
  return m;
}
