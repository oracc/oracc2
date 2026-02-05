#include <oraccsys.h>
#include <roco.h>
#include "kis.h"

Kis *
kis_load(const char *kis_fn)
{
  Kis *kis = roco_load1(kis_fn);
  kis->hash_key_col = 1;
  roco_hash_r(kis);
  return kis;
}
