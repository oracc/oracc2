#include <oraccsys.h>
#include <tree.h>
#include <runexpat.h>
#include <xml.h>
#include "cfy.h"

/* This module manages the Cfy.body which is a Tree representing XTF's
 * segment/object/surface/fragment/column/fragment hierarchy.
 *
 * The XTF expression of this hierarchy differs between
 * xtf:transliteration and xtf:composite; the cfy implementation
 * unifies the different forms.
 *
 * To implement this fully for @composites requires an audit of all of
 * the xtf:m and xtf:div uses in the corpus because these are defined
 * in the XTF schema to have free text values (D'oh!).
 *
 * The initial implementation handles all of the @transliteration
 * structure and those parts of @composite that mirror the
 * transliteration structure using @m-milestones.
 */

Tree *
cfy_body_init(Cfy *c, const char *name, const char **atts)
{
  Tree *tp = tree_init();
  tree_ns_default(tp, NS_CFY);
  const char *n = (ccp)pool_copy((uccp)name, c->p);
  tree_root(tp, NS_CFY, n, 0, NULL);
  tp->rootless = 1;
  Div *dp = memo_new(c->m_div);
  Xtfbody *xp = xtfbody(n, strlen(n));
  *dp = *xp;
  dp->c = c;
  tp->root->text = (ccp)pool_copy((uccp)findAttr(atts, "n"), c->p);
  tp->root->user = dp;
  c->body = tp;
  c->divs_with_lines = list_create(LIST_SINGLE);
  return tp;
}

Div *
cfy_body(Cfy *c, Xtfbody *xp, const char **atts)
{
  extern int pi_line;
  Elt *ep = memo_new(c->m_elt);
  ep->line = pi_line;
  ep->etype = xp->e;

  Div *dp = memo_new(c->m_div);
  *dp = *xp;
  dp->c = c;
  ep->data = dp;

  if (verbose)
    fprintf(stderr, "cfy_body: called with div=%s\n", dp->name);
  Div *tdp = c->body->curr->user;
  while (xp->b <= tdp->b)
    {
      (void)tree_pop(c->body);
      tdp = c->body->curr->user;
    }
  Node *np = tree_add(c->body, NS_CFY, "div", c->body->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)findAttr(atts, "n"), c->p);
  np->user = dp;
  if (xp->b > tdp->b)
    (void)tree_push(c->body);

  c->body->curr->user = dp;
  return dp;
}

void
cfy_body_term(void)
{
  tree_term();
}

void
cfybody_debug_attr(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  Div *dp = np->user;
  fprintf(xhp->fp, " class=\"%s\" n=\"%s\"", dp->name, np->text);
}

void
cfybody_debug_user(Node *np, void *user)
{
  Div *dp = np->user;
  if (dp->elt_lines)
    ci_div(dp->c, dp);
}

void
cfy_body_debug(Cfy *c)
{
  cfy_out_html_config();
  c->body->rootless = 0;
  nodeh_register(treexml_a_handlers, NS_CFY, (nodehandler)cfybody_debug_attr);
  nodeh_register(treexml_u_handlers, NS_CFY, (nodehandler)cfybody_debug_user);
  tree_xml(stdout, c->body);
}

void
cfy_body_lines(Cfy *c)
{
  curr_div->lines = list_create(LIST_SINGLE);
  list_add(c->divs_with_lines, curr_div);
}
