#include <oraccsys.h>
#include "nx.h"

/* This is a simple driver to read the step-diagram data */

extern int nsrestart(FILE *);

void
ns_data(void)
{
  mesg_init();
  FILE *fp = fopen("ns.dat", "r");
  currnsfile = "ns.dat";
  nsrestart(fp);
  nsparse();
  if (mesg_status())
    {
      mesg_print(stderr);
      exit(1);
    }
}
