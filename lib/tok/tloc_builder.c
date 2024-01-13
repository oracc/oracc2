#include <oraccsys.h>
#include <tok.h>

/* tloc_builder can work in single-unit mode or in multi-unit mode.
   
   In single-unit mode a single set of structures is continuously
   updated throughout the run.

   In multi-unit mode a each text/line/word has its own structures.
 */

#define list_add_r(lp,dp) list_add((lp),(dp)),dp
#define tlb_dup(s) (ccp)hpool_copy((ucp)(s),r->p)
#define tlb_error(e) fprintf(stderr,"%s\n",e)

Loch *
tlb_init(Trun *r, const char *project, const char *type)
{
  Loch *l = calloc(1, sizeof(Loch));
  l->project = project;
  l->type = type;
  if ('x' == *type)
    l->tlocs = list_create(LIST_DOUBLE);
  else
    l->x.tlocs = list_create(LIST_DOUBLE);
  r->l = l;
  return l;
}

void
tlb_term(Loch *l)
{
  list_free(l->tlocs, NULL);
}

void
tlb_T(Trun *r, const char *p, const char *id, const char *n)
{
  if (r)
    {
      if (!r->l->t || r->multi)
	{
	  r->l->t = memo_new(r->t_m);
	  list_add(r->l->tlocs, r->l->t);
	}
      loch_text(r)->file = r->rs.file;
      loch_text(r)->andline_num = r->rs.andline_num;
      loch_text(r)->text_project = tlb_dup(p);
      loch_text(r)->text_id = tlb_dup(id);
      loch_text(r)->text_name = tlb_dup(n);
      loch_text(r)->loch = r->l;
      loch_text(r)->llocs = list_create(LIST_DOUBLE);
    }
}

void
tlb_L(Trun *r, const char *num, const char *id, const char *lab)
{
  if (r)
    {
      if (!r->l->l || r->multi)
	{
	  r->l->l = memo_new(r->l_m);
	  if (loch_text(r)->llocs)
	    list_add(loch_text(r)->llocs, r->l->l);
	  else
	    tlb_error("line found before text is set");
	}
      loch_line(r)->line_num = tlb_dup(num);
      loch_line(r)->line_id = tlb_dup(id);
      loch_line(r)->line_label = tlb_dup(lab);
      loch_line(r)->t = r->l->t;
      loch_line(r)->wlocs = list_create(LIST_DOUBLE);
    }
}

void
tlb_W(Trun *r, const char *id, const char *lang, const char *form)
{
  if (r)
    {
      if (!r->l->w || r->multi)
	{
	  r->l->w = memo_new(r->w_m);
	  list_add(loch_line(r)->wlocs, r->l->w);
	}
      loch_word(r)->word_id = tlb_dup(id);
      loch_word(r)->word_lang = tlb_dup(lang);
      loch_word(r)->word_form = tlb_dup(form);
      loch_word(r)->l = r->l->l;
      loch_word(r)->w = &r->rw;
    }
}

void
tlb_K(Trun *r, const char *k, const char *v)
{
  if (!r->rs.k)
    r->rs.k = list_create(LIST_SINGLE);
  list_add(r->rs.k, k);
  list_add(r->rs.k, v);
}

void
tlb_K_wrapup(Trun *r)
{
  loch_text(r)->keys = list2array_c(r->k, &loch_text(r)->nkeys);
  list_free(r->rs.k);
  r->rs.k = NULL;
}

void
tlb_F(Trun *r, const char *filename)
{
  r->rs.file = tlb_dup(filename);
}

void
tlb_P(Trun *r, const char *project)
{
  r->l->project = tlb_dup(project);
}

void
tlb_Y(Trun *r, const char *rtype)
{
  r->l->type = tlb_dup(project);
}

void
tlb_G(Trun *r, const char *glosslang)
{
  
}

void
tlb_A(Trun *r, const char *oid, const char *cgp)
{
  
}

/* P(hrase) tokens are for anything that is put in the
   linkbase of an XTF file */
void
tlb_P(Trun *r, const char *type, const char *wids, const char *oid, const char *name)
{
}
