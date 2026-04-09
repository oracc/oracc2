#include <tree.h>
#include <memo.h>
#include <pool.h>
#include <scan.h>
#include <xml.h>
#include <rnvval.h>
#include <rnvxml.h>
#include "inl.h"

Memo *inl_scan_m = NULL;
Memo *inl_scanseg_m = NULL;
Pool *inl_scan_p = NULL;

int inl_rnv;
int inl_wild_mode;
inl_self_func_p inl_self_func = NULL;

void
inl_self_set(inl_self_func_p f)
{
  inl_self_func = f;
}

static void
inl_rnv_bib(Node *np, Scanseg *sp)
{
  const char *atts[5] = { "key" , np->text, "pp" , NULL , NULL };
  Ratts *r = NULL;
  if (sp)
    {
      if (sp->attr)
	atts[3] = sp->attr;
      else
	atts[2] = NULL;
      r = rnvval_aa_ccpp(atts);
    }
  char name[10];
  sprintf(name, "b:%s", np->name);
  rnvxml_ec(name, r);
}

static void
inl_rnv_inl(Node *np, Scanseg *sp)
{
  Ratts *r = NULL;

  char name[10] = { 'i', ':' , '\0' };

  if ('#' == *np->name && sp)
    strcat(name, sp->name);
  else
    strcat(name, np->name);
  
  if (sp)
    {
      if (sp->name && sp->attr)
	{
	  const char *atts[5] = { "tag" , NULL, "att" , NULL , NULL };
	  atts[2] = sp->name;
	  atts[4] = sp->attr;
	  r = rnvval_aa_ccpp(atts);
	}
    }

  if (name[2])
    rnvxml_ea(name, r);

  if ((!sp || !sp->name) && np->text)
    rnvxml_ch((ccp)xmlify((uccp)np->text));
}

static void
treexml_o_bib(Node *np, void *user)
{
  Xmlhelper *xhp = user;

  if (inl_rnv)
    inl_rnv_bib(np, user);
  else
    {
      fprintf(xhp->fp, "<b:%s", np->name);
      Scanseg *sp = np->user;
      if (sp)
	{
	  fprintf(xhp->fp, " key=\"%s\"", np->text);
	  sp->np->kids = NULL; /* the key is also np->text so we've printed it */
	  if (sp->attr)
	    fprintf(xhp->fp, " pp=\"%s\"", sp->attr);
	}
      fputs("/>", xhp->fp);
    }
}

void
treexml_c_bib(Node *np, void *user)
{
}

static void
treexml_o_inl(Node *np, void *user)
{
  Xmlhelper *xhp = user;

  if (inl_rnv)
    inl_rnv_inl(np, user);
  else
    {
      if (np->name && '#' != *np->name)
	{
	  fprintf(xhp->fp, "<i:%s", np->name);

	  /* FIXME: This must be wrong given the open tag in the line above */
	  if (!np->rent)
	    tree_ns_xml_print(np->tree, xhp->fp);

	  if (treexml_a_handlers[np->ns] && user)
	    treexml_a_handlers[np->ns](np, user);
	}

      Scanseg *sp = np->user;
      if (sp)
	{
	  if (!sp->name)
	    {
	      fputc('>', xhp->fp);
	      const char *xch = (ccp)xmlify((uccp)np->text);
	      fputs(xch, xhp->fp);
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
}

void
treexml_c_inl(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if ('#' != *np->name)
    {
      char name[10] = { 'i', ':' , '\0' };
      strcat(name, np->name);
      if (inl_rnv)
	rnvxml_ee(name);
      else
	fprintf(xhp->fp, "</i:%s>", np->name);
    }
}

void
inl_init(void)
{
  if (!inl_scan_m)
    {
      inl_scan_m = memo_init(sizeof(Scan), 256);
      inl_scanseg_m = memo_init(sizeof(Scan), 1024);
    }
  if (!inl_scan_p)
    inl_scan_p = pool_init();

  nodeh_register(treexml_o_handlers, NS_BIB, treexml_o_bib);
  nodeh_register(treexml_c_handlers, NS_BIB, treexml_c_bib);
  nodeh_register(treexml_o_handlers, NS_INL, treexml_o_inl);
  nodeh_register(treexml_c_handlers, NS_INL, treexml_c_inl);
}

void
inl_term(void)
{
  if (inl_scan_m)
    {
      memo_term(inl_scan_m);
      memo_term(inl_scanseg_m);
      inl_scan_m = NULL;
      inl_scanseg_m = NULL;
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
inl(Mloc *mp, char *s)
{
  Tree *tp = NULL;
  if (s && *s)
    {
      tp = tree_init();
      /*tree_ns_default(tp, NS_INL);*/
      tree_root(tp, NS_INL, "inl", 0, NULL);
      tp->rootless = 1;
      tp->root->text = s;
      Scan *sp = memo_new(inl_scan_m);
      sp->data = (ccp)pool_copy((uccp)s, inl_scan_p);
      sp->tree = tp;
      sp->mp = *mp;
      (void)inl_nodes(sp, tp->root, (char*)tp->root->text);
    }
  return tp;
}
