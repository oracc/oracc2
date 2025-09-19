#include <oraccsys.h>

int
main(int argc, char **argv)
{
  ucp *ll = loadfile_lines((uccp)argv[1], NULL);
  int i;
  for (i = 0; ll[i]; ++i)
    printf("%lu\t%s\n", strlen((ccp)ll[i]), (ccp)ll[i]);
}
