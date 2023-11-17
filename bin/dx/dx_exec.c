#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "soapH.h"
#include "osc-lib.h"

int wait_seconds = 1;

const char *name = "/home/oracc/bin/odo";
const char *path = "/bin/dash";

#define ERR_MAX 512

/* Caller gets error in err and should also call strerror(errno) */

int
dx_exec(const char **keys, int nkeys, char **err)
{
  pid_t pid;
  const char *argv[nkeys+3];

  assert(err);
  *err = NULL;
  
  if (!keys || nkeys <= 0)
    {
      
      return 1;
    }
  
  argv[0] = argv[2] = name;
  for (i = 0; i < keys->__size; ++i)
    argv[i+2] = keys->key[i];
  argv[i+2] = NULL;

  if ((pid = fork()) < 0)
    {
      /* set error status to return to client */
      *err = "failed to fork child";
      return 1;
    }
  else if (pid != 0) /* parent */
    {
      /* either wait for child if the method has wait_seconds == 0, or return OK 
	 status to send back to client, which will then ping the server until it
	 gets back a 'completed' status
       */
      if (0 == wait_seconds)
	{
	  int sloc;
	  waitpid(pid, &sloc, 0);
	  if (WIFSIGNALED(sloc))
	    {
	      *err = "unexpected failure or crash";
	      return 1;
	    }
	  /* return success and let caller pick up the log file for response */
	  /* NOTE THAT IN THIS BLOCK THE COMMAND HAS EXECUTED ON RETURN */
	  return 0;
	}
      else
	{
	  /* NOTE THAT IN THIS BLOCK THE COMMAND IS STILL RUNNING */
	  int sloc;
	  waitpid(pid, &sloc, 0);
	  return 0;
	}
    }
  else /* child */
    {
      int fd;

      if ((pid = fork()) < 0)
	{
	  *err = soap_sender_fault(soap, "can't fork again", strerror(errno));
	  return 1;
	}
      else if (pid != 0) /* parent */
	{
#if 0
	  int sloc;
	  waitpid(pid, &sloc, 0);
#endif
	  exit(0);
	}

      setsid();

#if 0
      fprintf(stderr, "oracc-exec: switching grandchild output to %s\n", request_log);

      if ((fd = open(request_log, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0)
	{
	  *err = soap_sender_fault(soap, "child failed to open request log", NULL);
	  return 1;
	}
      if (dup2(fd, fileno(stderr)) < 0)
	{
	  *err = soap_sender_fault(soap, "child failed to attach stdout", NULL);
	  return 1;
	}
      if (dup2(fd, fileno(stdout)) < 0)
	{
	  *err = soap_sender_fault(soap, "child failed to attach stderr", NULL);
	  return 1;
	}
      else
	setbuf(stdout, NULL);
#endif

      if (execve(path, (char *const *)argv, NULL)) /* (char *const *)envp */
	{
	  fprintf(stderr, "exec failed: path=%s\n%s\n", path, strerror(errno));
	  *err = "exec failed";
	  return 1;
	}
      else
	return 0; /* NEVER HAPPENS */
    }
  return 0;
}
