#include <oraccsys.h>
#include "wpx_lib.h"
#include "px.h"
#include "pxdefs.h"

static void
print_hdr(void)
{
  static int hdone = 0;
  if (!hdone++)
    {
      fputs("Content-type: text/html; charset=utf-8\nAccess-Control-Allow-Origin: *\n\n", stdout);
      fflush(stdout);
    }
}

int
main(int argc, char **argv)
{
  mesg_init();
  
  Isp *ip = isp_init();

  const char *hal = getenv("HTTP_ACCEPT_LANGUAGE");
  if (hal)
    ip->halp = http_accept_language(hal);
  
  if (px_options(argc, argv, ip))
    goto error;

  if (px_validate(ip))
    goto error;

  if (isp_config(ip))
    goto error;
  
  if ((ip->part && strcmp(ip->part,"page"))
      || (ip->form && !ip->part))
    if (px_return(ip))
      {
	ip->err = wpx_last_error();
	goto error;
      }
  
 tryforce:
  
  if (isp_cache_sys(ip))
    goto error;

  if (isp_cache_project(ip))
    goto error;
  
  if (ip->srchdata.tmp)
    {
      if (isp_srch(ip))
	goto error;
      ip->srchdata.new = 1;
    }
  else if (isp_list_method(ip))
    goto error;

  if (!ip->srch || ip->srchdata.count)
    {
      if (isp_cache_sub(ip))
	goto error;

      if (ip->glos && isp_glos_list(ip))
	goto error;
  
      if (!strcmp(ip->from, "list") && isp_cache_list(ip))
	goto error;

      if (!ip->glos || ip->glosdata.xis)
	if (isp_cache_sort(ip))
	  goto error;

      if (ip->item && (!ip->glos || ip->glosdata.xis))
	{
	  if (isp_item_set(ip))
	    goto error;
	}
  
      if (isp_cache_page(ip))
	goto error;
    }
  else if (isp_srch_fixup(ip))
    goto error;
      
  if (ip->debug)
    {
      print_hdr();
      if (setjmp(ip->errjmp))
	goto error;
      else if (pui_output(ip, stdout, pui_filetext("p4debug.xml")))
	goto error;
    }
  else
    {
      if (ip->pack && !strcmp(ip->pack, "div"))
	{
	  /* option to print hdr here ? */
	  if (setjmp(ip->errjmp))
	    goto error;
	  else if (pui_output(ip, stdout, pui_filetext("p4pager.xml")))
	    goto error;
	  else if (ip->err)
	    goto error;
	}
      else
	{
	  print_hdr();  
	  if (setjmp(ip->errjmp))
	    goto error;
	  else if (pui_output(ip, stdout, pui_filetext("p4html.xml")))
	    goto error;
	}
    }

  goto ok;
  
 error:

  /* Some errors may occur when a URL has dependencies that don't
   * exist because it is a deep request for something that hasn't had
   * the logisitical infrastructure created yet.  To catch these
   * errors we set ip->force and go again which will force the
   * infrastructure creation.
   *
   * Some errors won't be improved by a second pass; they need to
   * include the keyword 'fatal' in the ip->err string which
   * suppresses the second pass attempt.
   */
  if (!ip->force && !strstr(ip->err, "fatal"))
    {
      ip->force = 2;
      ip->err = ip->errx = NULL;
      goto tryforce;
    }
  
  if (strncmp(ip->err, PX_ERROR_START, strlen(PX_ERROR_START)))
    fprintf(stderr, "%s", ip->err);
  else
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
  isp_term(ip);
  fflush(stdout);
  return 0;
}

const char *prog = "px";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
