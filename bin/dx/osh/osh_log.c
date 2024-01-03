#include <oraccsys.h>
#include "../dx.h"
#include <osh.h>

#define DXLOG_MAX 512

const char *
osh_log(Job *jp)
{
  char dxlogfn[strlen(oracc_builds())+strlen(jp->project)+strlen(DX_DOTLOG)+3];
  sprintf(dxlogfn, "%s/%s/%s", oracc_builds(), jp->project, DX_DOTLOG);
  FILE *sidf = fopen(dxlogfn, "r");
  if (sidf)
    {
      char dxlog[DXLOG_MAX];
      fgets(dxlog, DXLOG_MAX, sidf);
      fclose(sidf);
      return strdup(dxlog);
    }
  else
    {
      perror("failed to open project .dxlog file");
      return NULL;
    }
}
