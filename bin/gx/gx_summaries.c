#include <oraccsys.h>
#include <xnn.h>
#include <ns-cbd_summaries.h>
#include <rnvif.h>
#include <xmlify.h>
#include <joxer.h>
#include <rnvxml.h>
#include <gt.h>
#include "gx.h"

extern locator *xo_loc;
static void o_jox_summaries(Cbd *c);
static void o_jox_sum_letter(Letter *lp);
static void o_jox_summary(Entry *ep);
static void o_jox_sum_bases(Field **f);

void
gx_summaries(Cbd *c)
{
  FILE *sfp = fopen("summaries.xml", "w");
  jox_xml_output(sfp);
  joxer_init(&cbd_summaries_data, "cbd-summaries", 1, sfp, NULL);
  o_jox_summaries(curr_cbd);
  joxer_term(sfp,NULL);
}

static void
o_jox_summaries(Cbd *c)
{
  int l;
  xo_loc = &c->l;
  Ratts *ratts = ratts_cbd(c, O_XML_SUM);
  joxer_ea(xo_loc, "articles", ratts);
  for (l = 0; l < c->nletters; ++l)
    o_jox_sum_letter(&c->letters[l]);
  joxer_ee(xo_loc,"articles");
}

static void
o_jox_sum_letter(Letter *lp)
{
  if (lp->entries)
    {
      xo_loc = &lp->entries[0]->l;
      const char *r[3] = { "dc:title", (ccp)lp->l, NULL };
      joxer_ea(xo_loc,"letter",rnvval_aa_ccpp(r));
      int i;
      for (i = 0; lp->entries[i]; ++i)
	o_jox_summary(lp->entries[i]);
      joxer_ee(xo_loc, "letter");
    }
}

static const char *
o_sum_periods(Field **f)
{
  int len = 0, nfields, i;
  for (nfields = 0; f[nfields]; ++nfields)
    ;
  char *pp[nfields+1];
  for (i = 0; f[i]; ++i)
    {
      pp[i] = strchr(strrchr(f[i]->k[1], (char)1), '/')+1;
      len += strlen(pp[i]);
    }
  pp[i] = NULL;
  len += (2*nfields) + 1;
  char p[len];
  *p = '\0';
  for (i = 0; pp[i]; ++i)
    {
      if (*p)
	strcat(p, ", ");
      strcat(p, pp[i]);
    }
  return (ccp)pool_copy((uccp)p, csetp->pool);
}

static void
o_jox_summary(Entry*ep)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "ref", ep->oid);
  ratts_kis(lp, ep->k);
  list_pair(lp, "n", ep->cgp->tight);
  char title[strlen((ccp)ep->owner->project)+strlen((ccp)ep->owner->lang) + strlen((ccp)ep->cgp->tight)+3];
  sprintf(title, "%s/%s/%s", ep->owner->project, ep->owner->lang, ep->cgp->tight);
  list_pair(lp, "dc:title",pool_copy((uccp)title, csetp->pool));
  if (ep->hshary[0])
    list_pair(lp, "periods", o_sum_periods(ep->hshary[0]));
  Ratts *r = rnvval_aa_ccpp((const char**)list2array(lp));
  list_free(lp, NULL);
  joxer_ea(xo_loc,"summary",r);
  joxer_et(xo_loc, "cf", NULL, (ccp)ep->cgp->cf);
  r = ratts_one("xml:lang", ep->owner->trans ? (ccp)ep->owner->trans : "en");
  joxer_et(xo_loc, "gw", r, (ccp)ep->cgp->gw);
  joxer_et(xo_loc, "pos", NULL, (ccp)ep->cgp->pos);
  if (ep->hshary[EFLD_BASE])
    o_jox_sum_bases(ep->hshary[EFLD_BASE]);
  if (ep->senses)
    {
      Sense *sp;
      for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
	{
	  r = ratts_one("xml:lang", ep->owner->trans ? (ccp)ep->owner->trans : "en");
	  joxer_et(xo_loc, "mng", r, (ccp)sp->mng);
	}
    }
  joxer_ee(xo_loc, "summary");  
}

static void
o_jox_sum_bases(Field **f)
{
  int i;
  for (i = 0; f[i]; ++i)
    {
      joxer_ea(xo_loc, "base", ratts_base(f[i], O_XML));
      Gt *t = ((Cform*)f[i]->data)->b;
      if (t)
	{
	  joxer_et(xo_loc,"s", NULL, (ccp)t->sign);
	  joxer_ea(xo_loc,"t", NULL);
	  grx_jox(t->gdl, NULL);
	  joxer_ee(xo_loc,"t");
	}
      joxer_ee(xo_loc, "base");
    }
}
