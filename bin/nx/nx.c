#include <oraccsys.h>

#include "ns.h"

Pool *nspool;

/* The data manager object for the run; probably need to reset this
   after each text to handle large runs */
nx *nxp;

void
nx_init(void)
{
  nxp = calloc(1, sizeof(nx));
  nspool = nxp->p = pool_init();
  nxp->m_sys = memo_init(sizeof(nx_sys), 10);
  nxp->m_step = memo_init(sizeof(nx_step), 100);
  nxp->m_inst = memo_init(sizeof(nx_inst), 1000);
  nxp->ir = hash_create(1024);
  mesg_init();
}

int
main(int argc, char **argv)
{
  ns_flex_debug = nsflextrace = 0;
  nx_init();
  nsparse();
  mesg_print(stderr);
}
