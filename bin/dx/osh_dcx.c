#include <dx.h>
#include <osh.h>

#define DATA "hello"

int
osh_dcx(char **optv)
{
  int sock, rval;
  char buf[1024];

  if (verbose)
    printf("dcx: trying to connect to %s\n", DX_SERVER_NAME);

  if ((sock = dx_connect(DX_SERVER_NAME)) < 0)
    {
      perror("Error connecting stream socket (dx not running?)");
      return 1;
    }
  else
    printf("dcx: connection successful\n");

  if (write(sock, DATA, sizeof(DATA)) < 0)
    {
      perror("writing on stream socket");
      return 1;
    }
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
  return 0;
}
