#include <oraccsys.h>
#include "p4url.h"

int
main(int argc, char **argv)
{
  static P4url p;
  p4url(&p, argv[1], argv[2]);
}
