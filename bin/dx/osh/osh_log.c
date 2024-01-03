#include "../dx.h"
#include <osh.h>

#define DXLOG_MAX 512

int
osh_log(Job *jp)
{
  char dxlogfn[strlen(oracc_builds())+strlen(jp->project)+strlen(DX_DOTLOG)+3];
  sprintf(dxlogfn, "%s/%s/%s", oracc_builds(), jp->project, DX_DOTLOG);
  FILE *sidf = fopen(dxlogfn, "r");
  if (sid)
    {
      char dxlog[DXLOG_MAX];
      fgets(dxlog, DXLOG_MAX, sidf);
      close(sidf);
      char catlog[strlen("cat "+strlen(dxlog)+1];
      sprintf(catlog, "cat %s", dxlog);
      system(catlog);
      return 0;
    }
  else
    {
      perror("failed to open session file");
      return 1;
    }
}
