#include <tree.h>
#include "inl.h"

void
inl_bib_h(Scan *sp, Scanseg *ssp, char *s)
{
  /*bib_check_keys(s);*/
  ssp->np = tree_add(sp->tree, NS_BIB, ssp->name, sp->tree->curr->depth, NULL);
  ssp->np->text = s;
}
