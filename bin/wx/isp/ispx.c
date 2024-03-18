#include <oraccsys.h>
#include "isp.h"

int
main(int argc, char **argv)
{
  Isp *ip = isp_init();

  if (isp_options(argc, argv, ip))
    goto error;

  if (isp_validate(ip))
    goto error;

  if (isp_list_method(ip))
    goto error;

  if (isp_cache_sys(ip))
    goto error;
  
  if (isp_cache_sub(ip))
    goto error;

  if (isp_cache_list(ip))
    goto error;

  if (isp_cache_sort(ip))
    goto error;

  if (isp_cache_page(ip))
    goto error;
  
  /* do this after isp_cache_page to ensure ip->md1 is set */
  if (iss_outline_dump(ip))
    goto error;

  if (ip->p3)
    {
      if (isp_p3(ip, stdout))
	goto error;
    }
  else
    {
      if (isp_ui(ip, stdout))
	goto error;
    }
  
  goto ok;
  
 error:
  fprintf(stderr, ip->errx ? ip->err : ISP_ERROR_START "%s. Stop.\n",
	  ip->errx ? ip->errx : ip->err);
  if (ip->web)
    printf("<error>%s</error>", ip->err);

  /* falls through to clean up */
  
 ok:
  if (ip->verbose)
    isp_show(stderr, ip);
  fflush(stdout);
  isp_term(ip);
  return 0;
}

const char *prog = "ispx";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
