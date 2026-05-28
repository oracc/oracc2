#include <oraccsys.h>
#include "cbd.h"

/* cbd_cmp: compare two CBD structures
 *
 */

static List *
cmp_only_b(Cbd *cpb, Hash *hseen)
{
  List *lb = list_create(LIST_SINGLE);
  const char **kk = hash_keys(cpb->hentries);
  int i;
  for (i = 0; kk[i]; ++i)
    if (!hash_find(hseen, (uccp)kk[i]))
      list_add(lb, (void*)kk[i]);
  return lb;
}

static List *
cmp_s_only_b(List *ls, Hash *hseen)
{
  List *lb = list_create(LIST_SINGLE);
  Sense *sp;
  for (sp = list_first(ls); sp; sp = list_next(ls))
    if (!hash_find(hseen, sp->cgspe))
      list_add(lb, (void*)sp->cgspe);
  return lb;
}

static void
cbd_cmp_list_e(List *lp, const char *fn)
{
  const char *cgp;
  for (cgp = list_first(lp); cgp; cgp = list_next(lp))
    fprintf(stderr, "entry %s only in glossary %s\n", cgp, fn);
}

static void
cbd_cmp_list_s(List *lp, const char *fn)
{
  const char *cgp;
  for (cgp = list_first(lp); cgp; cgp = list_next(lp))
    fprintf(stderr, "sense %s only in glossary %s\n", cgp, fn);
}

static int
cbd_cmp_senses(Entry *ea, Entry *eb)
{
  int ret = 0;
  Hash *hb = hash_create(10);
  List *l_only_a = list_create(LIST_SINGLE);
  Hash *h_seen_s = hash_create(10);
  Sense *sbp;
  for (sbp = list_first(eb->senses); sbp; sbp = list_next(eb->senses))
    hash_add(hb, sbp->cgspe, sbp);
  Sense *sap;
  for (sap = list_first(ea->senses); sap; sap = list_next(ea->senses))
    {
      Sense *sb = hash_find(hb, sap->cgspe);
      if (sb)
	{
	  hash_add(h_seen_s, sb->cgspe, sb);
	}
      else
	{
	  if (sap->ed)
	    {
	      switch (sap->ed->type)
		{
		case ADD_S:
		  break;
		default:
		  list_add(l_only_a, (void*)sap->cgspe);
		  break;
		}
	    }
	  else
	    list_add(l_only_a, (void*)sap->cgspe);
	}
    }
  hash_free(hb, NULL);

  List *l_only_b = cmp_s_only_b(eb->senses, h_seen_s);
  if (list_len(l_only_a))
    {
      cbd_cmp_list_s(l_only_a, ea->owner->file);
      ++ret;
    }

  if (list_len(l_only_b))
    {
      cbd_cmp_list_s(l_only_b, eb->owner->file);
      ++ret;
    }  

  return ret;
}

static int
cbd_cmp_entry(Entry *tep, Entry *fep)
{
  return cbd_cmp_senses(tep, fep);
}

int
cbd_cmp(Cbd *cpa, Cbd *cpb)
{
  int ret = 0;

  List *l_only_a = list_create(LIST_SINGLE);
  Hash *h_seen_b = hash_create(128);
  
  Entry *fe;
  for (fe = list_first(cpa->entries); fe; fe = list_next(cpa->entries))
    {
      Entry *ep = hash_find(cpb->hentries, fe->cgp->tight);
      if (ep)
	{
	  ret += cbd_cmp_entry(ep, fe);
	  hash_add(h_seen_b, fe->cgp->tight, fe);
	}
      else
	{
	  if (fe->ed)
	    {
	      switch (fe->ed->type)
		{
		case ADD_E:
		  break;
		default:
		  list_add(l_only_a, (void*)fe->cgp->tight);
		  break;
		}
	    }
	  else
	    list_add(l_only_a, (void*)fe->cgp->tight);
	}
    }
  
  List *l_only_b = cmp_only_b(cpb, h_seen_b);
  if (list_len(l_only_a))
    {
      cbd_cmp_list_e(l_only_a, cpa->file);
      ++ret;
    }
  if (list_len(l_only_b))
    {
      cbd_cmp_list_e(l_only_b, cpb->file);
      ++ret;
    }

  return ret;
}
