#include <oraccsys.h>
#include "ofp.h"

/* Collect all the glyphs that belong to each sign */
void
ofp_marshall(Ofp *ofp)
{
  Ofp_liga *curr_lig = NULL;
#define gp(x) (&ofp->glyphs[x])
  int i;
  Ofp_sign *curr_sp = hash_find(ofp->h_sign, (uccp)gp(0)->key);
  for (i = 0; i < ofp->nglyphs; ++i)
    {
      if (strcmp(curr_sp->glyph->key, gp(i)->key))
	curr_sp = hash_find(ofp->h_sign, (uccp)gp(i)->key);
      if (gp(i)->ligl)
	{
	  if (!curr_lig || strcmp(gp(i)->liga, curr_lig->glyph->liga))
	    {
	      curr_lig = memo_new(ofp->m_liga);
	      if (!curr_sp->ligas)
		curr_sp->ligas = list_create(LIST_SINGLE);
	      list_add(curr_sp->ligas, curr_lig);
	      curr_lig->glyph = gp(i);
	    }
	  switch (gp(i)->f)
	    {
	    case OFPF_CVNN:
	      if (!curr_lig->cvnns)
		curr_lig->cvnns = list_create(LIST_SINGLE);
	      list_add(curr_lig->cvnns, gp(i));
	      break;
	    case OFPF_SALT:
	      if (!curr_lig->salts)
		curr_lig->salts = list_create(LIST_SINGLE);
	      list_add(curr_lig->salts, gp(i));
	      break;
	    case OFPF_SSET:
	      if (!curr_lig->ssets)
		curr_lig->ssets = list_create(LIST_SINGLE);
	      list_add(curr_lig->ssets, gp(i));
	      break;
	    default:
	      /* nowt */
	      break;
	    }
	  if (gp(i)->ivs)
	    {
	      if (!curr_lig->oivs)
		curr_lig->oivs = list_create(LIST_SINGLE);
	      list_add(curr_lig->oivs, gp(i));
	    }
	}
      else
	{
	  switch (gp(i)->f)
	    {
	    case OFPF_CVNN:
	      if (!curr_sp->cvnns)
		curr_sp->cvnns = list_create(LIST_SINGLE);
	      list_add(curr_sp->cvnns, gp(i));
	      break;
	    case OFPF_SALT:
	      if (!curr_sp->salts)
		curr_sp->salts = list_create(LIST_SINGLE);
	      list_add(curr_sp->salts, gp(i));
	      break;
	    case OFPF_SSET:
	      if (!curr_sp->ssets)
		curr_sp->ssets = list_create(LIST_SINGLE);
	      list_add(curr_sp->ssets, gp(i));
	      break;
	    default:
	      /* nowt */
	      break;
	    }
	  if (gp(i)->ivs)
	    {
	      if (!curr_sp->oivs)
		curr_sp->oivs = list_create(LIST_SINGLE);
	      list_add(curr_sp->oivs, gp(i));
	    }
	}
    }
#undef gp
}
