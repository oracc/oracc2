#include <oraccsys.h>
#include <spawn.h>

int rnvc_use_posix_spawn = 1;

int
rnv_check(const char *rnc, const char *xml)
{
  static char *rncfn, *rnvbin;

  if (!rnc)
    {
      if (!rncfn)
	{
	  fprintf(stderr, "rnv_check: called with NULL rnc");
	  return -1;
	}
    }
  if (access(rnc, R_OK))
    {
      xperror("rnv_check: %s", rnc);
      fputc('\n', stderr);
      return -1;
    }
  else
    rncfn = strdup(rnc);

  if (!xml)
    {
      if (rncfn)
	{
	  free(rncfn);
	  rncfn = NULL;
	  if (rnvbin)
	    {
	      free(rnvbin);
	      rnvbin = NULL;
	    }
	  return 0;
	}
      else
	{
	  fprintf(stderr, "rnv_check: called with NULL xml");
	  return -1;
	}
    }

  if (access(xml, R_OK))
    {
      xperror("rnv_check: %s", xml);
      fputc('\n', stderr);
      return -1;
    }

  if (!rnvbin)
    {
      char rnvbuf[strlen(oracc())+strlen("/bin/rnv0")];
      sprintf(rnvbuf, "%s/bin/rnv", oracc());
      rnvbin = strdup(rnvbuf);
    }

  int status = 0;
  if (rnvc_use_posix_spawn)
    {
      extern char**environ;
      pid_t pid = 0;
      char *const args[] = { rnvbin, rncfn, (char*)xml, NULL };
      status = posix_spawnp(&pid, rnvbin, NULL, NULL, args, environ);

      if (status == 0)
        waitpid(pid, &status, 0);
      else
        perror("posix_spawn failed");
    }
  else
    {
      List *argsl = list_create(LIST_SINGLE);
      list_add(argsl, rnvbin);
      list_add(argsl, " ");
      list_add(argsl, (void*)rncfn);
      list_add(argsl, " ");
      list_add(argsl, (void*)xml);
      unsigned char *syscmd = list_concat(argsl);
      status = system((ccp)syscmd);
      free(syscmd);
    }

  if (status)
    {
      int xstatus = WEXITSTATUS(status);
      if (xstatus > 1)
	return xstatus;
      else
	{
	  fprintf(stderr,
		  "rnv_check: failed system call: %s %s %s\n",
		  rnvbin, rnc, xml);
	  return -1;
	}
      
    }
  else
    return 0;
}
