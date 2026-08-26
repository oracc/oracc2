#include <oraccsys.h>
#include "vx.h"

int identity_mode = 1;

int
main(int argc, char *const *argv)
{
  mesg_init();
  options(argc, argv, "I");
  Tree *tp = vx_load(argv[optind]);
  if (identity_mode)
    vx_identity(tp, stdout);
  mesg_print(stderr);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'I':
      identity_mode = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  fprintf(stderr, "vx -[MODE] [FILE]\n");
}
