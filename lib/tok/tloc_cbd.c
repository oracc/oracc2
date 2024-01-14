#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

void
tloc_cbd(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  if (!strcmp(name, "articles"))
    tlw_G(r, get_xml_lang(atts));
  else if (!strcmp(name, "summary"))
    tlw_A(r, findAttr(atts, "oid"), findAttr(atts, "n"));
}
