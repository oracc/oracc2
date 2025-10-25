#include <pool.h>
#include "scan.h"

Pool *scan_pool;

void
scan_init(void)
{
  if (!scan_pool)
    scan_pool = pool_init();
}
