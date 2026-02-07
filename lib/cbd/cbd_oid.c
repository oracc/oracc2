#include <oraccsys.h>
#include <oid.h>
#include "cbd.h"

static Hash *oids;
static int zid = 0;

void
cbd_oid_init(const char *lang)
{
  oids = oid_load(NULL, "oid", lang);
  if (!oids)
    oids = hash_create(1);  
}

const char *
cbd_zid(void)
{
  static char buf[8];
  sprintf(buf, "z%06d", ++zid);
  return (ccp)pool_copy((uccp)buf, csetp->pool);
}

void
cbd_oid(int e_or_s, unsigned const char *k, void *v)
{
  const char *oid = hash_find(oids, (uccp)k);
  if (!oid)
    oid = cbd_zid();
  if (e_or_s == 'e')
    ((Entry*)v)->eid = oid;
  else
    ((Sense*)v)->sid = oid;
}
