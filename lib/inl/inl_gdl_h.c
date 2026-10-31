#include <oraccsys.h>
#include <gdllib.h>
#include <gdlparse.h>
#include "inl.h"

void
inl_gdl_h(Scan *sp, Scanseg *ssp, Tree *tp, char *s)
{
  ssp->np = tree_add(sp->tree, NS_GDL, "gdl", sp->tree->curr->depth, NULL);

  char buf[strlen(s)+(ssp->attr ? strlen(ssp->attr) : 0) + 2];
  if (ssp->attr && '%' == *ssp->attr)
    sprintf(buf, "%s %s", ssp->attr, s);
  else
    strcpy(buf, s);
    
  gdlparse_string(&sp->mp, buf);
  tree_graft(ssp->np, tcurr);
  
  gdlparse_reset();  
}
