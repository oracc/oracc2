#include <oraccsys.h>
#include "nx.h"
extern int nsrestart(FILE *);
void
nx_data(void)
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
