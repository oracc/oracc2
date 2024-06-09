#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

const void *
isp_dbx_one_off(Isp *ip, const char *proj, const char *dir, const char *name, const char *key, Unsigned32 *n)
{
  char dbdir[strlen(ip->oracc)+strlen(proj)+strlen(dir)+strlen("//0")];
  sprintf(dbdir, "%s/%s/%s", ip->oracc, proj, dir);
  Dbi_index *dp = dbx_init(dbdir, name);
  const void *ret = NULL;
  if (dp)
    {
      if (!(ret = dbx_key(dp, key, n)))
	ip->err = px_err("key %s not in dbi %s/%s", key, pool_copy((uccp)dbdir,ip->p), name);
      ret = dbi_detach_data(dp, NULL);
      dbx_term(dp);
    }
  else
    ip->err = px_err("unable to open dbi %s/%s", pool_copy((uccp)dbdir,ip->p), name);
  return ret;
}
