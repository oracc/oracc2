#include <oraccsys.h>
#include "cbd.h"

/* Because this is executed on entries with bases it's convenient to
 * auto-generate NORM as needed.
 */

static void
cbd_auto_norm(Form *fp)
{
  if (fp->morph)
    {
      char n[strlen((ccp)fp->cf)+strlen((ccp)fp->morph)];
      char *tilde = strchr((ccp)fp->morph, '~');
      if (tilde)
	{
	  int len = tilde - (char*)fp->morph;
	  if (len)
	    {
	      strncpy(n, (ccp)fp->morph, len);
	      n[len] = '\0';
	    }
	  else
	    *n = '\0';
	  strcat(n, (ccp)fp->cf);
	  ++tilde;
	  strcat(n, tilde);
	  fp->norm = pool_copy((uccp)n, csetp->pool);
	}
    }
  else
    {
      fp->norm = fp->cf;
    }
}

void
cbd_no_form_bases(Entry *ep)
{
  /* index the bases that occur in @form with morph=#~ */
  Hash *fb = hash_create(5);
  Form *fp;
  for (fp = list_first(ep->forms); fp; fp = list_next(ep->forms))
    {
      if (fp->base)
	{
	  if (fp->morph && !strcmp((ccp)fp->morph, "~"))
	    hash_add(fb, fp->base, "");
	  if (!fp->norm)
	    cbd_auto_norm(fp);
	}
    }

  /* now go through the primary bases and create a basic @form for
     those not in the form-base index */
  List_node *outer;
  for (outer = ep->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      struct loctok *ltp = (struct loctok *)inner->data;
      if (!hash_find(fb, ltp->tok))
	{
	  Cform *nfbf = cbd_bld_form(ep->l, ep);
	  BIT_SET(nfbf->f.flags, FORM_FLAGS_IMPLICIT);
	  nfbf->f.lang = (uccp)(ltp->lang ? ltp->lang : "sux");
	  nfbf->f.form = nfbf->f.base = ltp->tok;
	  nfbf->f.morph = (uccp)"~";
	  cbd_bld_form_setup(ep, nfbf);
	  nfbf->f.norm = nfbf->f.cf;
	}
    }
  hash_free(fb, NULL);
}

