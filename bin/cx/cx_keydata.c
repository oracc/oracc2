#include <oraccsys.h>
#include "cx.h"

Keydata *
cx_keydata(const char *prj)
{
  Keydata *kp = NULL;
  char *kd = keydata_find(prj);
  if (kd)
    {
      kp = keydata_init(kd);
      if (!keydata_validate(kp))
	keydata_load(kp);
    }
  return kp;  
}
