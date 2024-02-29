#include <oraccsys.h>
#include "isp.h"

int
main(int argc, char **argv)
{
  struct isp *ip = isp_init();

  if (isp_options(argc, argv, ip))
    goto error;

  if (isp_validate(ip))
    goto error;

  if (ip->verbose)
    isp_show(stderr, ip);

  goto ok;
  
 error:
  fprintf(stderr, "isp: error: %s. Stop.\n", ip->err);
  if (ip->web)
    printf("<error>%s</error>", ip->err);

 ok:
  fflush(stdout);
  isp_term(ip);
  return 0;
}
