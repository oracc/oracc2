#include <oraccsys.h>
#include <tok.h>

/* tloc_builder can work in single-unit mode or in multi-unit mode.
   
   In single-unit mode a single set of structures is continuously
   updated throughout the run.

   In multi-unit mode a each text/line/word has its own structures.
 */

#define tlb_dup(s) (ccp)hpool_copy((ucp)(s),r->p)
#define tlb_error(e) fprintf(stderr,"%s\n",e)

Loch *
tlb_init(Trun *r, const char *project, const char *type)
{
  Loch *l = calloc(1, sizeof(Loch));
  r->l = l;
  l->project = project;
  l->type = type;
  loch_xtf(r).tlocs = list_create(LIST_DOUBLE);
  return l;
}

void
tlb_term(Trun *r)
{
  if (loch_text(r)->keys)
    free(loch_text(r)->keys);
#if 0
  if ('c' == *r->l->type)
    list_free(loch_cbd(r).alocs, NULL);
  else
#endif
    list_free(loch_xtf(r).tlocs, NULL);
}

void
tlb_T(Trun *r, const char *p, const char *id, const char *n)
{
  if (r)
    {
      if (!loch_text(r) || r->multi)
	{
	  loch_text(r) = memo_new(r->t_m);
	  list_add(loch_xtf(r).tlocs, loch_text(r));
	}
      loch_text(r)->file = r->rs.file;
      loch_text(r)->andline_num = r->rs.andline_num;
      loch_text(r)->text_project = tlb_dup(p);
      loch_text(r)->text_id = tlb_dup(id);
      loch_text(r)->text_name = tlb_dup(n);
      loch_text(r)->loch = r->l;
      loch_text(r)->llocs = list_create(LIST_DOUBLE);
      if (loch_text(r)->keys)
	{
	  free(loch_text(r)->keys);
	  loch_text(r)->keys = NULL;
	}
      loch_text(r)->nkeys = -1;	
    }
}

void
tlb_L(Trun *r, int num, const char *id, const char *lab)
{
  if (r)
    {
      if (!loch_line(r) || r->multi)
	{
	  loch_line(r) = memo_new(r->l_m);
	  if (loch_text(r)->llocs)
	    list_add(loch_text(r)->llocs, loch_line(r));
	  else
	    tlb_error("line found before text is set");
	}
      loch_line(r)->line_num = num;
      loch_line(r)->line_id = tlb_dup(id);
      loch_line(r)->line_label = tlb_dup(lab);
      loch_line(r)->t = loch_text(r);
      loch_line(r)->wlocs = list_create(LIST_DOUBLE);
    }
}

void
tlb_W(Trun *r, const char *id, const char *lang, const char *form)
{
  if (r)
    {
      if (!loch_word(r) || r->multi)
	{
	  loch_word(r) = memo_new(r->w_m);
	  list_add(loch_line(r)->wlocs, loch_word(r));
	}
      loch_word(r)->word_id = tlb_dup(id);
      loch_word(r)->word_lang = tlb_dup(lang);
      loch_word(r)->word_form = tlb_dup(form);
      loch_word(r)->l = loch_line(r);
    }
}

void
tlb_K(Trun *r, const char *k, const char *v, const char *c)
{
  if (!r->rs.k)
    r->rs.k = list_create(LIST_SINGLE);
  list_add(r->rs.k, (void*)tlb_dup(k));
  list_add(r->rs.k, (void*)tlb_dup(v));
  list_add(r->rs.k, (void*)tlb_dup(c));
}

void
tlb_K_wrapup(Trun *r)
{
  if (loch_text(r)->keys)
    free(loch_text(r)->keys);
  loch_text(r)->keys = (const char **)list2array_c(r->rs.k, &loch_text(r)->nkeys);
  list_free(r->rs.k, NULL);
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
  r->l->type = tlb_dup(rtype);
}

void
tlb_G(Trun *r, const char *glosslang)
{
  
}

void
tlb_A(Trun *r, const char *oid, const char *cgp)
{
  
}

void
tlb_M(Trun *r, const char *id, const char *ref)
{
  
}

