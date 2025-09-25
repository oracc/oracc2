#include <oraccsys.h>
#include "cfy.h"

void
cfy_lgs(Cfy *c)
{
  if (c->infile)
    {
      char buf[strlen(c->infile)+1];
      strcpy(buf, c->infile);
      strcpy(buf+strlen(buf)-3, "lgs");
      if (!access(buf, R_OK))
	{
	  if (verbose)
	    fprintf(stderr, "cfy_lgs: using %s\n", buf);
	  size_t nl;
	  uchar **l = loadfile_lines((uccp)buf, &nl);
	  int i;
	  c->hlgs = hash_create(1024);
	  for (i = 0; i < nl; ++i)
	    {
	      char **w = space_split((char*)l[i]);
	      /* initial implementation only maps first MTS to LGS; if
		 this ever gets fixed it will be in the LGS table so
		 we will still only map 1:1 here */
	      if (w[0] && w[1])
		hash_add(c->hlgs, (uccp)w[0], w[1]);
	      free(w);
	    }
	}
    }
}
