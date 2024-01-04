#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runexpat.h"

void
firstC(const char *xid, const char *name, const char **atts, FILE *fp)
{
  fprintf(stderr, "firstC: %s\n", xid);
}

int
main(int argc, char **argv)
{
  xmlfilter_actions(NULL, firstC, NULL);
  xmlfilter(NULL, stdout);
}
