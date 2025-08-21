#include <oraccsys.h>
#include "ofp.h"

static Hash *lindex;
static enum Ofp_feature feature_code(const char *ext);
static int feature_int(const char *n);

int
glyphnamep(const void *p1, const void *p2)
{
  return strcmp((*(Ofp_glyph**)p1)->name, (*(Ofp_glyph**)p2)->name);
}

/* Return the period at the start of a feature, ignoring any .liga */
static const char *
feature_check(const char *n)
{
  const char *l = strstr(n, ".liga");
  return strchr(l ? l+1 : n, '.');
}

static enum Ofp_feature
feature_code(const char *ext)
{
  if (isdigit(*ext))
    {
      do
	++ext;
      while (isdigit(*ext));
      if (!*ext)
	return OFPF_SALT;
    }

  if ('c' == ext[0] && 'v' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFPF_CVNN;

  if ('s' == ext[0] && 's' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFPF_SSET;

  return OFPF_NONE;
}

static int
feature_int(const char *n)
{
  while (*n && !isdigit(*n))
    ++n;
  if (n)
    return atoi(n);
  else
    return 0;
}

static void
feature_set(struct Ofp_sign *sp, Ofp_glyph *gp, const char *f)
{
  gp->f = feature_code(f);
  gp->f_chr = f;
  gp->f_int = feature_int(f);

  switch (gp->f)
    {
    case OFPF_CVNN:
      if (!sp->cvnns)
	sp->cvnns = list_create(LIST_SINGLE);
      list_add(sp->cvnns, gp);
      break;
    case OFPF_SALT:
      if (!sp->salts)
	sp->salts = list_create(LIST_SINGLE);
      list_add(sp->salts, gp);
      break;
    case OFPF_SSET:
      if (!sp->ssets)
	sp->ssets = list_create(LIST_SINGLE);
      list_add(sp->ssets, gp);
      break;
    default:
      /* nowt */
      break;
    }
#if 0
  if (gp->ivs)
    {
      if (!sp->oivs)
	sp->oivs = list_create(LIST_SINGLE);
      list_add(sp->oivs, gp);
    }
#endif
}

static void
liga_add(Ofp *ofp, List *lp, Ofp_glyph *gp, const char *f)
{
  Ofp_liga *ligp = memo_new(ofp->m_liga);
  hash_add(lindex, (uccp)gp->name, ligp);
  feature_set(ligp, gp, f);
}

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
  Ofp_glyph **v = (Ofp_glyph**)hash_vals2(ofp->h_liga, &nk);
  qsort(v, nk, sizeof(Ofp_glyph*), glyphnamep);
  if (ofp->trace)
    {
      fprintf(ofp->trace, "===\nofp_marshall begin sorted glyphs:\n");
      for (i = 0; i < nk; ++i)
	fprintf(ofp->trace, "%s\n", v[i]->name);
      fprintf(ofp->trace, "ofp_marshall end sorted glyphs\n===\n");
    }
  for (i = 0; i < nk; ++i)
    {
      const char *f = feature_check(v[i]->name);
      if (f)
	{
	  char *p = (char *)pool_copy((uccp)v[i]->name, ofp->p);
	  p[f++ - v[i]->name] = '\0';
	  Ofp_sign *sp = hash_find(ofp->h_sign, (uccp)p);
	  if (!sp)
	    fprintf(stderr, "%s:%d: %s has undefined parent %s\n",
		    ofp->file, v[i]->index, v[i]->name, p);
	  else
	    feature_set(sp, v[i], f);
	}
      else
	{
	  Ofp_sign *sp = memo_new(ofp->m_sign);
	  hash_add(ofp->h_sign, (uccp)v[i]->name, sp);
	}
    }

  v = (Ofp_glyph**)hash_vals2(ofp->h_liga, &nk);
  qsort(v, nk, sizeof(Ofp_glyph*), glyphnamep);
  if (ofp->trace)
    {
      fprintf(ofp->trace, "===\nofp_marshall begin sorted liga:\n");
      for (i = 0; i < nk; ++i)
	fprintf(ofp->trace, "%s\n", v[i]->name);
      fprintf(ofp->trace, "ofp_marshall end sorted liga\n===\n");
    }

  lindex = hash_create(1024);
  for (i = 0; i < nk; ++i)
    {
      const char *f = feature_check(v[i]->name);
      if (f)
	{
	  char *p = (char *)pool_copy((uccp)v[i]->name, ofp->p);
	  p[f++ - v[i]->name] = '\0';
	  Ofp_liga *ligp = hash_find(lindex, (uccp)p);
	  if (!ligp)
	    fprintf(stderr, "%s:%d: %s has undefined parent %s\n",
		    ofp->file, v[i]->index, v[i]->name, p);
	  else
	    {
	      if (!ligp->ligas)
		ligp->ligas = list_create(LIST_SINGLE);
	      liga_add(ofp, ligp->ligas, v[i], f);
	    }
	}
      else
	{
	  Ofp_sign *sp = hash_find(ofp->h_sign, (uccp)v[i]->ligl);
	  if (!sp)
	    fprintf(stderr, "%s:%d: %s has undefined head %s\n",
		    ofp->file, v[i]->index, v[i]->name, v[i]->ligl);
	  else
	    {
	      if (!sp->ligas)
		sp->ligas = list_create(LIST_SINGLE);
	      liga_add(ofp, sp->ligas, v[i], NULL);
	    }
	}
    }
  hash_free(lindex, NULL);
}

#if 0
/* Ligatures attach to their head sign, and ligatures with features
 * attach to their parent, where parent is defined as the name with
 * the final feature stripped off. It is an error for Oracc fonts to
 * have orphaned feature combinations, i.e., for x.y.liga.ss01.1 to
 * exist if x.y.liga.ss01 does not exist.
 */
static void
ofp_marshall_liga(Ofp *ofp, Ofp_glyph *gp)
{
  if (parent)
    {
      Ofp_liga *ligp = hash_find(lindex, (uccp)parent);
      if (!ligp)
	fprintf(stderr, "%s:%d: %s has undefined parent %s\n",
		ofp->file, gp->index, gp->name, parent);
      else
	{
	  if (!ligp->ligas)
	    ligp->ligas = list_create(LIST_SINGLE);
	  liga_add(ofp, ligp->ligas, gp, feature);
	}
    }
  else
    {
      Ofp_sign *sp = hash_find(ofp->h_sign, (uccp)gp->ligl);
      if (!sp)
	fprintf(stderr, "%s:%d: %s has undefined head %s\n",
		ofp->file, gp->index, gp->name, gp->ligl);
      else
	{
	  if (!sp->ligas)
	    sp->ligas = list_create(LIST_SINGLE);
	  liga_add(ofp, sp->ligas, gp, NULL);
	}
    }
}

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

#endif
