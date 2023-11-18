#include <dx.h>

#define DATA "Testing Oracc dx server"

int verbose = 1;

int
main(int argc, char **argv)
{
  int sock, rval;
  char buf[1024];

  if (argc < 2) {
    printf("usage:%s <pathname>", argv[0]);
    exit(1);
  }

  if (verbose)
    printf("dcx: trying to connect to %s\n", DX_SERVER_NAME);

  if ((sock = dx_connect(DX_SERVER_NAME)) < 0)
    {
      perror("Error connecting stream socket (dx not running?)");
      exit(1);
    }
  else
    printf("dcx: connection successful\n");

  if (write(sock, DATA, sizeof(DATA)) < 0)
    perror("writing on stream socket");
  else
    do
      {
	memset(buf, '\0', sizeof(buf));
	if ((rval = read(sock, buf, 1024)) < 0)
	  perror("Error reading back message from dx");
	else if (rval == 0)
	  printf("Ending dx connection\n");
	else
	  printf("<--%s\n", buf);
      }
    while (rval > 0);
  
  close(sock);
}
