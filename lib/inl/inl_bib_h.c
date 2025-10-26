#include <tree.h>
#include "inl.h"

void
inl_bib_h(Tree *tp, Scan *sp, char *s)
{
  /*bib_check_keys(s);*/
  sp->np = tree_add(tp, NS_BIB, "cite", tp->curr->depth, NULL);
  sp->np->text = s;
}
