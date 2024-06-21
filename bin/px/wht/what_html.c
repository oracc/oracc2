#include <oraccsys.h>
#include "../isp/isp.h"
#include "../pxdefs.h"
#include "../wpx_lib.h"
#include "what.h"

int
what_html(Isp *ip)
{
  const char *ext = (!strcmp(ip->what, "score") ? "sxh" : "html");
  char *div = expand(ip->itemdata.proj, ip->item, ext);
  const char *line = NULL, *frag = NULL;
  struct content_opts *cop = what_content_init();
  cop->echo = 1;
  cop->hilite_id = line;
  cop->frag_id = frag;
  cop->sigs = 1;

  if (strcmp(ip->what,"score") && strcmp(ip->lang,"en"))
    {
      /* the Spanish version of babrit P363710 is P363710/html.es */
      char *tmp = malloc(strlen(div) + strlen(ip->lang) + 2);
      sprintf(tmp, "%s.%s", div, ip->lang);
      free(div);
      div = tmp;
    }
  else if (!strcmp(ip->what, "score"))
    {
      cop->html = 1;
    }
  
#if 0
  if (query_string && *query_string)
    {
      char *qs = malloc(strlen(query_string)+1);
      strcpy(qs, query_string);
      line = qs;
      qs = strchr(qs, ',');
      if (qs)
	{
	  *qs++ = '\0';
	  frag = qs;
	}
      if ((!strlen(line) || !_is_ncname(line))
	  || (frag && strlen(frag) && !_is_ncname(frag)))
	{
	  do404();
	}
    }
#endif

  wpx_print_hdr();
  return what_content(ip, cop, div);
}
