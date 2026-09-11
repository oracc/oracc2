#include <oraccsys.h>
#include <roco.h>
#include "vx.h"

static Roco *cft, *fobacu, *sid;
static Hash *h_cft, *h_fobacu, *h_sid;

void
vx_epsd_init(void)
{
  char cft_fn[strlen(oracc()) + strlen("/epsd2/02pub/cft.tsv0")];
  sprintf(cft_fn, "%s/epsd2/02pub/cft.tsv", oracc());
  char fobacu_fn[strlen(oracc()) + strlen("/epsd2/02pub/fo-ba-cu.tsv0")];
  sprintf(fobacu_fn, "%s/epsd2/02pub/fo-ba-cu.tsv", oracc());
  char sid_fn[strlen(oracc()) + strlen("/epsd2/02pub/sid.tsv0")];
  sprintf(sid_fn, "%s/epsd2/02pub/sid.tsv", oracc());
  cft = roco_load1(cft_fn);
  fobacu = roco_load1(fobacu_fn);
  sid = roco_load1(sid_fn);
  h_cft = roco_hash_r(cft);
  h_fobacu = roco_hash_r(fobacu);
  h_sid = roco_hash_r(sid);
}

const char *
vx_epsd_cft(const char *cgp)
{
  const char **r = hash_find(h_cft, (uccp)cgp);
  if (r)
    return r[1];
  else
    return "_";
}

const char *
vx_epsd_oid(const char *cgp)
{
  const char **r = hash_find(h_cft, (uccp)cgp);
  if (r)
    return r[2];
  else
    return "_";
}

const char *
vx_epsd_dis(const char *cgp)
{
  const char **r = hash_find(h_cft, (uccp)cgp);
  if (r)
    return r[3];
  else
    return "_";
}

const char *
vx_epsd_sid(const char *cgspe)
{
  const char **r = hash_find(h_sid, (uccp)cgspe);
  if (r)
    return r[1];
  else
    return "_";
}

const char *
vx_epsd_sgw(const char *cgspe)
{
  const char **r = hash_find(h_sid, (uccp)cgspe);
  if (r)
    return r[2];
  else
    return "_";
}

const char *
vx_epsd_ucun(const char *tlit)
{
  const char **r = hash_find(h_fobacu, (uccp)tlit);
  if (r)
    return r[1];
  else
    return "_";
}
