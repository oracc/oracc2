#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

void
tloc_xtf_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if (!strcmp(name, "transliteration")
      || !strcmp(name, "composite")
      || !strcmp(name, "score"))
    {
      tlb_T(r, findAttr(atts, "project"), get_xml_id(atts), findAttr(atts, "n"));

      r->rs.printing = 1;
      r->rs.in_xcl = 0;

      /*fprintf(tab,"T\t%s\t%s\t%s\n",project,pqx,tlabel);*/

      mds_xmd(r, loch_text(r)->text_project, loch_text(r)->text_id);
    }
  else if (!r->rs.in_xcl && !strcmp(name, "l"))
    {
      tlb_L(r, pi_line, get_xml_id(atts), findAttr(atts, "label"));
      /*fprintf(tab,"L\t%d\t%s\t%s\n",pi_line,lid,llabel);*/
      r->rs.printing = 1;
    }
  else if (!strcmp(name, "g:w") || !strcmp(name, "g:nonw"))
    {
      tlb_W(r, get_xml_id(atts), get_xml_lang(atts), findAttr(atts, "form"));
      /*fprintf(tab, "W\t%s\n", get_xml_id(atts));*/
    }
  else if (!strcmp(name, "xcl"))
    r->rs.in_xcl = 1;
}

