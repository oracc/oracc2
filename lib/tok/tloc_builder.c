#include <oraccsys.h>
#include <tok.h>

/* tloc_builder can work in single-unit mode or in multi-unit mode.
   
   In single-unit mode a single set of structures is continuously
   updated throughout the run.

   In multi-unit mode a each text/line/word has its own structures.
 */

Loch *
tlb_init(int many)
{
  Loch *l = malloc(sizeof(Loch));
  
  return l;
}

void
tlb_term(void)
{
}

