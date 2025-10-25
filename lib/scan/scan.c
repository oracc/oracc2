#include <pool.h>
#include "scan.h"

Pool *scan_pool;

void
scan_init(void)
{
  if (!scan_pool)
    scan_pool = pool_init();
}

void
scan_term(void)
{
  if (scan_pool)
    {
      pool_term(scan_pool);
      scan_pool = NULL;
    }
}

