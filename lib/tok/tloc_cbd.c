#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

#define tlb_dup(s) (ccp)hpool_copy((ucp)(s),r->p)

void
tloc_cbd_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  if (!strcmp(name, "articles"))
    {
      if (!loch_text(r) || r->multi)
	{
	  loch_text(r) = memo_new(r->t_m);
	  if (loch_xtf(r).tlocs)
	    list_add(loch_xtf(r).tlocs, loch_text(r));
	}
      r->l->project = tlb_dup(findAttr(atts, "project"));
      loch_text(r)->text_project = r->l->project;
      tlw_G(r, get_xml_lang(atts));
    }
  else if (!strcmp(name, "summary"))
    {
      if (!loch_line(r) || r->multi)
	{
	  loch_line(r) = memo_new(r->l_m);
	  if (loch_text(r)->llocs)
	    list_add(loch_text(r)->llocs, loch_line(r));
	}
      tlw_A(r, findAttr(atts, "oid"), findAttr(atts, "n"));
    }
  else if (!strcmp(name, "g:w"))
    {
      if (loch_line(r))
	{
	  if (!loch_word(r) || r->multi)
	    {
	      loch_word(r) = memo_new(r->w_m);
	      if (loch_line(r)->wlocs)
		list_add(loch_line(r)->wlocs, loch_word(r));
	    }
	  loch_word(r)->word_lang = get_xml_lang(atts);
	  loch_word(r)->word_form = tlb_dup(findAttr(atts,"form"));
	  (void)trun_word_init(r);
	  r->rw->w = loch_word(r);
	}
      else
	{
	  fprintf(stderr, "tloc_cbd_sH: g:w without summary--wrong input format? (need summaries.xml). Stop.\n");
	  exit(1);
	}
    }
}
