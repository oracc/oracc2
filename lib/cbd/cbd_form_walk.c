#include <oraccsys.h>
#include <cbd.h>
#include <form.h>
#include <gt.h>

/* Iterate over the Cbd calling a handler for each entry/sense/form.
 *
 * Handler takes three args:
 *
 *  Form * -- the current running form; must be cloned to persist
 *  Cbd_fw_type -- the type of the third arg, Entry, Sense, Form
 *  Void * -- the Cbd data structure from which the first arg was constructed
 */

static void
cbd_fw_fields(List *forms, Cform *f, int context, void *vp, cbdfwfunc h)
{
  Cform *fp;
  for (fp = list_first(forms); fp; fp = list_next(forms))
    {
      if (BIT_ISSET(fp->f.flags, FORM_FLAGS_COF_TAIL))
	continue;

      f->l = fp->l;
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

      if ('s'==context)
	f->s = fp->s = vp;

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
    cbd_fw_entry(ep, h);
}
