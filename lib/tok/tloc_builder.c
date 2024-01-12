#include <oraccsys.h>
#include <tok.h>

/* tloc_builder can work in single-unit mode or in multi-unit mode.
   
   In single-unit mode a single set of structures is continuously
   updated throughout the run.

   In multi-unit mode a each text/line/word has its own structures.
 */

#define list_add_r(lp,dp) list_add((lp),(dp)),dp

Loch *
tlb_init(int many)
{
  Loch *l = malloc(sizeof(Loch));
  l->t_m = memo_init(sizeof(Tloc));
  l->l_m = memo_init(sizeof(Lloc));
  l->w_m = memo_init(sizeof(Wloc));
  l->tlocs = list_create(LIST_DOUBLE);
  return l;
}

void
tlb_term(Loch l)
{
  memo_term(l->t_m);
  memo_term(l->l_m);
  memo_term(l->w_m);
  list_free(l->tlocs, NULL);
}

void
tlb_T(Trun r, const char *p, const char *id, const char *n)
{
  if (r)
    {
      if (!r->l->tloc || r->multi)
	r->tloc = memo_new(r->t_m);
      list_add(r->l->tlocs, r->l->tloc);
      
    }
}
