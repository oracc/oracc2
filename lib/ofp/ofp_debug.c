#include <oraccsys.h>
#include "ofp.h"

void
ofp_debug(Ofp *o, const char *fname)
{
  FILE *fp = fopen(fname, "w");
  if (fp)
    {
      ofp_dump(o, fp);
      fclose(fp);
    }
  else
    fprintf(stderr, "ofp_debug: open failed on %s: %s\n", fname, strerror(errno));
}
