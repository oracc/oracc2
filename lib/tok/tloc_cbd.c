#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

void
tloc_cbd(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "articles"))
    1; /*fprintf(tab, "G\t%s\n", get_xml_lang(atts));*/
  else if (!strcmp(name, "summary"))
    1; /*fprintf(tab, "A\t%s\t%s\n", findAttr(atts, "oid"), findAttr(atts, "n"));*/
}
