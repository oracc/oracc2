#include <oraccsys.h>
#include "nx.h"

/* This is a simple driver to read the step-diagram data */

extern int nsrestart(FILE *);
extern int nslex_destroy(void);

void
ns_data(void)
{
  mesg_init();
  char dat[strlen(oracc())+strlen("/lib/data/ns.dat0")];
  if (!access("ns.dat", R_OK))
    strcpy(dat, "ns.dat");
  else
    sprintf(dat, "%s/lib/data/ns.dat", oracc());
  FILE *fp = fopen(dat, "r");
  currnsfile = dat;
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
