#include <oraccsys.h>
#include "cbd.h"

/* cbd_cmp: compare two CBD structures
 *
 */

List *l_only_a = NULL;
Hash *h_seen_b = NULL;

static void
cbd_cmp_senses(Entry *tep, Entry *fep)
{
  /*Hash *hf = hash_create(128);*/  
}

static void
cbd_cmp_entry(Entry *tep, Entry *fep)
{
  cbd_cmp_senses(tep, fep);
}

int
cbd_cmp(Cbd *cpa, Cbd *cpb)
{
  l_only_a = list_create(LIST_SINGLE);
  h_seen_b = hash_create(128);
  
  Entry *fe;
  for (fe = list_first(cpa->entries); fe; fe = list_next(cpa->entries))
    {
      Entry *te = hash_find(cpb->hentries, fe->cgp->tight);
      if (te)
	{
	  cbd_cmp_entry(te, fe);
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
		  list_add(l_only_a, fe);
		  break;
		}
	    }
	  else
	    list_add(l_only_a, fe);
	}
    }
  
  return 0;
}
