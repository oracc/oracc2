#include <oraccsys.h>
#include "px.h"

static void
print_hdr(void)
{
  fputs("Content-type: text/html; charset=utf-8\nAccess-Control-Allow-Origin: *\n\n", stdout);
  fflush(stdout);
}

int
main(int argc, char **argv)
{
  Isp *ip = isp_init();

  if (px_options(argc, argv, ip))
    goto error;

  if (px_validate(ip))
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

  if (ip->item && isp_item_set(ip))
    goto error;
  
  if (isp_cache_page(ip))
    goto error;

#if 1
  if (ip->debug)
    {
      print_hdr();  
      if (pui_output(ip, stdout, pui_filetext("p4debug.xml")))
	goto error;
    }
  else
    {
      if (ip->pack && !strcmp(ip->pack, "div"))
	{
	  /* option to print hdr here ? */
	  if (pui_output(ip, stdout, pui_filetext("p4pager.xml")))
	    goto error;
	}
      else
	{
	  print_hdr();  
	  if (pui_output(ip, stdout, pui_filetext("p4html.xml")))
	    goto error;
	}
    }
#else
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
#endif

  goto ok;
  
 error:
  fprintf(stderr, ip->errx ? ip->err : PX_ERROR_START "%s. Stop.\n",
	  ip->errx ? ip->errx : ip->err);
  if (ip->web)
    {
      if (!strcmp(ip->xhmd, "xml"))
	printf("<error>%s</error>", ip->err);
      else
	{
	  print_hdr();
	  if (pui_output(ip, stdout, pui_filetext("p4error.xml")))
	    {
	      fprintf(stderr, "px: error in error reporting: %s\n", ip->err);
	    }
	    
	}
    }

  /* falls through to clean up */
  
 ok:
  if (ip->verbose)
    isp_show(stderr, ip);
  fflush(stdout);
  isp_term(ip);
  return 0;
}

const char *prog = "px";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";