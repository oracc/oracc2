#include <oraccsys.h>
#include "cfy.h"

static Line *curr_line = NULL;
Elt e_hp = { .etype=ELT_Hp };

static void
cfy_charify(Cfy *c, Elt *ep, List *lp)
{
  size_t ulen;
  wchar_t *w = utf2wcs((uccp)ep->data, &ulen);
  int i;
  for (i = 0; i < ulen; ++i)
    {
      if (w[i+1] == 0x200d)
	{
	  wchar_t lig[4];
	  lig[0]=w[i++]; lig[1]=w[i++]; lig[2]=w[i]; lig[3]=L'\0';
	  ep->data = hpool_copy(wcs2utf(lig, 3), c->hp);
	  list_add(lp, ep);
	}
      else
	{
	  wchar_t g[2];
	  g[0] = w[i]; g[1]=L'\0';
	  Elt *xep = elt_clone(c, ep);
	  xep->data = hpool_copy(wcs2utf(g, 1), c->hp);
	  list_add(lp, xep);
	  if (w[i+1])
	    {
	      Elt *ep2 = memo_new(c->m_elt);
	      ep2->etype = ELT_Sp;
	      curr_line->last_w = list_len(lp);
	      list_add(lp, ep2);
	    }
	}
    }
  /*free(w);*//* this is not how to free utf2wcs mem */
}

void
cfy_charspace(Cfy *c, Eltline *elp)
{
  int i = 0;
  List *lp = list_create(LIST_SINGLE);
  curr_line = elp->epp[i]->data;
  for (i = 0; elp->epp[i]; ++i)
    {
      if (ELT_G == elp->epp[i]->etype)
	{
	  if (elp->epp[i]->prev->etype == ELT_G)
	    list_add(lp, &e_hp);
	  cfy_charify(c, elp->epp[i], lp);
	}
      else
	{
	  if (ELT_W == elp->epp[i]->etype)
	    curr_line->last_w = list_len(lp);
	  list_add(lp, elp->epp[i]);
	}
    }
  elp->epp = (Elt **)list2array_c(lp, &elp->len);
}
