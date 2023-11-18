#include <dx.h>

static void set_sigactions(void);

int
main(int argc, char **argv)
{
  int sock, msgsock, rval;
  struct sockaddr_un server;
  char buf[1024];

  set_sigactions();
  
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("opening stream socket");
    exit(1);
  }
  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, DX_SOCK_NAME);
  if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)))
    {
      perror("binding stream socket");
      exit(1);
    }
  
  printf("Socket has name %s\n", server.sun_path);
  listen(sock, 5);
  for (;;)
    {
      msgsock = accept(sock, 0, 0);
      if (msgsock == -1)
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
  unlink(DX_SOCK_NAME);
}

