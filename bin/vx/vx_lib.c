#include <oraccsys.h>
#include "vx.h"

/* Find an upper level component or return NULL */
Node *
vx_component(Tree *tp, const char *name)
{
  if (!strcmp(tp->root->name, name)) /* normally xtf */
    return tp->root;

  if (!strcmp(tp->root->kids->name, name)) /* composite, transliteration */
    return tp->root->kids;

  if (tp->root->kids->next && !strcmp(tp->root->kids->next->name, name)) /* translation lives here */
    return tp->root->kids->next;

  if (tp->root->kids->last && !strcmp(tp->root->kids->last->name, name)) /* XCL */
    return tp->root->kids->last;

  return NULL;
}

List *
vx_tags(Node *np, const char *tag)
{
  return vx_tags_by_attr(np, tag, NULL, NULL, 0);
}

const char *
vx_att(Node *np, const char *att)
{
  if (np->props)
    {
      Prop *p = prop_find_kv(np->props, att, NULL);
      if (p)
	return p->u.k->v;
    }
  return NULL;
}

void
vx_selector(Node *np, Vxsel *vp)
{
  if (vp->tag && strcmp(np->name, vp->tag))
    return;
  const char *v = NULL;
  if (vp->att && !(v = vx_att(np, vp->att)))
    return;
  if (vp->val && v
      && ((!vp->match && strcmp(v, vp->val))
	  || (vp->match && !strstr(v, vp->val))))
    return;
  list_add(vp->l, np);
}

List *
vx_tags_by_attr(Node *np, const char *tag, const char *attr, const char *value, int match)
{
  Vxsel *vp = calloc(1, sizeof(Vxsel));
  vp->tag = tag;
  vp->att = attr;
  vp->val = value;
  vp->match = match;
  vp->l = list_create(LIST_SINGLE);
  node_iterator(np, vp, (nodehandler)vx_selector, NULL);
  List *lp = vp->l;
  free(vp);
  return lp;
}
