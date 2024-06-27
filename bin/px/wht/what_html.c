#include <oraccsys.h>
#include "../isp/isp.h"
#include "../pxdefs.h"
#include "../wpx_lib.h"
#include "what.h"

static int html_xtf_xmd(Isp *ip);

int
what_html(Isp *ip)
{
  const char *ext = (!strcmp(ip->what, "score") ? "sxh" : "html");
  char *div = expand(ip->itemdata.proj, ip->item, ext);

  if (access(div, R_OK))
    {
      int ret = html_xtf_xmd(ip);
      if (ret)
	return PX_ERROR;
      else
	div = NULL;
    }    
  
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

  wpx_print_hdr();
  return what_content(ip, cop, div);
}

static int
html_xtf_xmd(Isp *ip)
{
  if (!strcmp(ip->what, "text") && !strcmp(ip->form, "html"))
    {
      ip->data = "dtxt";
      ip->itemdata.item = ip->item;
    }
  else
    {
      if (!strcmp(ip->what, "meta"))
	ip->data = "dcat";
      else
	ip->data = "dtxt";
    }
  return isp_item_xtf(ip); /* there's no list context so isp_item_set is not right */
}
