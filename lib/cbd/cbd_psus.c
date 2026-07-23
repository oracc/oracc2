#include <oraccsys.h>
#include "cbd.h"

/* ON OUTPUT CREATE A BLOCK OF ALL PSU DATA FOR EACH PSU SENSE */

static Entry *
xcbd_find(Entry *ep, unsigned const char *cgp, Cbd *x)
{
  Cbd *c;
  for (c = list_first(csetp->lcbds); c; c = list_next(csetp->lcbds))
    if (x == c)
      continue;
    else if ((ep = hash_find(c->hentries, cgp)))
      return ep;
  return NULL;
}

static void
psu_index(Entry *part, Entry *psu)
{
  List *lp = hash_find(csetp->hpsus, part->cgp->tight);
  if (!lp)
    hash_add(csetp->hpsus, part->cgp->tight, (lp = list_create(LIST_SINGLE)));
  list_add(lp, psu);
}

#if 0
static char *
sanitize_cgp(const char *cfg)
{
  char buf[strlen(cfg)], *bp = buf;
  const char *s = cfg, *x;
  *bp = '\0';
  while (*s)
    {
      while (' ' == *s)
	++s;
      if ('n' == s[0] && ' ' == s[1])
	s += 2;
      else if ((x = strstr(s, "//")))
	{
	  while (s < x)
	    *bp++ = *s++;
	  *bp = '\0';
	  s = strchr(s, ']');
	  while (*s && '\'' != *s)
	    *bp++ = *s++;
	  *bp = '\0';
	  while (*s && !isspace(*s))
	    ++s;
	}
      else if ((x = strchr(s, '\'')))
	{
	  while (s < x)
	    *bp++ = *s++;
	  *bp = '\0';
	  s += strlen(s);
	}
      else
	{
	  while (*s)
	    *bp++ = *s++;
	  *bp = '\0';
	}
    }
  return strdup(buf);
}
#endif

/* validate @parts, linking each cgp to its Entry.
 *
 * Two things need better implementation:
 *
 * n in @parts, e.g., n ŋal[be]V/it--these are currently just ignored when checking
 *
 * //SENSE in @parts CGP; this is currently stripped out in checking but should use SENSE_GW; similarly for EPOS
 *
 */
void
cbd_psus(void)
{
  static int psu_verbose = 0;
  Parts *p;
  for (p = list_first(csetp->parts); p; p = list_next(csetp->parts))
    {
      int cgp_fails = 0;
      Cbd *pcbd = p->owner->owner;
      Cgp *cp;
      Hash *seen = hash_create(0);
      if (psu_verbose)
	fprintf(stderr, "cbd_psus: %s\n", p->owner->cgp->tight);
      p->f.parts = memo_array_new(csetp->formspmem, list_len(p->cgps));
      /* Each Cgp here is actually a signature broken into CF,
	 GW//SENSE, POS-etc.  This is already reassembled by cgp_save
	 as cgp->tight using pool_copy so we can safeley split it
	 using form_parse */
      int i;
      for (i = 0, cp = list_first(p->cgps); cp; cp = list_next(p->cgps), ++i)
#if 1
	{
	  form_parse(p->mloc->file, p->mloc->line, cp->tight, p->f.parts[i], NULL);
	  cp->loose = cp->tight = NULL;
	  cgp_init(cp, form[i].cf, form[i].gw, form[i].pos); /* reset all cgp fields */
	  Entry *ep = hash_find(pcbd->hentries, (uccp)cp->tight);
	  if (ep)
	    cp->owner = ep;
	  else
	    {
	      if ((ep = xcbd_find(ep,(uccp)cp->tight,pcbd)))
		cp->owner = ep;
	      else
		{
		  mesg_verr(&p->l, "part %s not found in any glossary", cp->tight);
		  cp->owner = NULL; /* flag value to indicate that no
				      future processing of @parts
				      should be done */
		}
	    }
	  if (cp->owner)
	    {
	      if (!hash_find(seen, (uccp)c))
		{
		  psu_index(cp->owner, p->owner); /* index the current part
						     as occurring in the
						     current PSU */
		  /* if a part occurs more than once in a PSU only index
		     it once to avoid faffing with uniqing later */
		  hash_add(seen, (uccp)c, "");
		}
	    }
	  else
	    ++cgp_fails;
	}
      hash_free(seen, NULL);
      /* All the CGPs are valid; finish up the Form which encapsulates the
	 @entry and @parts data */
      if (!cgp_fails)
	{
	  bit_set(p->f.flags, FORM_FLAGS_PSU_PART);
	  p->f.cf = p->owner->cgp->cf;
	  p->f.gw = p->owner->cgp->gw;
	  p->f.pos = p->owner->cgp->pos;
	}
      else
	p->f.parts = NULL;
    }
#else
  {
    char *c = (char*)cp->tight, *m = NULL;
    if (('n' == c[0] && ' ' == c[1])
	|| strstr(c, " n ")
	|| strstr(c, "//")
	|| strchr(c, '\''))
      c = m = sanitize_cgp(c);
    /*fprintf(stderr, "cbd_psus: need %s in %p ...", c, pcbd->hentries);*/
    Entry *ep = hash_find(pcbd->hentries, (uccp)c);
    if (ep)
      {
	/*fprintf(stderr, "ok\n");*/
	cp->owner = ep;
      }
    else
      {
	/*fprintf(stderr, "nope\n");*/
	if ((ep = xcbd_find(ep,(uccp)c,pcbd)))
	  cp->owner = ep;
	else
	  {
	    mesg_verr(&p->l, "part %s not found in any glossary", cp->tight);
	    p->owner = NULL; /* flag value to indicate that no
				future processing of @parts
				should be done */
	  }
      }
    if (cp->owner && !hash_find(seen, (uccp)c))
      {
	psu_index(cp->owner, p->owner); /* index the current part
					   as occurring in the
					   current PSU */
	/* if a part occurs more than once in a PSU only index
	   it once to avoid faffing with uniqing later */
	hash_add(seen, (uccp)c, "");
      }
    if (m)
      free(m);
  }
  hash_free(seen, NULL);  
}
#endif
}

static List *
cpf_try_parts(Parts *p, List *ffs)
{
  List *fok = list_create(LIST_SINGLE);
  /* For each of the forms in ffs, see if p[i] makes a known fcgp; if so,
     continue, if not, break; after this loop, if p->f.parts[i] is NULL
     we have a successful match */
  char *f;
  Cgp *c;
  int i;
  for (i = 0, f = list_first(ffs), c = list_first(p->cgps);
       f && c; ++i, f = list_next(ffs), c = list_next(p->cgps))
    {
      char fcgp[strlen(f)+strlen(c->tight)+2];
      sprintf(fcgp, "%s=%s", f, c->tight);
      Cform *ok = hash_find(p->owner->owner->hfcgps, fcgp);
      if (ok)
	list_add(fok, ok);
      else
	break;
    }
  if (NULL != p->f.parts[i]) /* success */
    {
      list_free(fok);
      fok = NULL;
    }
  return fok;
}

static int
cpf_try_form(Cform *f)
{
  char buf[strlen(f->f.form)+1];
  strcpy(buf, f->f.form);
  char *lvecstr = list_vec_sep_str;
  list_vec_sep_str = "_";
  char **ffs = list_from_str(buf);
  list_vec_sep_str = lvecstr;

  Parts *p;
  List *fok;
  for (p = list_first(f->e->parts); p; p = list_next(f->e->parts))
    {
      if ((fok = cpf_try_parts(p, ffs)))
	break;
    }

  if (fok)
    {
      /* forms sequence matches @parts: set up f->f as a form with parts and with PSU_FLAGS_PSU_FORM set */
      bit_set(f->f.flags, FORM_FLAGS_PSU_FORM);
      f->f.parts = memo_new_array(csetp->formspmem, list_len(fok)+1);
      int i;
      Form *okf, *pf;
      for (i = 0, okf = list_first(fok); okf; okf = list_next(okf))
	{
	  *f->f.parts[i] = *p->f.parts[i];
	  f->f.parts[i]->form = okf->f.form;
	  f->f.parts[i]->lang = okf->f.lang;
	  list_free(fok, NULL);
	}
    }

  return p != NULL;
}

void
cbd_psu_forms(void)
{
  Entry *e;
  for (e = list_first(csetp->ewithparts); e; e = list_next(csetp->ewithparts))
    {
      Cform *f;
      List *pforms = list_create(LIST_SINGLE); /* list of forms that belong to @parts */
      int i;
      for (i = 0, f = list_first(e->forms); f; f = list_next(e->forms), ++i)
	{
	  if (!cpf_try_form(f))
	    mesg_verr(f->mloc, "form %s does not match any @parts spec", f->f.form);
	}
    }
}

static unsigned char *
cfp_corgi(Form *fp)
{
  
}

unsigned char *
cbd_psu_sig(Form *fp)
{
  int corgi = 1;
  int i;
  for (i = 0; fp->parts[i]; ++i)
    {
      if (!fp->sense && !fp->morph)
	{
	  corgi = 0;
	  break;
	}
    }
  unsigned char *sig = form_sig(csetp->pool, fp);
  if (corgi)
    {
      char *tail = strstr(sig, "}::");
      tail += 3;
      *tail = '\0';
    }
  return sig;
}
