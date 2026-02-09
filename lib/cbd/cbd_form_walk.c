#include <oraccsys.h>
#include <cbd.h>
#include <form.h>

/* Iterate over the Cbd calling a handler for each entry/sense/form.
 *
 * Handler takes three args:
 *
 *  Form * -- the current running form; must be cloned to persist
 *  Cbd_fw_type -- the type of the third arg, Entry, Sense, Form
 *  Void * -- the Cbd data structure from which the first arg was constructed
 */

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

static void
cbd_fw_psu_parts(Entry *ep, Parts *p, cbdfwfunc h)
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
      ff[0].e = ep;
      ff[0].f.lang = ff[1].f.lang;
      ff[0].f.form = psu_orth_form(ff, 1+list_len(p->cgps));
      ff[0].f.cf = ep->cgp->cf;
      ff[0].f.gw = ep->cgp->gw;
      ff[0].f.pos = ep->cgp->pos;
      ff[0].f.psu_ngram = psu_ngram(p->cgps);
      /*fprintf(stderr, "FW_PE: ");*/
      h(&ff[0], CBD_FW_PE, ep);
      for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
	{
	  ff[0].s = sp;
	  ff[0].f.sense = sp->mng;
	  ff[0].f.epos = sp->pos;
	  /*fprintf(stderr, "FW_PS parts: ");*/
	  h(&ff[0], CBD_FW_PS, sp);
	}
    }
}

static void
cbd_fw_psu(Entry *ep, cbdfwfunc h)
{
  Parts *p;
  for (p = list_first(ep->parts); p; p = list_next(ep->parts))
    cbd_fw_psu_parts(ep, p, h);
}

static void
cbd_fw_fields(List *forms, Cform *f, int context, void *vp, cbdfwfunc h)
{
  Cform *fp;
  for (fp = list_first(forms); fp; fp = list_next(forms))
    {
      if (BIT_ISSET(fp->f.flags, FORM_FLAGS_COF_TAIL))
	continue;

      f->f.base = fp->f.base;
      f->f.stem = fp->f.stem;
      f->f.cont = fp->f.cont;
      f->f.norm = fp->f.norm;
      f->f.morph = fp->f.morph;
      f->f.morph2 = fp->f.morph2;
      if (fp->f.lang)
	f->f.lang = fp->f.lang;
      char *n = strstr((ccp)f->f.lang, "/n");
      if (n)
	{
	  *n = '\0';
	  struct map *l949 = lang949((ccp)f->f.lang, strlen((ccp)f->f.lang));
	  if (l949)
	    f->f.lang = (uccp)l949->v;
	  else
	    fprintf(stderr,
		    "glosigx: no -949 version of lang %s in lang949.g; ignoring -949 here\n",
		    f->f.lang);		
	  f->f.form = (uccp)"*";
	}
      else
	f->f.form = fp->f.form;

      f->f.flags = fp->f.flags;
      f->f.parts = fp->f.parts;

      if ('s'==context && !fp->s)
	f->s = vp;

      h(&f[0], context=='e' ? CBD_FW_EF : CBD_FW_SF, fp);
    }
}

static void
cbd_fw_entry(Entry *ep, cbdfwfunc h)
{
  Cform f;
  memset(&f, '\0', sizeof(Cform));

  f.e = ep;
  f.f.project = ep->owner->project;
  f.f.lang = ep->owner->lang;
  f.f.cf = ep->cgp->cf;
  f.f.gw = ep->cgp->gw;
  f.f.pos = ep->cgp->pos;

  /*fprintf(stderr, "FW_E entry: ");*/
  h(&f, CBD_FW_E, ep);
  cbd_fw_fields(ep->forms, &f, 'e', ep, h);

  Sense *sp;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    {
      f.s = sp;
      f.f.sense = sp->mng;
      f.f.epos = sp->pos;

      h(&f, CBD_FW_S, sp);
      cbd_fw_fields(sp->forms ? sp->forms : ep->forms, &f, 's', sp, h);
      h(&f, CBD_FW_SE, sp);
    }

  h(&f, CBD_FW_EE, ep);
}

void
cbd_form_walk(Cbd *c, cbdfwfunc h)
{
  Entry *ep;
  for (ep = list_first(c->entries); ep; ep = list_next(c->entries))
    if (ep->parts)
      cbd_fw_psu(ep, h);
    else
      cbd_fw_entry(ep, h);
}
