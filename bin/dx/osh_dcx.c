#include <dx.h>
#include <osh.h>

int
osh_dcx(char **optv)
{
  int sock, rval;
  char buf[1024];
  char *request = NULL;
  int i, len;

  for (i = 1, len = 0; optv[i]; ++i)
    len += strlen(optv[i]) + 1;
  request = malloc(len+1);
  *request = '\0';
  for (i = 1; optv[i]; ++i)
    strcat(strcat(request, optv[i]), " ");
  request[strlen(request)-1] = '\0';
  
  if (verbose)
    fprintf(stderr, "%s: dcx: trying to connect to %s\n", progname, DX_SERVER_NAME);

  if ((sock = dx_connect(DX_SERVER_NAME)) < 0)
    {
      perror("Error connecting stream socket (dx not running?)");
      goto error;
    }
  else if (verbose)
    fprintf(stderr, "%s: dcx[%d]: connection successful\n", progname, sock);

  if (verbose)
    fprintf(stderr, "%s: dcx[%d]: sending %s\n", progname, sock, request);

  if (write(sock, request, strlen(request)) < 0)
    {
      perror("writing on stream socket");
      goto error;
    }
  else
    {
      do
	{
	  memset(buf, '\0', sizeof(buf));
	  if ((rval = read(sock, buf, 1024)) < 0)
	    perror("Error reading back message from dx");
	  else if (rval == 0)
	    fprintf(stderr, "%s: dcx[%d]: ending connection\n", progname, sock);
	  else
	    {
	      if ('\n' == buf[strlen(buf)-1])
		buf[strlen(buf)-1] = '\0';
	      if (verbose)
		fprintf(stderr, "%s: dcx[%d]: received '%s'\n", progname, sock, buf);
	    }
	}
      while (rval > 0);
    }
  free(optv);
  close(sock);
#define cleanup() \
  if (request) free(request)
  cleanup();
  return 0;
 error:
  cleanup();
  return 1;
}
