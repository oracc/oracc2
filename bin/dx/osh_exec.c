#include <dx.h>
#include <osh.h>

int
osh_exec(const char *cmd, const char *arg)
{
  char av[4];
  av[0] = av[1] = cmd;
  av[2] = arg;
  av[3] = NULL;
  if (verbose)
    fprintf(stderr, "%s: exec %s %s", cmd, arg);
  execv(cmd, av);
  fprintf(stderr, "%s: failed exec of %s: %s\n", cmd, strerror(errno));
  return 1;
}
