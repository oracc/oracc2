#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

static List *
psu_permute(List *heads, Cgp *cp, int ffi, int nbytes)
{
  List *nheads = list_create(LIST_SINGLE);
  Entry *ce = cp->owner;
  Sense *sp;
  Cform *hff;
  for (hff = list_first(heads); hff; hff = list_next(heads))
    for (sp = list_first(ce->senses); sp; sp = list_next(ce->senses))
      {
	Cform *fp;
	for (fp = list_first(ce->forms); fp; fp = list_next(ce->forms))
	  {
	    Cform *ff = malloc(nbytes);
	    memcpy(ff, hff, nbytes);
	    ff[ffi] = *fp;
	    ff[ffi].f.sense = sp->mng;
	    ff[ffi].f.epos = sp->pos;
	    list_add(nheads, ff);
	  }
      }
  return nheads;
}

void
psu_out_sig(Cform *ff, int n, FILE *fp)
{
  fprintf(fp,
	  "{%s = %s += %s[%s//%s]%s'%s}::",
	  ff[0].f.form, ff[0].f.psu_ngram,
	  ff[0].f.cf, ff[0].f.gw, ff[0].f.sense, ff[0].f.pos, ff[0].f.epos);
  int i;
  for (i = 1; i < n; ++i)
    {
      fprintf(fp, "%s", form_sig(csetp->pool, &ff[i].f));
      if (n - i > 1)
	fputs("++", fp);
    }
}

unsigned char *
psu_one_sig(Cform *f, int n)
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
psu_orth_form(Cform *ff, int n)
{
  int i, len = 0;
  for (i = 1; i < n; ++i)
    len += strlen((ccp)ff[i].f.form);
  len += n;
  char buf[len+2];
  *buf = '\0';
  for (i = 1; i < n; ++i)
    {
      strcat(buf, (ccp)ff[i].f.form);
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
      Cform *fp;
      for (fp = list_first(ce->forms); fp; fp = list_next(ce->forms))
	{
	  Cform *ff = calloc(1+list_len(p->cgps), sizeof(Cform));
	  ff[i] = *fp;
	  ff[i].f.sense = sp->mng;
	  ff[i].f.epos = sp->pos;
	  list_add(heads, ff);
	}
    }

  for (++i, cp = list_next(p->cgps); cp; cp = list_next(p->cgps), ++i)
    heads = psu_permute(heads, cp, i, (1+list_len(p->cgps)) * sizeof(Cform));

  Cform *ff;
  for (ff = list_first(heads); ff; ff = list_next(heads))
    {
      ff[0].f.form = psu_orth_form(ff, 1+list_len(p->cgps));
      ff[0].f.cf = ep->cgp->cf;
      ff[0].f.gw = ep->cgp->gw;
      ff[0].f.pos = ep->cgp->pos;
      ff[0].f.psu_ngram = psu_ngram(p->cgps);
      for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
	{
	  ff[0].f.sense = sp->mng;
	  ff[0].f.epos = sp->pos;
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
      cbd_sig_add_one((uccp)sig, 0);
}
