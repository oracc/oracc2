#include <oraccsys.h>
#include <gdl.h>
#include "inl.h"

void
inl_gdl_h(Scan *sp, Scanseg *ssp, char *s)
{
  ssp->np = tree_add(sp->tree, NS_GDL, "gdl", sp->tree->curr->depth, NULL);

  char *g = s;
  if (ssp->attr && '%' == *ssp->attr)
    {
      char buf[strlen(s)+strlen(ssp->attr) + 2];
      sprintf(buf, "%s %s", ssp->attr, s);
      g = buf;
    }
    
  Tree *gtp = gdlparse_string(&sp->mp, g);
  tree_graft(ssp->np, gtp);
  
  gdlparse_reset();  
}
