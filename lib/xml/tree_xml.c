#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include "xml.h"

int treexml_no_output = 0;

NSdata *ns_key_val[NS_LAST];

nodehandlers treexml_o_handlers;
nodehandlers treexml_p_handlers;
nodehandlers treexml_c_handlers;
nodehandlers treexml_a_handlers; /* attr */
nodehandlers treexml_u_handlers; /* text */

void
tree_ns_xml_print(Tree *tp, FILE *fp)
{
  enum nscode nsx = NS_NONE+1;
  nsdata_set_key_data(ns_key_val);
  if (tp->ns_used[0])
    fprintf(fp, " xmlns=\"%s\"", ns_key_val[tp->ns_used[0]]->equiv);
  for (; nsx < NS_LAST; ++nsx)
    if (tp->ns_used[nsx])
      fprintf(fp, " xmlns:%s=\"%s\"", ns_key_val[nsx]->name, ns_key_val[nsx]->equiv);
}

static void
treexml_keqv(const char *k, const char *v, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, " %s=\"%s\"", k, xmlify((uccp)v));
}

static void
treexml_props(Node *np, void *user)
{
  Prop *p = np->props;
  while (p)
    {
      if (p->g == PG_XML)
	treexml_keqv(p->u.k->k,p->u.k->v,user);
      p = p->next;
    }
}

void
treexml_o_generic(Node *np, void *user)
{
  if (treexml_no_output)
    return;

  Xmlhelper *xhp = user;

  if ('#' == *np->name)
    fputs(np->text, xhp->fp);
  else
    {
      fprintf(xhp->fp, "<%s", np->name);

      if (!np->rent)
	tree_ns_xml_print(np->tree, xhp->fp);

      if (treexml_a_handlers[np->ns] && user)
	treexml_a_handlers[np->ns](np, user);
      else
	treexml_props(np, user);
      
      fputc('>', xhp->fp);
    }
  
  if (treexml_u_handlers[np->ns] && user)
    treexml_u_handlers[np->ns](np, user);
#if 0
  else if (np->text)
    fprintf(xhp->fp, "<text>%s</text>", xmlify((uccp)np->text));
#endif
}

/* no generic output for parsed nodes */

void
treexml_c_generic(Node *np, void *user)
{
  if (treexml_no_output)
    return;

  if ('#' != *np->name)
    {
      Xmlhelper *xhp = user;
      fprintf(xhp->fp, "</%s>", np->name);
    }
}

void
tree_xml_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_o_handlers[np->ns])
    (treexml_o_handlers[np->ns])(np, xhp);
  else
    treexml_o_generic(np, xhp);
}

void
tree_xml_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_c_handlers[np->ns])
    (treexml_c_handlers[np->ns])(np, xhp);
  else
    treexml_c_generic(np, xhp);
}

void
tree_xml(FILE *fp, Tree *tp)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  tree_iterator(tp, xhp, tree_xml_node, tree_xml_post);
  free(xhp);
}

void
node_xml(FILE *fp, Node *np)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  node_iterator(np, xhp, tree_xml_node, tree_xml_post);
  free(xhp);
}
