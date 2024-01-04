#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runexpat.h"

const char **
tis_attrs(const char *xid, const char *name, const char **atts, FILE *fp)
{
  const char **natts = NULL;
  tisinfo = hash_find(tish, xid);
  if (tisinfo)
    {
      int i;
      for (i = 0; atts[i]; ++i)
	;
      natts = malloc((i+7)*sizeof(char *));
      memcpy(natts, atts, i*sizeof(char*));
      natts[i++] = "is:cnt";
      natts[i++] = tisinfo->count;
      natts[i++] = "is:pct";
      natts[i++] = tisinfo->pct;
      natts[i++] = "is:ref";
      natts[i++] = tisinfo->ref;
      natts[i] = NULL;
      return natts;
    }
  else
    return atts;
}

void
tis_island(const char *xid, const char *name, const char **atts, FILE *fp)
{
  fprintf(stderr, "firstC: %s\n", xid);
}

int
main(int argc, char **argv)
{
  xmlfilter_actions(tis_attrs, NULL, tis_island);
  xmlfilter(NULL, stdout);
}
