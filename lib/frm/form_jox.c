#include <oraccsys.h>
#include <xml.h>
#include <joxer.h>
#include <rnvif.h>
#include <rnvxml.h>
#include "form.h"
#include "ilem_form.h"
#include "l3props.h"

extern int lem_extended;

char *
untildify(Form *fp, const unsigned char *tilde)
{
  char buf[strlen((ccp)fp->morph)+strlen((fp->cf?(ccp)fp->cf:"X"))+1], *dst = buf;
  const char *m = (ccp)fp->morph;
  while (m != (ccp)tilde)
    *dst++ = *m++;
  if (fp->cf)
    strcat(dst, (ccp)fp->cf);
  else
    strcat(dst, "X");
  strcat(dst, ++m);
  return strdup(buf);
}

static void
dump_norm(List *ap, Form *fp)
{
  const unsigned char *tilde = (unsigned char *)strchr((char*)fp->morph,'~');
  if (tilde)
    joxer_attr(ap,"norm0",untildify(fp, tilde));
  else if (fp->norm)
    joxer_attr(ap,"norm",(char*)fp->norm);
  else if (fp->cf)
    joxer_attr(ap,"norm",(char*)fp->cf);
  else
    {
      /* no norm0 */
    }
}

void
form_serialize_jox(Form *f)
{
  List *ap = list_create(LIST_SINGLE);
  joxer_attr(ap,"xml:lang",(char*)f->lang);
  if (f->oform && strlen((const char *)f->oform))
    {
      /* always treat the corpus's version of a form as primary */
      joxer_attr(ap,"form",(char*)f->oform);
      joxer_attr(ap,"aform",(char*)f->form);
    }
  else
    joxer_attr(ap,"form",(char*)f->form);
  /*  joxer_attr(ap,"oform",(char*)f->oform); */
  joxer_attr(ap,"cf",(char*)f->cf);
  if (f->cf)
    joxer_attr(ap,"gw",(char*)f->gw);
  joxer_attr(ap,"sense",(char*)f->sense);
  if (f->morph && lem_extended)
    dump_norm(ap, f);
  else
    joxer_attr(ap,"norm",(char*)f->norm);
  joxer_attr(ap,"pos",(char*)f->pos);
  joxer_attr(ap,"epos",(char*)f->epos);
  joxer_attr(ap,"base",(char*)f->base);
  joxer_attr(ap,"cont",(char*)f->cont);
  joxer_attr(ap,"stem",(char*)f->stem);
  joxer_attr(ap,"morph",(char*)f->morph);
  joxer_attr(ap,"morph2",(char*)f->morph2);
  joxer_attr(ap,"rws",(char*)f->rws);
  Ratts *ratts = ratts_list2ratts(ap);
  joxer_ec(mloc_file_line((ccp)f->file, f->lnum), "xff:f", ratts);
}

#if 0
/* This function is not called by any of the o2 library routines so it
   probably isn't needed any more; it can't work with joxer because of
   the manual emission of tags in the has_found stuff at the end */
void
form_serialize_form_2(FILE *f_f2, struct ilem_form *f)
{
  extern int lem_extended;
  int has_found = f->finds ? f->fcount : 0;
  int finds_index = 0;
  struct ilem_form**ffinds = f->finds;
  if (has_found < 0)
    has_found = 0;
  /* First print the principal form unconditionally */
  while (f)
    {
#if 0
      joxer_attr(ap,"rws",f->rws);
#endif
      joxer_attr(ap,"rws_cf",f->rws_cf);
      joxer_attr(ap,"aform",f->aform);
      joxer_attr(ap,"type",f->type);
      if (f->result == -1 || f->invalid == 1)
	joxer_attr(ap,"invalid","1");
      joxer_attr(ap,"literal",f->literal);
      joxer_attr(ap,"sublem",f->sublem);
#if 0
      joxer_attr(ap,"restrictor",f->restrictor);
#endif
      joxer_attr(ap,"fdisamb",f->fdisamb);
      joxer_attr(ap,"sdisamb",f->sdisamb);

#if 0
      form_attr_i("freq", f->freq);
      form_attr_i("pct", f->pct);
#endif

      if (f->newflag)
	joxer_attr(ap, "new", "1");
      if (f->explicit&M_NORM0)
	joxer_attr(ap, "ex_norm0", "1");
      if (f->explicit&M_SENSE)
	joxer_attr(ap, "ex_sense", "1");
      if (f->is_part == 1)
	joxer_attr(ap, "is_part", "1");

      Ratts *ratts = ratts_list2ratts(ap);

      if (f->props)
	{
	  joxer_ea(f->np->mloc, "xff:f", ratts);
	  props_dump_props(f,f_f2);
	  joxer_ee(f->np->mloc, "xff:f");
	}
      else
	joxer_ec(f->np->mloc, "xff:f", ratts);

      if (has_found)
	{
	  if (finds_index)
	    fputs("</l>", f_f2);
	  else
	    fprintf(f_f2,"<ll type=\"finds\">");
	  if (finds_index < has_found)
	    {
	      f = ffinds[finds_index++];
	      fputs("<l>",f_f2);
	    }
	  else
	    f = NULL;
	}
      else
	f = NULL;
    }
  if (has_found)
    fprintf(f_f2,"</ll>");
}
#endif
