#include <stdlib.h>
#include <string.h>
#include <tree.h>
#include "asl.h"
#include "signlist.h"
#include "asl.tab.h"

extern int asllex_destroy(void);

struct sl_signlist *
aslyacc(const char *file)
{
  if (file)
    curraslfile = aslfile = file;
  else
    curraslfile = aslfile = "<stdin>";
  curr_asl = NULL; /* Must do this to ensure that successive
		      aslparse() do not read into the same struct
		      sl_signlist */
  aslparse();
  asllex_destroy();
  /*asl_bld_term(sl);*/ /* This needs to be called after the signlist is done with */
  return curr_asl;
}
