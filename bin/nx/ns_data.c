#include <oraccsys.h>
#include "nx.h"

const char *file;
Mloc *xo_loc;

/* This is a simple driver to read the step-diagram data */
extern const char *arg_dat; /* .dat file given with -d on command line */
extern int nsrestart(FILE *);
extern int nslex_destroy(void);

void
ns_data(void)
{
  mesg_init();
  char dat[strlen(oracc())+strlen("/lib/data/ns.dat0")];

  const char *use_dat = arg_dat ? arg_dat : "ns.dat";  

  file = use_dat;
  
  if (!access(use_dat, R_OK))
    strcpy(dat, use_dat);
  else if (arg_dat)
    {
      fprintf(stderr, "nx: data file given with -d %s could not be opened. Stop.\n", arg_dat);
      exit(1);
    }
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
