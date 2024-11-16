#include <pool.h>
#include "asl.h"

Pool *aslpool;
static struct sl_config *aslconfigp;

struct sl_config *
asl_get_config(void)
{
  return aslconfigp;
}

void
asl_init(struct sl_config *cp)
{
  aslpool = pool_init();
  aslconfigp = cp;
}

void
asl_term(void)
{
  pool_term(aslpool);
}
