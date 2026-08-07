#include "../lib/dx.h"

const char *progname = NULL;
int verbose = 1;

static void
dx_server(void)
{
  int sock, msgsock, rval;
  char buf[1024];

  dx_prechecks();
  sock = dx_listen(DX_SERVER_NAME);

  for (;;)
    {
      if (verbose)
	fprintf(stderr, "%s: listening on socket [%d]\n", progname, sock);
      uid_t uptr;
      msgsock = dx_accept(sock, &uptr);
      if (msgsock < 0)
	perror("accept");
      else
	{
	  fprintf(stderr, "%s: opened connection [%d]\n", progname, msgsock);
	  do
	    {
	      memset(buf, '\0', sizeof(buf));
	      if ((rval = read(msgsock, buf, 1024)) < 0)
		perror("reading stream message");
	      else if (rval == 0)
		{
		  fprintf(stderr, "%s: closing connection [%d]\n", progname, msgsock);
		  close(msgsock);
		}
	      else
		{
		  if (verbose)
		    fprintf(stderr, "%s: read '%s' on [%d]\n", progname, buf, msgsock);
		  dx_process(msgsock, buf);
		  break;
		}
	    }
	  while (rval > 0);
	}
    }

  close(sock);
  unlink(DX_SERVER_NAME);
}

static void
dx_tester(int argc, char *const *argv)
{
  char *sesh_id;
  dx_session(&sesh_id);
}

int
main(int argc, char * const *argv)
{
  progname = argv[0];
  dx_signal();
  if (argv[1])
    dx_tester(argc, argv);
  else
    {
      if (-1 == daemon(1,1))
	{
	  perror("failed to make dx a daemon");
	  exit(1);
	}
      dx_server();
    }
}
