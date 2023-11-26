#include <../dx.h>
#include <osh.h>

static int osh_status(Job *jp);

int
osh_wait(Job *jp)
{
  fprintf(stderr, "osh[%d]: waiting on session %s\n", jp->pid, jp->sesh);
  for (;;)
    {
      sleep(3);
      jp->time += 3;
      if (osh_status(jp))
	goto error;
      if (!strcmp(jp->status, "run"))
	fprintf(stderr, "osh[%d]: session %s time %i sec\n", jp->pid, jp->sesh, jp->time);
      else if (!strcmp(jp->status, "done") || !strcmp(jp->status, "error"))
	break;
      else
	{
	  fprintf(stderr, "osh[%d]: unknown status %s found in %s\n", jp->pid, jp->status, jp->statusfile);
	  break;
	}      
    }
  return 0;
 error:
  return 1;
}

static int
osh_status(Job *jp)
{
  int fd;
  if ((fd = open(jp->statusfile, O_RDONLY)) >= 0)
    {
      int n = read(fd, jp->status, 5);
      if (n >= 0)
	{
	  jp->status[n] = '\0';
	  if (verbose)
	    fprintf(stderr, "osh[%d]: read %s and found status '%s'\n", jp->pid, jp->statusfile, jp->status);
	}
      else
	{
	  fprintf(stderr, "osh[%d]: read %s failed; %s\n", jp->pid, jp->statusfile, strerror(errno));
	  goto error;
	}
    }
  else
    {
      fprintf(stderr, "osh[%d]: unable to open status file %s; %s\n", jp->pid, jp->statusfile, strerror(errno));
      goto error;
    }
  return 0;
 error:
  return 1;
}
