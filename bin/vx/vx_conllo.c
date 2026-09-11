#include <oraccsys.h>
#include <gdl.h>
#include "vx.h"
#include "conll.h"

Node **tnodes;

static void
vxc_atf(Node *np, FILE *fp)
{
  if (!strcmp(np->name, "xcl:l"))
    {
      const char *ref = vx_att(np, "ref");
      Node *gdl_np = hash_find(xmlid_h, (uccp)ref);
      vx_atf_node(gdl_np, fp);
    }
}

static void
vxc_cun(Node *np, FILE *fp)
{
  if (!strcmp(np->name, "xcl:l"))
    {
      const char *ref = vx_att(np, "ref");
      Node *gdl_np = hash_find(xmlid_h, (uccp)ref);
      vx_cun_node(gdl_np, fp);
    }
}

static void
vxc_tra(Node *np, FILE *fp)
{
  vx_tra_node(np, fp);
}

void
vxc_words(Conll_sent *s, List *wl)
{
  Node *lp;
  for (lp = list_first(wl); lp; lp = list_next(wl))
    {
      Conll_word *w = conll_word(s);
      Node *xff = lp->kids;
      w->c.ID = (ccp)pool_copy((uccp)itoa(s->windex), s->run->pool);
      w->c.FORM = vx_att(xff, "form");
      char *bs = strchr(w->c.FORM, '\\'); if (bs) *bs = '\0';
      w->c.UPOS = vx_att(xff, "epos");
      w->c.XPOS = w->c.FEATS = w->c.HEAD = w->c.DEPREL = w->c.DEPS = "_";
      w->p.LANG = vx_att(xff, "xml:lang");
      w->p.CF = vx_att(xff, "cf");
      w->p.GW = vx_att(xff, "gw");
      w->p.SENSE = vx_att(xff, "sense");
      w->p.POS = vx_att(xff, "pos");
      w->p.EPOS = vx_att(xff, "epos");
      w->p.BASE = vx_att(xff, "base");
      w->p.CONT = vx_att(xff, "cont");
      w->p.M1 = vx_att(xff, "morph");
      w->p.M2 = vx_att(xff, "morph2");
      w->p.STEM = vx_att(xff, "stem");
      w->p.NORM = vx_att(xff, "norm");
      char cgp[strlen(w->p.CF)+strlen(w->p.GW)+strlen(w->p.POS)+strlen("[]0")];
      sprintf(cgp, "%s[%s]%s", w->p.CF, w->p.GW, w->p.POS);
      w->c.LEMMA = vx_epsd_cft(cgp);
      w->p.OID = vx_epsd_oid(cgp);
      char cgspe[strlen(cgp)+strlen(w->p.SENSE)+strlen(w->p.EPOS)+strlen("//'0")];
      sprintf(cgspe, "%s[%s//%s]%s'%s", w->p.CF, w->p.GW, w->p.SENSE, w->p.POS, w->p.EPOS);
      w->p.SENSEID = vx_epsd_sid(cgspe);
      w->p.LEMMAC = vx_epsd_ucun(w->c.LEMMA);
      w->p.FORMC = vx_epsd_ucun(w->c.FORM);
      w->p.BASEC = vx_epsd_ucun(w->p.BASE);
      w->ref = vx_att(lp, "ref");
      w->lp = lp;
      w->wp = hash_find(xmlid_h, (uccp)w->ref);
      w->atfl = vx_att(w->wp, "atfl");
      w->atfw = vx_att(w->wp, "atfw");
      w->wid = vx_att(w->wp, "xml:id");
      w->lid = vx_att(w->wp->rent, "xml:id");
      w->lbl = vx_att(w->wp->rent, "label");
    }
}

void
vxc_sentences(Conll_doc *d, Node **snp, Node **tnp)
{
  extern int s_words;
  int i;
  for (i = 0; i < d->nsents; ++i)
    {
      List *w = vx_tags(snp[i], "xcl:l");
      Conll_sent *s = conll_sent(d, list_len(w));
      size_t ignored;

      s_words = 0;
      FILE *atfp = open_memstream(&s->text, &ignored);
      node_iterator(snp[i], atfp, (nodehandler)vxc_atf, NULL);
      fclose(atfp);

      s_words = 0;
      FILE *cunp = open_memstream(&s->xsux, &ignored);
      node_iterator(snp[i], cunp, (nodehandler)vxc_cun, NULL);
      fclose(cunp);

      if (tnp && tnp[i])
	{
	  s_words = 0;
	  FILE *trap = open_memstream(&s->tren, &ignored);
	  node_iterator(tnp[i], trap, (nodehandler)vxc_tra, NULL);
	  fclose(trap);
	}

      vxc_words(s, w);
    }
}

static Node **
vxc_snodes(Tree *tp, int *nsp)
{
  Node *xcl = vx_component(tp, "xcl:xcl");
  if (xcl)
    {
      List *s = vx_tags_by_attr(xcl, "xcl:c", "type", "sentence", 0);
      if (s)
	{
	  *nsp = list_len(s);
	  return (Node**)list2array(s);
	}
      else
	{
	  *nsp = 0;
	  return NULL;
	}      
    }
  return NULL;
}

Node **
vxc_tnodes(Tree *tp, int *ntp)
{
   Node *tra = vx_component(tp, "xtr:translation");
   if (tra)
     {
       List *divs = vx_tags_by_attr(tra, "xh:div", "data-unit", NULL, 0);
       if (list_len(divs))
	 {
	   *ntp = list_len(divs);
	   return (Node**)list2array(divs);
	 }
       else
	 {
	   *ntp = 0;
	   return NULL;
	 }
     }
   return NULL;
}

void
vxc_doc(Conll_run *r, Tree *tp)
{
  int nsent, ntran;
  Node *tlit = vx_component(tp, "composite");
  if (!tlit)
    tlit = vx_component(tp, "transliteration");
  if (tlit)
    {
      Node **snodes = vxc_snodes(tp, &nsent);
      Node **tnodes = vxc_tnodes(tp, &ntran);
      const char *id = (ccp)pool_copy((uccp)vx_att(tlit, "xml:id"), r->pool);
      const char *nm = (ccp)pool_copy((uccp)vx_att(tlit, "n"), r->pool);
      Conll_doc *d = conll_doc(r, id, nm, nsent);
      d->atff = vx_att(tp->root, "atff");
      d->project = vx_att(tlit, "project");
      vxc_sentences(d, snodes, tnodes);
    }
}

void
vx_conllo(Tree *tp, FILE *fp)
{
  Conll_run *r = conll_init();
  vx_epsd_init();
  vxc_doc(r, tp);
  conll_dump(r, fp);
}
