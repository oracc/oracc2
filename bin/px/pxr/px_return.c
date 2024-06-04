#include <oraccsys.h>
#include "../px.h"
#include "../wpx_lib.h"
#include "pxr.h"

#if 0
void
px_cat_file(const char *file)
{
  execl("/bin/cat", "cat", file, NULL);
  perror("execl failed");
}
#endif

int
px_return_file(const char *form, const char *file)
{
  /* the file-finder routines must set wpx_err appropriately */
  if (!file)
    return 1;
  if (form && strcmp(form, "html"))
    wpx_cat_xml(form, file);
  else
    wpx_cat_html(form, file);
  return 1;
}

int
px_return(Isp *ip)
{
  if (ip->glos)
    {
      if (ip->item)
	return px_return_file(ip->form, pxr_find_gent(ip->project, ip->glos, ip->item, ip->form));
      else
	return px_return_file(ip->form, pxr_find_glos(ip->project, ip->glos, ip->form));
    }
  else if (ip->item)
    {
#if 1
      /* Phase 1: default setting */
      if (ip->part)
	{
	  if (strstr("page/full/plus", ip->part))
	    {
	      if (ip->form && strcmp(ip->form, "html"))
		{
		  /* unsupported; shall we throw an error or force html? */
		  ip->form = "html";
		}
	    }
	}
      else
	{
	  if (ip->form)
	    {
	      if (!strcmp(ip->form, "html") && (!ip->what || !strcmp(ip->what, "text")))
		ip->part = "full";
	    }
	  else if (ip->what)
	    {
	      if (!strcmp(ip->what, "text"))
		ip->form = "html";
	      /* else futured */
	    }
	}
      /* Phase 2: determine support and required inputs */
      /* Phase 3: generate inputs if necessary */
      /* Phase 4: return outputs */
#else
      const char *dtype = NULL, *dlang = NULL;
      if (!ip->data)
	dtype = "xtf";
      else if (!strcmp(ip->data, "score"))
	dtype = "sxh";
      else if (!strcmp(ip->data, "trans"))
	{
	  dtype = "xtr";
	  dlang = ip->lang;
	}
      else if (strcmp(ip->data, "meta"))
	dtype = "xmd";
#endif
    }
  return 0;
}
