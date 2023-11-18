#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define DATA "Half a league, half a league . . ."

int
main(int argc, char **argv)
{
  int sock, rval;
  struct sockaddr_un server;
  char buf[1024];

  if (argc < 2) {
    printf("usage:%s <pathname>", argv[0]);
    exit(1);
  }

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("opening stream socket");
    exit(1);
  }

  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, argv[1]);

  if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0)
    {
      close(sock);
      perror("connecting stream socket");
      exit(1);
    }
  if (write(sock, DATA, sizeof(DATA)) < 0)
    perror("writing on stream socket");

  do
    {
      memset(buf, '\0', sizeof(buf));
      if ((rval = read(sock, buf, 1024)) < 0)
	perror("reading back message from dx");
      else if (rval == 0)
	printf("Ending dx connection\n");
      else
	printf("<--%s\n", buf);
    }
  while (rval > 0);
  
  close(sock);
}
