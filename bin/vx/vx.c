#include <oraccsys.h>
#include "vx.h"

int conllo_mode = 0;
int identity_mode = 1;
const char *translation_fn = NULL;

int
main(int argc, char *const *argv)
{
  mesg_init();
  options(argc, argv, "CIt:");
  Tree *tp = vx_load(argv[optind]);
  if (identity_mode)
    vx_identity(tp, stdout);
  else if (conllo_mode)
    vx_conllo(tp, stdout);
  mesg_print(stderr);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'C':
      conllo_mode = 1;
      identity_mode = 0;
      break;
    case 'I':
      identity_mode = 1;
      break;
    case 't':
      translation_fn = arg;
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
