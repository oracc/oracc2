#include <oraccsys.h>
#include <gdl.h>
#include "inl.h"

void
inl_gdl_h(Tree *tp, Scan *sp, char *s)
{
  sp->np = tree_add(tp, NS_GDL, "gdl", tp->curr->depth, NULL);

  char *g = s;
  if (sp->attr && '%' == *sp->attr)
    {
      char buf[strlen(s)+strlen(sp->attr) + 2];
      sprintf(buf, "%s %s", sp->attr, s);
      g = buf;
    }
    
  Tree *gtp = gdlparse_string(sp->np->mloc, g);
  tree_graft(sp->np, gtp);
  
  gdlparse_reset();  
}
