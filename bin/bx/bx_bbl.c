#include <oraccsys.h>
#include "bx.h"
#include "bbl.h"
#include "bbl.tab.h"

void
bbl_load(const char *bblfile)
{
  extern const char *curr_bbl;
  extern FILE *bblin;
  curr_bbl = bblfile;
  FILE *fp = fopen(bblfile, "r");
  if (!fp)
    {
      fprintf(stderr, "can't read .bbl %s\n", bblfile);
      exit(1);
    }
  bblin = fp;
  bblparse();
  fclose(fp);
}
