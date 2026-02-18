#include <oraccsys.h>
#include <cbd.h>
#include <rnvxml.h>
#include "gx.h"

void
ratts_kis(List *lp, Kis_data k)
{
  if (k && k[0])
    {
      list_pair(lp, "icount", kis_cnt(k));
      list_pair(lp, "ipct", kis_pct(k));
      list_pair(lp, "xis", kis_tis(k));
    }
}

Ratts *
ratts_kis_r(Kis_data k)
{
  if (k && k[0])
    {
      List *lp = list_create(LIST_SINGLE);
      ratts_kis(lp, k);
      return ratts_list2ratts(lp);
    }
  return NULL;
}

Ratts *
ratts_list2ratts(List *lp)
{
  Ratts *r = rnvval_aa_ccpp((const char**)list2array(lp));
  list_free(lp, NULL);
  return r;
}

static void
ratts_cbd_list(List *lp, Cbd *c, enum o_mode mode)
{
  if (mode == O_JSN)
    list_pair(lp, "type", "glossary");
  else
    list_pair(lp, "n", c->lang);
  list_pair(lp, "project", c->project);
  if (mode == O_JSN)
    {
      char src[strlen((ccp)c->project)+strlen("http://oracc.org/0")];
      sprintf(src, "http://oracc.org/%s", c->project);
      unsigned char *psrc = pool_copy((uccp)src, csetp->pool);
      list_pair(lp, "source", psrc);
      list_pair(lp, "license", "This data is released under the CC0 license");
      list_pair(lp, "license-url", "https://creativecommons.org/publicdomain/zero/1.0/");
      list_pair(lp, "more-info", "http://oracc.org/doc/opendata/");
    }
  if (mode == O_XML || mode == O_XML_SUM)
    list_pair(lp, "xml:lang", c->lang);
  else
    list_pair(lp, "lang", c->lang);

  if (mode == O_XML_SUM)
    {
      char title[strlen((ccp)c->project)+strlen((ccp)c->lang) + strlen("  Glossary0")];
      sprintf(title, "%s %s Glossary", c->project, c->lang);
      list_pair(lp, "dc:title",pool_copy((uccp)title, csetp->pool));
    }
}

Ratts *
ratts_cbd(Cbd *c, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  ratts_cbd_list(lp, c, mode);
  Ratts *r = rnvval_aa_ccpp((const char**)list2array(lp));
  list_free(lp, NULL);
  return r;
}

void
ratts_entry_list(List *lp, Entry *e, enum o_mode mode)
{
  Cgp *cp = e->cgp;
  if (mode == O_JSN)
    {
      list_pair(lp, "headword", cp->tight);
      list_pair(lp, "id", e->oid);
    }
  else
    {
      list_pair(lp, "xml:id", e->oid);
      list_pair(lp, "n", cp->tight);
    }
  list_pair(lp, "oid", e->oid);

  ratts_kis(lp, e->k);

  if (mode == O_JSN)
    {
      char dc[strlen((char*)e->owner->project)
	      +strlen((char*)e->owner->lang)
	      +strlen((char*)cp->tight)
	      +strlen("//0")];
      sprintf(dc, "%s/%s/%s", e->owner->project, e->owner->lang, cp->tight);
      list_pair(lp, "dc_title", pool_copy((uccp)dc,csetp->pool));
      list_pair(lp, "cf", cp->cf);
      list_pair(lp, "gw", cp->gw);
      list_pair(lp, "pos", cp->pos);
    }
}

Ratts *
ratts_entry_sa(Entry *e, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  ratts_cbd_list(lp, e->owner, mode);
  /*Hack: first entry of lp is "n" but we can't have cbd-n and entry-n*/
  if (!strcmp((const char *)lp->first->data, "n"))
    lp->first->data = "cbd-n";
  ratts_entry_list(lp, e, mode);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_entry(Entry *e, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  ratts_entry_list(lp, e, mode);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_field(Field *f, unsigned const char *n, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "cbd:id", f->id);
  list_pair(lp, "n", n);
  ratts_kis(lp, f->k);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_cpd(Cgp *c, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  if (c->primary)
    list_pair(lp, "primary", 1);
  list_pair(lp, "partsig", c->tight);
  list_pair(lp, "ref", c->owner->oid);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_form(Field *f, int c, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "xml:id", f->id);
  list_pair(lp, "n", ((Cform*)f->data)->f.form);
  if (c >= 0)
    list_pair(lp, "c", itoa(c));
  ratts_kis(lp, f->k);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_norm(Field *f, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "xml:id", f->id);
  list_pair(lp, "n", ((Cform*)f->data)->f.norm);
  ratts_kis(lp, f->k);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_nmfm(Field *f, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "n", ((Cform*)f->data)->f.form);
  list_pair(lp, "cbd:id", f->id);
  list_pair(lp, "ref", ((Cform*)f->data)->f.user);
  ratts_kis(lp, f->k);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_one(const char *attr, const char *aval)
{
  const char *rar[3] = { attr , aval , NULL };
  return rnvval_aa_ccpp(rar);
}

Ratts *
ratts_sense(Sense *s, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "xml:id", s->oid);
  list_pair(lp, "n", s->cgspe);
  list_pair(lp, "oid", s->oid);
  ratts_kis(lp, s->k);
  return ratts_list2ratts(lp);
}

Ratts *
ratts_sig(Field *f)
{
  List *lp = list_create(LIST_SINGLE);
  list_pair(lp, "xml:id", f->id);
  list_pair(lp, "sig", f->data);
  ratts_kis(lp, f->k);
  return ratts_list2ratts(lp);
}

