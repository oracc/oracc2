#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

void
tok_m_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if (!strcmp(name, "linkbase"))
    {
      if (!r->rs.F_done++)
	fprintf(r->o, "F\t%s\n", r->rs.file);
    }
  else if (!strcmp(name, "linkset"))
    {
      fprintf(r->o, "M\t%s\n", get_xml_id(atts));
      fprintf(r->o, "m\t%s\t%s\t%s\n", findAttr(atts, "xl:title"), findAttr(atts, "xl:role"), findAttr(atts, "sig"));
    }
}
void
tok_m_eH(void *userData, const char *name)
{
}
