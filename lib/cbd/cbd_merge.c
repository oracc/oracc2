#include <oraccsys.h>
#include "cbd.h"

/* cbd_merge.c: merge two CBD structures
 *
 * This is used to produce a union of the glossary data and the corpus
 * lemmatization data.
 *
 */

static void
cbd_m_entry(Entry *tep, Entry *fep)
{
}

void
cbd_merge(Cbd *ctp, Cbd *cfp)
{
  Entry *fe;
  for (fe = list_first(cfp->entries); fe; fe = list_next(cfp->entries))
    {
      Entry *te = hash_find(ctp->hentries, fe->cgp->tight);
      if (te)
	cbd_m_entry(te, fe);
      else
	{
	  list_add(ctp->entries, fe);
	  hash_add(ctp->hentries, fe->cgp->tight, fe);
	}
    }
}
