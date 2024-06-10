#include <oraccsys.h>
#include "wpx_lib.h"
#include "px.h"
#include "pxdefs.h"
#include "isp/isp.h"

int precomputing;

int
main(int argc, char **argv)
{
  mesg_init();

  precomputing = 1;
  
  Isp *ip = isp_init();
  ip->from = "list";

  if (px_options(argc, argv, ip))
    goto error;

  if (px_validate(ip))
    goto error;

  if (isp_config(ip))
    goto error;

  if (isp_cache_sys(ip))
    goto error;

  if (isp_cache_project(ip))
    goto error;

  if (isp_list_method(ip))
    goto error;

  if (isp_cache_sub(ip))
    goto error;

  if (isp_cache_list(ip))
    goto error;

  if (isp_cache_sort(ip))
    goto error;

  goto ok;
  
 error:
  
  if (strncmp(ip->err, PX_ERROR_START, strlen(PX_ERROR_START)))
    fprintf(stderr, "%s", ip->err);
  else
    fprintf(stderr, ip->errx ? ip->err : PX_ERROR_START "%s. Stop.\n",
	    ip->errx ? ip->errx : ip->err);

  /* falls through to clean up */
  
 ok:
  if (ip->verbose)
    isp_show(stderr, ip);
  isp_term(ip);
  fflush(stdout);
  return 0;
}

const char *prog = "issx";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
