#include <oraccsys.h>
#include "../../px/px.h"
#include "../../px/pxdefs.h"

struct sdata
{
  const char *prj;
  const char *pub;
  const char *tmp;
  const char *bar;
  const char *idx;
} sdata;

int p4status = 0;

void
px(const char *project, const char *tmpdir)
{
  Pool *p = pool_init();
  const char *err = NULL;
  mesg_init();
  sdata.prj = (ccp)pool_alloc(strlen(oracc_builds())+strlen(project)+2, p);
  sprintf((char*)sdata.prj, "%s/%s", oracc_builds(), project);
  struct stat sb;
  if (stat(sdata.prj, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      err = pool_copy(qx_err("unknown project %s", sdata.prj), p);
      goto wrapup;
    }

 wrapup:
  if (err)
    {
      fputs(err, stderr);
      p4status = 1;
    }
  mesg_term();
  pool_term(p);
  return;
}
