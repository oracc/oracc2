#include <oraccsys.h>

#include "ns.h"

Pool *nspool;

void
nx_init(void)
{
  nspool = pool_init();
  mesg_init();
}

int
main(int argc, char **argv)
{
  nsflextrace = 0;
  nx_init();
  nsparse();
  mesg_print(stderr);
}
