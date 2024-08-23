#include <oraccsys.h>

#include "nx.h"

Pool *nspool;

/* The data manager object for the run; probably need to reset this
   after each text to handle large runs */
nx *nxp;

void
nx_init(void)
{
  nxp = calloc(1, sizeof(nx));
  nspool = nxp->p = pool_init();
  nxp->m_sys = memo_init(sizeof(ns_sys), 10);
  nxp->m_step = memo_init(sizeof(ns_step), 100);
  nxp->m_nx_step = memo_init(sizeof(nx_step), 100);
  nxp->m_inst = memo_init(sizeof(ns_inst), 1000);
  nxp->m_nx_number = memo_init(sizeof(nx_number), 100);
  nxp->ir = hash_create(1024);
}

const char *test[] = { "1(u)" , "ba" , "2(u)" , "1(aš)" , "bu" , NULL };

int
main(int argc, char **argv)
{
  ns_flex_debug = nsflextrace = 0;
  nx_init();

  mesg_init();
  nsparse();
  mesg_print(stderr);

  mesg_init();
  /*nx_result *r = */ (void)nx_parse((const uchar **)test, NULL, -1);
  mesg_print(stderr);
}
