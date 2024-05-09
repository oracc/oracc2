#include <oraccsys.h>
#include "qx.h"

int
qx_error(struct qxdata *qp, struct sdata *sdp)
{
  const char *nl = "";
  const char *err = (sdp->err ? sdp->err : qp->err);
  if (err)
    {
      if ('\n' != err[strlen(err)-1])
	nl = "\n";
      fprintf(stderr, "%s%s", err, nl);
    }
  else
    {
      fprintf(stderr, "qx: Curious: qx_error called with no error messages\n");
    }
  return 0;
}
