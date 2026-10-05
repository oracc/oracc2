#include <oraccsys.h>
#include "runexpat.h"
#include "xml.h"

static void xlt_attr(Node *np, const char **atts);
static void xlt_attr_xmlid(Node *np, const char **atts);

static xlt_attr_fnc xlt_attr_p = xlt_attr;
static Hash *xmlid_h = NULL;

static void
xlt_attr(Node *np, const char **atts)
{
  int i;
  for (i = 0; atts[i]; i += 2)
    {
      prop_node_add(np, PROP_ANY, PG_XML, atts[i],
		    (ccp)hpool_copy((uccp)atts[i+1], np->tree->tm->pooh));
    }
}

static void
xlt_attr_xmlid(Node *np, const char **atts)
{
  int i;
  for (i = 0; atts[i]; i += 2)
    {
      const char *aval;
      prop_node_add(np, PROP_ANY, PG_XML, atts[i],
		    aval = (ccp)hpool_copy((uccp)atts[i+1], np->tree->tm->pooh));
      if (!strcmp(atts[i], "xml:id"))
	hash_add(xmlid_h, (uccp)aval, np);
    }
}

static void
xlt_char(Tree *tp, const char *c)
{
  tp->curr->text = (ccp)pool_copy((uccp)c, tp->tm->pool);
}

static Node *
xlt_push(Tree *tp, const char *nam)
{
  (void)tree_add(tp, NS_NONE, nam, tp->curr->depth+1, NULL);
  return tree_push(tp);
}

static void
xlt_root(Tree *tp, const char *nam)
{
  tree_root(tp, NS_NONE, nam, 0, NULL);
}

static void
xlt_sH(void *vp, const char *name, const char **atts)
{
  char *c = charData_retrieve();
  if (*c)
    xlt_char(vp, c);
  Node *ep = xlt_push(vp, name);
  if (atts[0])
    xlt_attr_p(ep, atts);
}

static void
xlt_eH(void *vp, const char *name)
{
  char *c = charData_retrieve();
  if (*c)
    xlt_char(vp, c);
  tree_pop(vp);
}

static void
xlt_sH_root(void *vp, const char *name, const char **atts)
{
  xlt_root(vp, name);
  if (atts[0])
    xlt_attr(((Tree*)vp)->root, atts);
  XML_SetElementHandler(curr_rip->parser, xlt_sH, xlt_eH);  
}

Tree *
xml_load_tree(const char *fn, int with_xmlid)
{
  if (with_xmlid)
    xlt_attr_p = xlt_attr_xmlid;
  char const *fnlist[2];
  fnlist[0] = fn;
  fnlist[1] = NULL;
  Tree *tp = tree_init();
  tree_ns_default(tp, NS_NONE);
  runexpat_omit_rp_wrap();
  runexpatuD(i_list, fnlist, xlt_sH_root, xlt_eH, tp);
  return tp;
}

/*************************************************************
 *
 * Accessor functions
 *
 */

List *
xlt_tags(Node *np, const char *tag)
{
  return xlt_tags_by_attr(np, tag, NULL, NULL, 0);
}

const char *
xlt_att(Node *np, const char *att)
{
  if (np->props)
    {
      Prop *p = prop_find_kv(np->props, att, NULL);
      if (p)
	return p->u.k->v;
    }
  return NULL;
}

static void
xlt_selector(Node *np, XLT_sel *vp)
{
  if (vp->tag && strcmp(np->name, vp->tag))
    return;
  const char *v = NULL;
  if (vp->att && !(v = xlt_att(np, vp->att)))
    return;
  if (vp->val && v
      && ((!vp->match && strcmp(v, vp->val))
	  || (vp->match && !strstr(v, vp->val))))
    return;
  list_add(vp->l, np);
}

List *
xlt_tags_by_attr(Node *np, const char *tag, const char *attr, const char *value, int match)
{
  XLT_sel *vp = calloc(1, sizeof(XLT_sel));
  vp->tag = tag;
  vp->att = attr;
  vp->val = value;
  vp->match = match;
  vp->l = list_create(LIST_SINGLE);
  node_iterator(np, vp, (nodehandler)xlt_selector, NULL);
  List *lp = vp->l;
  free(vp);
  return lp;
}
