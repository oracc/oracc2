#include <oraccsys.h>
#include <tree.h>
#include "cfy.h"

/* This module manages the Cfy.body which tracks the current location
 * in XTF's segment/object/surface/fragment/column/fragment hierarchy.
 *
 * The XTF expression of this hierarchy differs between
 * xtf:transliteration and xtf:composite; the cfy implementation
 * unifies the different forms.
 *
 * To implement this fully for @composites requires an audit of all of
 * the xtf:m and xtf:div uses in the corpus because these are defined
 * in the XTF schema to have text values.
 *
 * The initial implementation handles all of the @transliteration
 * structure and those parts of @composite that mirror the
 * transliteration structure using @m-milestones.
 */

Tree *
cfy_body_init(Cfy *c, const char *name)
{
  Tree *tp = tree_init();
  tree_ns_default(tp, NS_CFY);
  tree_root(tp, NS_CFY, name, 0, NULL);
  Div *b = xtfbody(name, strlen(name));
  tp->root->user = (void*)(uintptr_t)b;
  return tp;
}

Div *
cfy_body(Cfy *c, Xtfbody *xp, const char **atts)
{
  extern int pi_line;
  Elt *ep = memo_new(c->m_elt);
  ep->line = pi_line;
  ep->etype = xp->e;
  ep->data = xp;
  Div *dp = ((Elt *)c->body->curr->user)->data;
  if (verbose)
    fprintf(stderr, "cfy_body: called with div=%s[%s]\n", dp->name, dp->text);
  if (xp->b < dp->b)
    {
      while (xp->b < dp->b)
	{
	  (void)tree_pop(c->body);
	  dp = ((Elt *)c->body->curr->user)->data;
	}
    }
  else
    {
      (void)tree_add(c->body, NS_CFY, "div", c->body->curr->depth, NULL);
      if (xp->b > dp->b)
	(void)tree_push(c->body);
    }      
  return ep->data;
}

void
cfy_body_term(void)
{
  tree_term();
}
