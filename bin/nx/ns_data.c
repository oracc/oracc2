#include <oraccsys.h>
#include "nx.h"

/* This is a simple driver to read the step-diagram data */

extern int nsrestart(FILE *);
extern int nslex_destroy(void);

void
ns_data(void)
{
  mesg_init();
  FILE *fp = fopen("ns.dat", "r");
  currnsfile = "ns.dat";
  nsrestart(fp);
  nsparse();
  (void)nslex_destroy();
  fclose(fp);
  if (mesg_status())
    {
      mesg_print(stderr);
      exit(1);
    }
}
