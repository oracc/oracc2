#include <oraccsys.h>
#include <gdl.h>
#include "inl.h"

void
inl_gdl_h(Tree *tp, Scan *sp, char *s)
{
  sp->np = tree_add(tp, NS_GDL, "gdl", tp->curr->depth, NULL);
  Tree *gtp = gdlparse_string(sp->np->mloc, s);
  tree_graft(sp->np, gtp);
  gdlparse_reset();  
}
