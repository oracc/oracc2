#include <oraccsys.h>
#include <cbd.h>
#include <rnvxml.h>
#include "gx.h"

static const char *
ratts_id(enum o_mode mode)
{
  static int i = 1;
  static char id[10];
  sprintf(id, "z%06d", i);
#if 0
  if (mode == O_JSN)
#endif
    ++i;
  return id;
}

Ratts *
ratts_cbd(Cbd *c, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);

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
  if (mode == O_XML)
    list_pair(lp, "xml:lang", c->lang);
  else
    list_pair(lp, "lang", c->lang);

  
  Ratts *r = rnvval_aa_ccpp((const char**)list2array(lp));
  list_free(lp, NULL);
  return r;
}

Ratts *
ratts_entry(Entry *e, enum o_mode mode)
{
  List *lp = list_create(LIST_SINGLE);
  Cgp *cp = e->cgp;
  const char *id = e->eid ? e->eid : ratts_id(mode);
  if (mode == O_JSN)
    {
      list_pair(lp, "headword", cp->tight);
      list_pair(lp, "id", id);
    }
  else
    {
      list_pair(lp, "xml:id", id);
      list_pair(lp, "n", cp->tight);
    }
  list_pair(lp, "oid", id);
  if (e->k)
    {
      list_pair(lp, "icount", kis_cnt(e->k));
      list_pair(lp, "ipct", kis_pct(e->k));
      list_pair(lp, "xis", kis_tis(e->k));
    }
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
  Ratts *r = rnvval_aa_ccpp((const char**)list2array(lp));
  list_free(lp, NULL);
  return r;
}

Ratts *
ratts_one(const char *attr, const char *aval)
{
  const char *rar[3] = { attr , aval , NULL };
  return rnvval_aa_ccpp(rar);
}
