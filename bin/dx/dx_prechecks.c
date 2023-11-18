#include <dx.h>

void
dx_prechecks(void)
{
  struct stat statbuf;
  if (!stat (DX_SERVER_NAME, &statbuf))
    {
      unlink(DX_SERVER_NAME);
      if (!stat (DX_SERVER_NAME, &statbuf))
	{
	  fprintf(stderr, "dx: server socket %s already exists and I can't unlink it. Stop.", DX_SERVER_NAME);
	  exit(1);
	}
    }
  /* should have an already running check as well */
}
