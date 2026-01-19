#include <oraccsys.h>
#include "cx.h"
#include "keydata.h"

void
cx_keydata(Cx *c)
{
  Keydata *kp = NULL;
  char *kd = keydata_find(c->project);
  if (kd)
    {
      kp = keydata_init(c, kd, c->rr[0]->fields);
      if (!keydata_validate(kp))
	{
	  keydata_load(kp);
	  c->k = kp;
	}
    }
}
