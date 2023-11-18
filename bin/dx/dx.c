#include <dx.h>

int
main(int argc, char **argv)
{
  int sock, msgsock, rval;
  char buf[1024];

  dx_signal();
  dx_prechecks();
  sock = dx_listen(DX_SERVER_NAME);
  
  for (;;)
    {
      uid_t uptr;
      msgsock = dx_accept(sock, &uptr);
      if (msgsock < 0)
	perror("accept");
      else
	do
	  {
	    memset(buf, '\0', sizeof(buf));
	    if ((rval = read(msgsock, buf, 1024)) < 0)
	      perror("reading stream message");
	    else if (rval == 0)
	      printf("Ending connection\n");
	    else
	      {
		printf("-->%s\n", buf);
		dx_process(msgsock, buf);
		break;
	      }
	  }
	while (rval > 0);
      close(msgsock);
    }

  close(sock);
  unlink(DX_SERVER_NAME);
}
