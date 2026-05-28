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
cbd_cmp_need_plus_e(List *lp, const char *fn)
{
  const char *cgp;
  for (cgp = list_first(lp); cgp; cgp = list_next(lp))
    fprintf(stderr, "+%s\n", cgp);
}

static void
cbd_cmp_need_minus_e(List *lp, const char *fn)
{
  const char *cgp;
  for (cgp = list_first(lp); cgp; cgp = list_next(lp))
    fprintf(stderr, "-%s\n", cgp);
}

static void
cbd_cmp_need_s(List *lp, char need, const char *fn)
{
  const char *cgp;
  for (cgp = list_first(lp); cgp; cgp = list_next(lp))
    fprintf(stderr, "%c%s\n", need, cgp);
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
      cbd_cmp_need_s(l_only_a, '+', ea->owner->file);
      ++ret;
    }

  if (list_len(l_only_b))
    {
      cbd_cmp_need_s(l_only_b, '-', eb->owner->file);
      ++ret;
    }  

  return ret;
}

static int
cbd_cmp_entry(Entry *tep, Entry *fep)
{
  return cbd_cmp_senses(tep, fep);
}

/* This routine compares @entry/@sense to inform how the new glossary,
   cpa, should be marked up with +/- to suppress all differences with
   the old glossary, cpb */
int
cbd_cmp(Cbd *cpa, Cbd *cpb)
{
  int ret = 0;

  List *l_only_a = list_create(LIST_SINGLE);
  Hash *h_seen_b = hash_create(128);
  
  Entry *ea;
  for (ea = list_first(cpa->entries); ea; ea = list_next(cpa->entries))
    {
      Entry *eb = hash_find(cpb->hentries, ea->cgp->tight);
      if (eb)
	{
	  ret += cbd_cmp_entry(ea, eb);
	  hash_add(h_seen_b, eb->cgp->tight, eb);
	}
      else
	{
	  if (ea->ed)
	    {
	      switch (ea->ed->type)
		{
		case ADD_E:
		  break;
		default:
		  list_add(l_only_a, (void*)ea->cgp->tight);
		  break;
		}
	    }
	  else
	    list_add(l_only_a, (void*)ea->cgp->tight);
	}
    }
  
  List *l_only_b = cmp_only_b(cpb, h_seen_b);
  if (list_len(l_only_a))
    {
      cbd_cmp_need_plus_e(l_only_a, cpa->file);
      ++ret;
    }
  if (list_len(l_only_b))
    {
      cbd_cmp_need_minus_e(l_only_b, cpa->file);
      ++ret;
    }

  return ret;
}
