#include <dx.h>
#include <osh.h>

void
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
  exit(1);
}
