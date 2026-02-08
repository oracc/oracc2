#include <oraccsys.h>
#include <oid.h>
#include "cbd.h"

static Hash *oids;
static Roco *roids;
static int zid = 0;

static unsigned const char *
oid_roco_key(const char **row)
{
  char buf[strlen(row[1])+strlen(row[2])+2];
  sprintf(buf, "%s:%s", row[1], row[2]);
  return pool_copy((uccp)buf, csetp->pool);
}

static Roco *
oid_roco(const char *otab)
{
  if (!xaccess(otab, R_OK, 0))
    {
      Roco *r = roco_load1(otab);
      oids = roco_hash_r_f(r, oid_roco_key);
      return r;
    }
  else
    return NULL;
}

void
cbd_oid_init(void)
{
  roids = oid_roco(oid_tab());
  if (!roids)
    oids = hash_create(1);
}

static const char *
cbd_zid(void)
{
  static char buf[8];
  sprintf(buf, "z%06d", ++zid);
  return (ccp)pool_copy((uccp)buf, csetp->pool);
}

static const char *
cbd_oid(unsigned const char *k, const char *l)
{
  if (l)
    {
      char key[6+strlen((ccp)k)];
      strncpy(key, l, 3);
      key[3] = ':';
      strcpy(key+4, (ccp)k);
      const char **r = hash_find(oids, (uccp)key);
      if (r)
	return r[0];
    }
  return NULL;
}

/* There is an issue with QPN OIDs because they don't actually exist.
 *
 * What exists is one or more OIDs with keys that may be identical
 * except for their language.  So, in principle, one could have
 * %sux:Sargon[1]RN and %akk:Sargon[1]RN.
 *
 * This needs to be fixed in OID validation: we need to enforce a rule
 * whereby QPNs must be unique across domains, unlike other OID keys.
 *
 * For now we just go through an entry's forms and find the first
 * matching %lang:key for the QPN.
 */
static const char *
cbd_qpn_oid(unsigned const char *k, List *forms)
{
  Cform *cfp;
  const char *oid = NULL;
  for (cfp = list_first(forms); cfp; cfp = list_next(forms))
    if ((oid = cbd_oid(k, (ccp)cfp->f.lang)))
      break;
  return oid;
}

void
cbd_oid_e(Entry *e)
{
  const char *oid;
  
  if (qpnpos(e->cgp->pos))
    oid = cbd_qpn_oid(e->cgp->tight, e->forms);
  else
    oid = cbd_oid(e->cgp->tight, (ccp)e->owner->lang);

  if (!oid)
    oid = cbd_zid();

  e->oid = oid; /* no need to pool_copy this because it's in the oid.tab Roco */
}

void
cbd_oid_s(Sense *s)
{
  const char *oid;

  if (qpnpos(s->pos))
    oid = cbd_qpn_oid(s->cgspe, curr_entry->forms);
  else
    oid = cbd_oid(s->cgspe, (ccp)curr_entry->lang);

  if (!oid)
    oid = cbd_zid();

  s->oid = oid;
}

