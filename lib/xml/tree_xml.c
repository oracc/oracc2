#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include "xml.h"

struct ns_key_val {
  const char *prefix; const char *url;
} ns_key_val[] = {
  { "", "" } ,
  { "c" , "http://oracc.org/ns/cbd/1.0" } ,
  { "g" , "http://oracc.org/ns/gdl/1.0" } ,
  { "s" , "http://oracc.org/ns/sl/1.0" } ,
  { "x" , "http://oracc.org/ns/xtf/1.0" } ,
  { "m" , "http://oracc.org/ns/xmd/1.0" } ,
  { "cfy" , "http://oracc.org/ns/cfy/1.0" } ,
  { "h" , "http://www.w3.org/1999/xhtml" } ,
  { "i" , "http://oracc.org/ns/inl/1.0" } ,
  { "b" , "http://oracc.org/ns/bib/1.0" } ,
  { NULL , NULL } 
};

nodehandlers treexml_o_handlers;
nodehandlers treexml_p_handlers;
nodehandlers treexml_c_handlers;
nodehandlers treexml_a_handlers; /* attr */
nodehandlers treexml_u_handlers; /* text */

void
tree_ns_xml_print(Tree *tp, FILE *fp)
{
  enum nscode nsx = NS_NONE+1;
  if (tp->ns_used[0])
    fprintf(fp, " xmlns=\"%s\"", ns_key_val[tp->ns_used[0]].url);
  for (; nsx < NS_LAST; ++nsx)
    if (tp->ns_used[nsx])
      fprintf(fp, " xmlns:%s=\"%s\"", ns_key_val[nsx].prefix, ns_key_val[nsx].url);
}

void
treexml_o_generic(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s", np->name);

  if (!np->rent)
    tree_ns_xml_print(np->tree, xhp->fp);

  if (treexml_a_handlers[np->ns] && user)
    treexml_a_handlers[np->ns](np, user);

  fputc('>', xhp->fp);

  if (treexml_u_handlers[np->ns] && user)
    treexml_u_handlers[np->ns](np, user);
  else if (np->text)
    fprintf(xhp->fp, "<text>%s</text>", xmlify((uccp)np->text));
}

/* no generic output for parsed nodes */

void
treexml_c_generic(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
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
