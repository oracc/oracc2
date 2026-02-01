#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

static List *
psu_permute(List *heads, Cgp *cp, int ffi, int nbytes)
{
  List *nheads = list_create(LIST_SINGLE);
  Entry *ce = cp->owner;
  Sense *sp;
  Form *hff;
  for (hff = list_first(heads); hff; hff = list_next(heads))
    for (sp = list_first(ce->senses); sp; sp = list_next(ce->senses))
      {
	Form *fp;
	for (fp = list_first(ce->forms); fp; fp = list_next(ce->forms))
	  {
	    Form *ff = malloc(nbytes);
	    memcpy(ff, hff, nbytes);
	    ff[ffi] = *fp;
	    ff[ffi].sense = sp->mng;
	    ff[ffi].epos = sp->pos;
	    list_add(nheads, ff);
	  }
      }
  return nheads;
}

void
psu_out_sig(Form *ff, int n, FILE *fp)
{
  fprintf(fp,
	  "{%s = %s += %s[%s//%s]%s'%s}::",
	  ff[0].form, ff[0].psu_ngram, ff[0].cf, ff[0].gw, ff[0].sense, ff[0].pos, ff[0].epos);
  int i;
  for (i = 1; i < n; ++i)
    {
      fprintf(fp, "%s", form_sig(csetp->pool, &ff[i]));
      if (n - i > 1)
	fputs("++", fp);
    }
}

unsigned char *
psu_one_sig(Form *f, int n)
{
  char *psu_buf = NULL;
  size_t psu_len = 0;
  FILE *psu_fp = open_memstream(&psu_buf, &psu_len);
  psu_out_sig(f, n, psu_fp);
  fclose(psu_fp);
  unsigned char *ret = pool_copy((uccp)psu_buf, csetp->pool);
  free(psu_buf);
  return ret;
}

static unsigned char *
psu_orth_form(Form *ff, int n)
{
  int i, len = 0;
  for (i = 1; i < n; ++i)
    len += strlen((ccp)ff[i].form);
  len += n;
  char buf[len+2];
  *buf = '\0';
  for (i = 1; i < n; ++i)
    {
      strcat(buf, (ccp)ff[i].form);
      strcat(buf, " ");
    }
  buf[strlen(buf)-1] = '\0';
  return pool_copy((uccp)buf, csetp->pool);
}

static unsigned char *
psu_ngram(List *cgps)
{
  Cgp *cp;
  int len = 0;
  for (cp = list_first(cgps); cp; cp = list_next(cgps))
    len += strlen((ccp)cp->tight);
  len += list_len(cgps);
  char buf[len+2];
  *buf = '\0';
  for (cp = list_first(cgps); cp; cp = list_next(cgps))
    {
      strcat(buf, (ccp)cp->tight);
      strcat(buf, " ");
    }
  buf[strlen(buf)-1] = '\0';
  return pool_copy((uccp)buf, csetp->pool);
}

void
psu_parts_sigs(List *sigs, Entry *ep, Parts *p)
{
  List *heads = list_create(LIST_SINGLE);
  Cgp *cp = list_first(p->cgps);
  Entry *ce = cp->owner;
  Sense *sp;
  int i = 1; /* index ff from 1 because ff[0] is the PSU form */
  for (sp = list_first(ce->senses); sp; sp = list_next(ce->senses))
    {
      Form *fp;
      for (fp = list_first(ce->forms); fp; fp = list_next(ce->forms))
	{
	  Form *ff = calloc(1+list_len(p->cgps), sizeof(Form));
	  ff[i] = *fp;
	  ff[i].sense = sp->mng;
	  ff[i].epos = sp->pos;
	  list_add(heads, ff);
	}
    }

  for (++i, cp = list_next(p->cgps); cp; cp = list_next(p->cgps), ++i)
    heads = psu_permute(heads, cp, i, (1+list_len(p->cgps)) * sizeof(Form));

  Form *ff;
  for (ff = list_first(heads); ff; ff = list_next(heads))
    {
      ff[0].form = psu_orth_form(ff, 1+list_len(p->cgps));
      ff[0].cf = ep->cgp->cf;
      ff[0].gw = ep->cgp->gw;
      ff[0].pos = ep->cgp->pos;
      ff[0].psu_ngram = psu_ngram(p->cgps);
      for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
	{
	  ff[0].sense = sp->mng;
	  ff[0].epos = sp->pos;
	  list_add(sigs, psu_one_sig(ff, 1+list_len(p->cgps)));
	}
    }
}

void
psu_sigs(Entry *ep)
{
  List *sigs = list_create(LIST_SINGLE);
  Parts *p;

  for (p = list_first(ep->parts); p; p = list_next(ep->parts))
    if (p->owner)
      psu_parts_sigs(sigs, ep, p);

  char *sig;
  for (sig = list_first(sigs); sig; sig = list_next(sigs))
    if (out_stdout)
      fprintf(stdout, "%s\n", sig);
    else
      cbd_sig_add_one((uccp)sig);
}
