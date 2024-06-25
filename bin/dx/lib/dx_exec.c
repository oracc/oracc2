#include <dx.h>
#include <string.h>

int wait_seconds = 1;

const char *name = "/home/oracc/bin/odo";
const char *path = "/bin/dash";

#define ERR_MAX 512

/* Caller gets error in err and should also call strerror(errno)
 */
int
dx_exec(char **keys, int nkeys, char **err, char *tmpdir)
{
  pid_t pid;
  const char *argv[nkeys+3];
  char request_log[strlen(tmpdir)+strlen("/request.log")+1];

  sprintf(request_log, "%s/request.log", tmpdir);
  
  *err = NULL;
  
  if (!keys || nkeys <= 0)
    {
      *err = "no keys";
      return 1;
    }

  int voff = !strcmp(keys[0], "-v");

  argv[0] = name;
  if (voff)
    argv[1] = "-v";
  argv[1+voff] = tmpdir;
  int i;
  for (i = voff; keys[i]; ++i)
    argv[i+2] = keys[i];
  argv[i+2] = NULL;

  fprintf(stderr, "%s: exec %s", progname, name/*path*/);
  for (i = 0; argv[i]; ++i)
    fprintf(stderr, " %s", argv[i]);
  fprintf(stderr, "\n");
  fflush(stderr);
      
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
	  *err = "can't fork again";
	  return 1;
	}
      else if (pid != 0) /* parent */
	exit(0);

      setsid();

      fprintf(stderr, "%s: logging stderr to %s\n", progname, request_log);

      if ((fd = open(request_log, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)) < 0)
	{
	  *err = "child failed to open request log";
	  return 1;
	}
      if (dup2(fd, fileno(stderr)) < 0)
	{
	  *err = "child failed to attach stderr";
	  return 1;
	}
      if (dup2(fd, fileno(stdout)) < 0)
	{
	  *err = "child failed to attach stdout";
	  return 1;
	}
      else
	setbuf(stdout, NULL);

      if (execve(name/*path*/, (char *const *)argv, NULL)) /* (char *const *)envp */
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
