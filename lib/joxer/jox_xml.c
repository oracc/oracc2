#include <stdio.h>
#include <xml.h>
#include <joxer.h>
#include <scan.h>
#include <inl.h>

static FILE *f_xml;

void
jox_xml_output(FILE *fp)
{
  f_xml = fp;
}

/* ch must already have been put through xmlify to pass validation */
void
jox_xml_ch(const char *ch)
{
  fputs(ch,f_xml);
}

void
jox_xml_inl(Mloc *mp, char *ch)
{
  static int first = 1;
  if (first)
    {
      scan_init();
      inl_init();
      first = 0;
    }
  Tree *tp = inl(mp, ch);
  tree_xml(f_xml, tp);
}

void
jox_xml_ea(const char *pname, Rats *ratts)
{
  int i;
  fprintf(f_xml, "<%s", pname);
  if (xml_xmlns_atts)
    {
      int i;
      for (i = 0; xml_xmlns_atts[i].ns; ++i)
	fprintf(f_xml, " %s=\"%s\"", xml_xmlns_atts[i].ns, xml_xmlns_atts[i].uri);
      xml_xmlns_atts = NULL;
    }

  if (ratts)
    for (i = 0; ratts->atts[i]; i+=2)
      fprintf(f_xml, " %s=\"%s\"", ratts->atts[i], ratts->atts[i+1]);

  fprintf(f_xml, ">");
}

void
jox_xml_ee(const char *pname)
{
  fprintf(f_xml, "</%s>", pname);
}
