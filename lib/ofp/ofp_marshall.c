#include <oraccsys.h>
#include "ofp.h"

int
liganamecmp(const void *p1, const void *p2)
{
  return strcmp((*(Ofp_glyph**)p1)->name, (*(Ofp_glyph**)p2)->name);
}

#if 0
static void ofp_marshall_liga(Ofp *ofp, Ofp_sign *sp, Ofp_glyph *gp);
#endif

/* Collect all the glyphs that belong to each sign
 *
 * non-ligature glyphs are in h_glyf; liga in h_liga
 *
 * add all the h_glyf to h_sign first, then add h_liga to the head
 * signs
 *
 * sort the liga by name so they naturally ramp up onto the lists
 *
 */
void
ofp_marshall(Ofp *ofp)
{
  int i;
  int nk;
  const char **k = hash_keys2(ofp->h_glyf, &nk);
  for (i = 0; i < nk; ++i)
    {
      Ofp_sign *sp = memo_new(ofp->m_sign);
      sp->glyph = hash_find(ofp->h_glyf, (uccp)k[i]);
      hash_add(ofp->h_sign, (uccp)k[i], sp);
    }
  
  Ofp_glyph **v = (Ofp_glyph**)hash_vals2(ofp->h_liga, &nk);
  qsort(v, nk, sizeof(Ofp_glyph*), liganamecmp);
  if (ofp->trace)
    {
      fprintf(ofp->trace, "===\nofp_marshall begin sorted liga:\n");
      for (i = 0; i < nk; ++i)
	fprintf(ofp->trace, "%s\n", v[i]->name);
      fprintf(ofp->trace, "ofp_marshall end sorted liga\n===\n");
    }
#if 0
  List *liga_feat = list_create(LIST_SINGLE);
  for (i = 0; i < nliga; ++i)
    {
      Ofp_sign *sp = hash_find(ofp->h_sign, k[i]);
      Ofp_glyph *gp = hash_find(ofp->h_liga, k[i]);
      if (!sp)
	fprintf("%s:%d: undefined ligature head %s\n", ofp->file, gp->index, k);
      else
	ofp_marshall_liga(ofp, sp, gp);
    }
#endif
}

#if 0
static void
ofp_marshall_liga(Ofp *ofp, Ofp_sign *sp, Ofp_glyph *gp)
{
}

static void
ofp_marshall_liga_feat(Ofp *ofp, Ofp_glyph *gp)
{
}
#endif

void
xofp_marshall(Ofp *ofp)
{
  Ofp_liga *curr_lig = NULL;
#define gp(x) (&ofp->glyphs[x])
  int i;
  struct Ofp_sign *curr_sp = memo_new(ofp->m_sign);
  curr_sp->glyph = gp(0);
  hash_add(ofp->h_sign, (uccp)gp(0)->key, curr_sp);
  for (i = 0; i < ofp->nglyphs; ++i)
    {
      /* liga -> comp binding does not set glyph */
      if (gp(i)->key == NULL)
	continue;
      
      /* ligas that are signs use liga as key so they change curr_sp;
	 ligas that are not signs use ligl as key so they get added
	 under curr_sp */
      if (strcmp(curr_sp->glyph->key, gp(i)->key))
	{
#if 1
	      curr_sp = memo_new(ofp->m_sign);
	      curr_sp->glyph = gp(i);
	      hash_add(ofp->h_sign, (uccp)gp(i)->key, curr_sp);
#else
	  /* .liga come first in glyphs and .liga.1 come last--this
	     allows .liga.1 to find its parent despite not being
	     sorted */
	  if (!(curr_sp = hash_find(ofp->h_sign, (uccp)gp(i)->key)))
	    {
	      curr_sp = memo_new(ofp->m_sign);
	      curr_sp->glyph = gp(i);
	      hash_add(ofp->h_sign, (uccp)gp(i)->key, curr_sp);
	    }
#endif
	}
      if (gp(i)->ligl && strlen(gp(i)->key) < strlen(gp(i)->liga))
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
