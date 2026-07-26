#include <oraccsys.h>
#include <gt.h>
#include "cbd.h"

static Cgp n_cgp = {
  .cf=(uccp)"" , .gw=(uccp)"", .pos=(uccp)"n",
  .tight=(uccp)"n", .loose=(uccp)"n" 
};

static Entry n_entry = {
  .l = { .file="<builtin>", .line=1 },
  .oid = "o0040000",
  .cgp = &n_cgp
};

static Cform n_cform = {
  .l = { .file="<builtin>", .line=1 },
  .e = &n_entry,
  .f = { .form = (uccp)"n", .pos = (uccp)"n" } 
};

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
      p->f.parts = memo_new_array(csetp->formspmem, list_len(p->cgps)+1);
      /* Each Cgp here is actually a signature broken into CF,
	 GW//SENSE, POS-etc.  This is already reassembled by cgp_save
	 as cgp->tight using pool_copy so we can safeley split it
	 using form_parse */
      int i;
      for (i = 0, cp = list_first(p->cgps); cp; cp = list_next(p->cgps), ++i)
	{
	  p->f.parts[i] = memo_new(csetp->formsmem);
	  form_parse((uccp)p->l.file, p->l.line, (ucp)cp->tight, p->f.parts[i], NULL);
	  p->f.parts[i]->project = pcbd->project;
	  if (strcmp((ccp)cp->loose, "n"))
	    {
	      cp->loose = cp->tight = NULL;
	      /* reset all cgp fields */	  
	      cgp_init(cp, (ucp)p->f.parts[i]->cf, (ucp)p->f.parts[i]->gw, (ucp)p->f.parts[i]->pos);
	    }
	  Entry *ep = hash_find(pcbd->hentries, (uccp)cp->tight);
	  if (ep)
	    cp->owner = ep;
	  else
	    {
	      if (!strcmp((ccp)cp->tight, "n"))
		cp->owner = &n_entry;
	      else if ((ep = xcbd_find(ep,(uccp)cp->tight,pcbd)))
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
	      if (!hash_find(seen, (uccp)cp->tight))
		{
		  psu_index(cp->owner, p->owner); /* index the current part
						     as occurring in the
						     current PSU */
		  /* if a part occurs more than once in a PSU only index
		     it once to avoid faffing with uniqing later */
		  hash_add(seen, (uccp)cp->tight, "");
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
	  p->f.project = p->owner->owner->project;
	  p->f.cf = p->owner->cgp->cf;
	  p->f.gw = p->owner->cgp->gw;
	  p->f.pos = p->owner->cgp->pos;
	  p->f.user = p->owner; /* set form->user to entry that this form belongs to */
	}
      else
	p->f.parts = NULL;
    }
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
  int zeroes = 0;
  int i;
  for (i = 0, f = list_first(ffs), c = list_first(p->cgps);
       f && c; ++i, f = list_next(ffs), c = list_next(p->cgps))
    {
      if (!strcmp(f, "tu₂"))
	fprintf(stderr, "found tu₂\n");
      if (!f[1])
	{
	  if ('0' == *f)
	    ++zeroes; /* allow i to reach the end of p->f.parts */
	  else if ('n' == *f)
	    {
	      /* This should check we are matching against 'n' in @parts */
	      if (!n_cform.t)
		n_cform.t = gt_token(&n_cform.l, (ucp)memo_dup((ccp)n_cform.f.pos), 0, NULL);
	      list_add(fok, &n_cform);
	    }
	}
      else
	{
	  char fcgp[strlen(f)+strlen((ccp)c->tight)+2];
	  sprintf(fcgp, "%s=%s", f, c->tight);
	  Cform *ok = hash_find(p->owner->owner->hfcgps, (uccp)fcgp);
	  if (ok)
	    list_add(fok, ok);
	  else if ('n' == *p->f.parts[i]->pos && isdigit(*f) && strchr(f, '('))
	    list_add(fok, &n_cform);
	  else
	    break;
	}
    }
  if (NULL != p->f.parts[i]) /* success if p->f.parts[i] == NULL */
    {
      list_free(fok, NULL);
      fok = NULL;
    }
  return fok;
}

static int
cpf_try_form(Cform *f)
{
  char buf[strlen((ccp)f->f.form)+1];
  strcpy(buf, (ccp)f->f.form);
  const char *lvecstr = list_vec_sep_str;
  list_vec_sep_str = "_";
  List *ffs = list_from_str(buf, NULL, LIST_SINGLE);
  list_vec_sep_str = lvecstr;

  Parts *p;
  List *fok = NULL;
  for (p = list_first(f->e->parts); p; p = list_next(f->e->parts))
    {
      if (!p->f.parts)
	mesg_verr(&f->l, "NULL p->f.parts");
      else if ((fok = cpf_try_parts(p, ffs)))
	break;
    }

  if (fok)
    {
      /* forms sequence matches @parts: set up f->f as a form with parts
	 and with PSU_FLAGS_PSU_FORM set */
      bit_set(f->f.flags, FORM_FLAGS_PSU_FORM);
      f->f.parts = memo_new_array(csetp->formspmem, list_len(fok)+1);
      int i;
      Cform *okf;
      for (i = 0, okf = list_first(fok); okf; okf = list_next(fok), ++i)
	{
	  f->f.parts[i] = memo_new(csetp->formsmem);
	  *f->f.parts[i] = *p->f.parts[i];
	  f->f.parts[i]->form = okf->f.form;
	  f->f.parts[i]->lang = okf->f.lang;
	}
      list_free(fok, NULL);
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
      /*List *pforms = list_create(LIST_SINGLE);*/ /* list of forms that belong to @parts */
      int i;
      for (i = 0, f = list_first(e->forms); f; f = list_next(e->forms), ++i)
	{
	  if (!cpf_try_form(f))
	    mesg_verr(&f->l, "form %s does not match any @parts spec", f->f.form);
	}
    }
}

static void
cbd_psu_sig_key(FILE *bufp, Form *fp, uccp sense)
{
  fputc('{', bufp);
  int i;
  for (i = 0; fp->parts[i]; ++i)
    {
      if (i)
	fputc(' ', bufp);
      if (!strcmp((ccp)fp->parts[i]->pos, "n"))
	fputc('n', bufp);
      else
	fprintf(bufp, "%s[%s]%s", fp->parts[i]->cf, fp->parts[i]->gw, fp->parts[i]->pos);
    }
  if (sense)
    fprintf(bufp, " += %s[%s//%s]%s}::", fp->cf, fp->gw, sense, fp->pos);
  else
    fprintf(bufp, " += %s[%s]%s}::", fp->cf, fp->gw, fp->pos);
}

static void
cbd_psu_sig_one_part(FILE *bufp, Form *fp)
{
  if (!strcmp((ccp)fp->pos, "n"))
    fputc('n', bufp);
  else
    {
      if (fp->lang)
	fprintf(bufp,"@%s%%%s:%s=%s[%s",fp->project,fp->lang,fp->form,fp->cf,fp->gw);
      else
	fprintf(bufp,"@%s%%%s:=%s[%s",
		fp->project, ((Entry *)fp->user)->owner->lang,fp->cf,fp->gw);

      if (fp->sense)
	fprintf(bufp, "//%s", fp->sense);

      fprintf(bufp, "]%s", fp->pos);
      if (fp->epos)
	fprintf(bufp, "'%s", fp->epos);

      if (fp->norm)
	fprintf(bufp,"$%s",fp->norm);

      if (fp->base)
	fprintf(bufp,"/%s",fp->base);

      if (fp->cont && *fp->cont)
	fprintf(bufp,"+%s",fp->cont);

      if (fp->morph)
	fprintf(bufp,"#%s",fp->morph);

      if (fp->morph2)
	fprintf(bufp,"##%s",fp->morph2);

      if (fp->stem)
	fprintf(bufp,"*%s",fp->stem);

      if (fp->rws)
	fprintf(bufp,"@%s",fp->rws);
    }
}

static void
cbd_psu_sig_parts(FILE *bufp, Form *fp)
{
  int i;
  for (i = 0; fp->parts[i]; ++i)
    {
      if (i)
	fputs("++", bufp);
      cbd_psu_sig_one_part(bufp, fp->parts[i]);
    }
}

List *
cbd_psu_sigs(Form *fp)
{
  if (!fp->parts)
    return NULL;

#if 0
  int corgi = 1;
  int i;
  for (i = 0; fp->parts[i]; ++i)
    if (!fp->sense && !fp->morph)
      {
	corgi = 0;
	break;
      }
#endif

  List *lsigs = list_create(LIST_SINGLE);
  Entry *ep = fp->user;
  Sense *sp;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    {
      char *sig_buf = NULL;
      size_t sig_len = 0;
      FILE *sig_fp = open_memstream(&sig_buf, &sig_len);
      cbd_psu_sig_key(sig_fp, fp, sp->sgw ? sp->sgw : sp->mng);
      /*if (!corgi)*/
	cbd_psu_sig_parts(sig_fp, fp);
      fclose(sig_fp);
      memo_list(sig_buf);
      list_add(lsigs, sig_buf);
    }

  return lsigs;
}
