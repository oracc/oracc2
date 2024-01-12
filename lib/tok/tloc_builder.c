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
  l->tlocs = list_create(LIST_DOUBLE);
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
      r->l->t->file = r->rs.file;
      r->l->t->andline_num = r->rs.andline_num;
      r->l->t->text_project = tlb_dup(p);
      r->l->t->text_id = tlb_dup(id);
      r->l->t->text_name = tlb_dup(n);
      r->l->t->loch = r->l;
      r->l->t->llocs = list_create(LIST_DOUBLE);
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
	  if (r->l->t->llocs)
	    list_add(r->l->t->llocs, r->l->l);
	  else
	    tlb_error("line found before text is set");
	}
      r->l->l->line_num = tlb_dup(num);
      r->l->l->line_id = tlb_dup(id);
      r->l->l->line_label = tlb_dup(lab);
      r->l->l->t = r->l->t;
      r->l->l->wlocs = list_create(LIST_DOUBLE);
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
	  list_add(r->l->l->wlocs, r->l->w);
	}
      r->l->w->word_id = tlb_dup(id);
      r->l->w->word_lang = tlb_dup(lang);
      r->l->w->word_form = tlb_dup(form);
      r->l->w->l = r->l->l;
      r->l->w->w = &r->rw;
    }
}
