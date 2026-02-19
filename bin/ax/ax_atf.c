#include <oraccsys.h>
#include <atf.h>
#include "ax.h"

static const char *atf_flag_str[] = { "mylines" , "alignment-groups", "math",
				      "unicode", "legacy", "lexical", NULL };
static void ax_atf_link(Xlink *lp);

void
ax_atf(ATF*a)
{
  fprintf(stderr, "&%s = %s\n", a->pqx, a->name);
  if (a->edoc == EDOC_COMPOSITE)
    fprintf(stderr, "@composite\n");
  else if (a->edoc == EDOC_SCORE)
    fprintf(stderr, "@score %s %s%s\n",
	    a->stype == EDOC_MATRIX ? "matrix" : "synoptic",
	    a->sparse == EDOC_PARSED ? "parsed" : "unparsed",
	    a->sword ? " word" : "");
  if (a->project)
    fprintf(stderr, "#project: %s\n", a->project);
  if (a->flags)
    {
      int i, j;
      for (i = 1, j = 0; i < ATFF_TOP; i<<=1, ++j)
	if (a->flags&i)
	  fprintf(stderr, "#atf: use %s\n", atf_flag_str[j]);
    }
  if (a->lang)
    {
      if (a->altlang)
	fprintf(stderr, "#atf: lang %s _%s_\n", a->lang, a->altlang);
      else
	fprintf(stderr, "#atf: lang %s\n", a->lang);
    }
  if (a->nlinks)
    {
      int i;
      for (i = 0; i < a->nlinks; ++i)
	ax_atf_link(a->links[i]);
    }
}

void
ax_atf_link(Xlink *lp)
{
  if (lp->t == ELINK_DEF)
    fprintf(stderr, "#link: def %s = %s = %s\n", lp->siglum, lp->qid, lp->name);
  else
    fprintf(stderr, "#link: %s %s = %s\n", lp->t==ELINK_SOURCE ? "source" : "parallel",
	    lp->qid, lp->name);    
}
