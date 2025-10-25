#include <tree.h>
#include <memo.h>
#include <pool.h>
#include <scan.h>
#include <xml.h>
#include "inl.h"

Memo *inl_scan_m = NULL;
Pool *inl_scan_p = NULL;

static void
treexml_o_inl(Node *np, void *user)
{
  Xmlhelper *xhp = user;

  if (np->name && '#' != *np->name)
    {
      fprintf(xhp->fp, "<i:%s", np->name);

      if (!np->rent)
	tree_ns_xml_print(np->tree, xhp->fp);

      if (treexml_a_handlers[np->ns] && user)
	treexml_a_handlers[np->ns](np, user);
    }

  Scan *sp = np->user;
  if (sp)
    {
      if (!sp->name)
	{
	  fputc('>', xhp->fp);
	  fputs((ccp)xmlify((uccp)np->text), xhp->fp);
	}
      else if ('#' == *np->name)
	{
	  fprintf(xhp->fp, "<i:%s/>", sp->name);
	}
      else
	{
	  fprintf(xhp->fp, " tag=\"%s\"", sp->name);
	  if (sp->attr)
	    fprintf(xhp->fp, " att=\"%s\"", sp->attr);
	  fputc('>', xhp->fp);
	}
    }
  else
    fputc('>', xhp->fp);    
}

void
treexml_c_inl(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if ('#' != *np->name)
    fprintf(xhp->fp, "</i:%s>", np->name);
}

void
inl_init(void)
{
  if (!inl_scan_m)
    inl_scan_m = memo_init(sizeof(Scan), 256);
  if (!inl_scan_p)
    inl_scan_p = pool_init();

  nodeh_register(treexml_o_handlers, NS_INL, treexml_o_inl);
  nodeh_register(treexml_c_handlers, NS_INL, treexml_c_inl);
}

void
inl_term(void)
{
  if (inl_scan_m)
    {
      memo_term(inl_scan_m);
      inl_scan_m = NULL;
    }
  if (!inl_scan_p)
    {
      pool_term(inl_scan_p);
      inl_scan_p = NULL;
    }
}

/* scan an inline string and return a Tree whose children are the
 * parsed components of the string
 *
 * arg 's' must be a string that can be chopped up by the inl routines.
 */
Tree *
inl(char *s)
{
  Tree *tp = NULL;
  if (s && *s)
    {
      tp = tree_init();
      tree_ns_default(tp, NS_INL);
      tree_root(tp, NS_INL, "inl", 0, NULL);
      /*tp->rootless = 1;*/
      tp->root->text = s;
      (void)inl_nodes(tp->root, strdup(tp->root->text));
    }
  return tp;
}
