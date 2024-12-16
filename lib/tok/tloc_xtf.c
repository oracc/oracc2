#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

int tlw_want_LW = 1;

void
tloc_xtf_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if (!strcmp(name, "transliteration")
      || !strcmp(name, "composite")
      || !strcmp(name, "score"))
    {
      /*fprintf(stderr, "found pi_file=%s\n", pi_file);*/
      tlb_T(r, findAttr(atts, "project"), get_xml_id(atts), findAttr(atts, "n"));

      r->rs.printing = 1;
      r->rs.in_xcl = 0;

      mds_xmd(r, loch_text(r)->text_project, loch_text(r)->text_id);

      tlw_T(r);
      if (tlw_want_LW)
	fprintf(r->o, "Y\txtf\n");
      r->rs.in_xcl = 0;
    }
  else if (tlw_want_LW && !r->rs.in_xcl && !strcmp(name, "l"))
    {
      tlb_L(r, pi_line, get_xml_id(atts), findAttr(atts, "label"));
      tlw_L(r);
      r->rs.printing = 1;
    }
  else if (tlw_want_LW && (!strcmp(name, "g:w") || !strcmp(name, "g:nonw")))
    {
      const char *id = get_xml_id(atts);
      if (id && *id && strncmp(id, "gdl.", 4))
	{
	  tlb_W(r, id, get_xml_lang(atts), findAttr(atts, "form"));
	  tlw_W(r);
	}
    }
  else if (!strcmp(name, "xcl"))
    r->rs.in_xcl = 1;
  else if (r->rs.in_xcl)
    {
      if (*name == 'c')
	;
      else if (*name == 'd')
	;
      else if (*name == 'l')
	tlb_M(r, get_xml_id(atts), findAttr(atts, "ref"));
    }
}
