#include <oraccsys.h>
#include "nx.h"
extern int nsrestart(FILE *);
void
nx_data(void)
{
  mesg_init();
  FILE *fp = fopen("ns.dat", "r");
  nsrestart(fp);
  nsparse();
  mesg_print(stderr);
}
