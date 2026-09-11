#include <oraccsys.h>
#include "vx.h"
#include "conll.h"

#define CONLLO_COLUMNS							\
  "ID FORM LEMMA UPOS XPOS FEATS HEAD DEPREL DEPS MISC"			\
  " "									\
  "ORACC:PROJECT ORACC:LANG ORACC:CF ORACC:GW ORACC:SENSE ORACC:POS ORACC:EPOS" \
  " "									\
  "ORACC:NORM ORACC:BASE ORACC:CONT ORACC:STEM ORACC:MORPH ORACC:MORPH2" \
  " "									\
  "ORACC:OID ORACC:SENSEID ORACC:FORMC ORACC:LEMMAC ORACC:BASEC ORACC:WORDID" \
  " "									\
  "ORACC:LINEID ORACC:LINELABEL ORACC:ATFLW"

Conll_run *
conll_init(void)
{
  Conll_run *r = calloc(1, sizeof(Conll_run));
  r->global_columns = CONLLO_COLUMNS;
  r->m_doc = memo_init(sizeof(Conll_doc), 8);
  r->m_sent = memo_init(sizeof(Conll_doc), 128);
  r->m_word = memo_init(sizeof(Conll_word), 256);
  r->pool = pool_init();
  r->pooh = hpool_init();
  return r;
}

void
conll_term(Conll_run *r)
{
}

Conll_doc *
conll_doc(Conll_run *r, const char *id, const char *nm, int nsents)
{
  Conll_doc *d = memo_new(r->m_doc);
  if (!r->docs)
    r->docs = list_create(LIST_SINGLE);
  list_add(r->docs, d);
  d->doc_id = id;
  d->doc_nm = nm;
  d->run = r;
  d->nsents = nsents;
  d->sents = memo_new_array(r->m_sent, d->nsents);
  return d;
}

Conll_sent *
conll_sent(Conll_doc *d, size_t nwords)
{
  Conll_sent *s = &d->sents[d->sindex++];
  s->sent_id = (ccp)pool_copy((uccp)itoa(d->sindex), d->run->pool);
  s->nwords = nwords;
  s->words = memo_new_array(d->run->m_word, s->nwords);
  s->run = d->run;
  return s;
}

Conll_word *
conll_word(Conll_sent *s)
{
  Conll_word *w = &s->words[s->windex++];
  w->run = s->run;
  return w;
}

void
conll_dump_oracc(Conll_word *w, FILE *fp)
{
#define cdo(f) fputc('\t',fp);fputs(w->p.f?w->p.f:"_",fp)
#define cdw(f) fputc('\t',fp);fputs(w->f?w->f:"_",fp)
  cdo(LANG);
  cdo(CF);
  cdo(GW);
  cdo(SENSE);
  cdo(POS);
  cdo(EPOS);
  cdo(NORM);
  cdo(BASE);
  cdo(CONT);
  cdo(STEM);
  cdo(M1);
  cdo(M2);
  cdo(OID);
  cdo(SENSEID);
  cdo(FORMC);
  cdo(LEMMAC);
  cdo(BASEC);
  cdo(WORDID);
  cdo(LINEID);
  cdo(LINELABEL);
  cdw(wid);
  cdw(lid);
  cdw(lbl);
  if (w->atfl)
    fprintf(fp, "\t%s/%s", w->atfl, w->atfw);
  else
    fputs("\t_", fp);
}

void
conll_dump_misc(Keva **kp, FILE *fp)
{
}

void
conll_dump_word(Conll_word *w, FILE *fp)
{
  fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
	  w->c.ID, w->c.FORM, w->c.LEMMA, w->c.UPOS, w->c.XPOS, w->c.FEATS, w->c.HEAD, w->c.DEPREL, w->c.DEPS);
  if (w->c.MISC)
    conll_dump_misc(w->c.MISC, fp);
  else
    fputs("\t_", fp);
  conll_dump_oracc(w, fp);
  fputc('\n', fp);
}

void
conll_dump(Conll_run *r, FILE *fp)
{
  fprintf(fp, "# global.columns = %s\n", r->global_columns);
  Conll_doc *d;
  for (d = list_first(r->docs); d; d = list_next(r->docs))
    {
      fprintf(fp, "# newdoc id = %s\n", d->doc_id);
      fprintf(fp, "# docname = %s\n", d->doc_nm);
      if (d->atff)
	fprintf(fp, "# atffile = %s\n", d->atff);
      if (d->project)
	fprintf(fp, "# project = http://oracc.org/%s\n", d->project);
      int i;
      for (i = 0; i < d->nsents; ++i)
	{
	  Conll_sent *s = &d->sents[i];
	  fprintf(fp, "# sent_id = %s.s%s\n", d->doc_id, s->sent_id);
	  fprintf(fp, "# text = %s\n", s->text);
	  fprintf(fp, "# text_en = %s\n", s->tren);
	  fprintf(fp, "# xsux = %s\n", s->xsux);
	  int j;
	  for (j = 0; j < s->nwords; ++j)
	    conll_dump_word(&s->words[j], fp);
	  fputc('\n', fp);
	}
    }
}
