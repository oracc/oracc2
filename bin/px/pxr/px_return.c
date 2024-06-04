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
    }
  return 0;
}
