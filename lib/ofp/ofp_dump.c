#include <oraccsys.h>
#include "ofp.h"

void
ofp_dump(Ofp *o, FILE *fp)
{
#define nonull(s) (s ? (ccp)s : "")
#define gp(i) (&o->glyphs[i])
  int i;
  for (i = 0; i < o->nglyphs; ++i)
    {
      if ('-' != *gp(i)->key)
	fprintf(fp,
		"%s\t%s\t%s\t"
		"%s\t%s\t%s\t"
		"%s\t%s\t%s\t"
		"%s\t%s\t%s\n",
		gp(i)->key, gp(i)->name, nonull(gp(i)->code), 
		ofp_feat_str[gp(i)->f], nonull(gp(i)->f_chr), gp(i)->f_chr ? itoa(gp(i)->f_int) : "",
		nonull(gp(i)->ligl), nonull(gp(i)->liga), nonull(gp(i)->ivs),
		gp(i)->osl ? nonull(gp(i)->osl->u) : "", 
		gp(i)->osl ? nonull(gp(i)->osl->o) : "", 
		gp(i)->osl ? nonull(gp(i)->osl->n) : ""
		);
    }
#undef gp
}
