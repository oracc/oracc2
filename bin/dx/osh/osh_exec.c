#include <../dx.h>
#include <osh.h>

int
osh_exec(const char *cmd, const char *arg, Job *jp)
{
  const char *av[3];
  av[0] = cmd;
  av[1] = (arg && *arg) ? arg : NULL;
  av[2] = NULL;
  if (verbose)
    fprintf(stderr, "%s: exec %s %s\n", progname, cmd, arg);
  osh_cleanup(jp);
  execv(cmd, (char *const*)av);
  fprintf(stderr, "%s: failed exec of %s: %s\n", progname, cmd, strerror(errno));
  return 1;
}
