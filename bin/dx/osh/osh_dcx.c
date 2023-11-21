#include <../dx.h>
#include <osh.h>

/**osh_dcx sets up a connection to the dx server, sends the action
   request, and receives the name of the logfile for the process
   carrying out the action.  Then it closes the connection and
   returns.  If successful, osh_wait() will track the progress of the
   action and return information to caller.
 */
int
osh_dcx(char **optv, struct job *jp)
{
  int sock, rval;
  char buf[1024];

  if (verbose)
    fprintf(stderr, "%s: dcx: trying to connect to %s\n", progname, DX_SERVER_NAME);

  if ((sock = dx_connect(DX_SERVER_NAME)) < 0)
    {
      perror("Error connecting stream socket (dx not running?)");
      goto error;
    }
  else if (verbose)
    fprintf(stderr, "%s: dcx[%d]: connection successful\n", progname, sock);

  /* Send the request */
  if (verbose)
    fprintf(stderr, "%s: dcx[%d]: sending %s\n", progname, sock, jp->cmd);
  if (write(sock, jp->cmd, strlen(jp->cmd)) < 0)
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
	      char *p = strstr(buf, "::");
	      if (p)
		{
		  jp->sesh = strdup(p + 2);
		  *p = '\0';
		  *p++ = '/';
		  strcpy(p, "request.log");
		  jp->log = strdup(buf);
		  strcpy(p, "status");
		  jp->statusfile = strdup(buf);
		  p[-1] = '\0';
		  jp->tmp = strdup(buf);
		  if (verbose)
		    fprintf(stderr, "jp->sesh = %s; jp->tmp=%s\n"
			    	"  jp->log=%s;\n  jp->statusfile=%s\n",
			    jp->sesh, jp->tmp, jp->log, jp->statusfile);
		}
	      else
		goto error;
	      
	    }
	}
      while (rval > 0);
    }
  close(sock);
  return 0;
 error:
  return 1;
}
