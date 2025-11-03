#include <oraccsys.h>
#include <xml.h>
#include "bx.h"
#include "bib.h"

static FILE *xout;

const char *fldnames[] = {
  "address", "author", "authortype", "bookauthor", "booksubtitle",
  "booktitle", "chapter", "date", "doi", "edition", "editor",
  "entrysubtype", "eprint", "eprinttype", "event", "eventdate",
  "eventtitle", "eventvenue", "ids", "institution", "isbn", "issn",
  "journal", "keywords", "langid", "location", "month", "note",
  "number", "page", "pages", "part", "publisher", "related",
  "relatedtype", "school", "series", "shorthand", "subtitle", "title",
  "translator", "type", "url", "venue", "volume", "year", NULL
};

void
bib_xml_entry(Bibentry *ep)
{
  fprintf(xout, "<entry type=\"%s\" key=\"%s\">", ep->type, xmlify((uccp)ep->bkey));
  int i;
  for (i = 0; i < f_top; ++i)
    {
      if (ep->fields[i])
	fprintf(xout, "<%s>%s</%s>",
		fldnames[i], xmlify((uccp)ep->fields[i]), fldnames[i]);
    }
  fputs("</entry>", xout);
}

void
bib_xml(Bx *bp, List *b, FILE *fp)
{
  xout = fp;
  fputs("<bib xmlns=\"http://oracc.org/ns/bib/1.0\">", xout);
  if (bp->entries)
    {
      int i;
      for (i = 0; bp->ents[i]; ++i)
	bib_xml_entry(bp->ents[i]);
    }
  else
    {
      Bib *bibp;
      for (bibp = list_first(b); bibp; bibp = list_next(b))
	{
	  fprintf(xout, "<file n=\"%s\">", bibp->file);
	  Bibentry *ep;
	  for (ep = list_first(bibp->entries); ep; ep = list_next(bibp->entries))
	    bib_xml_entry(ep);
	  fputs("</file>", xout);
	}
    }
  fputs("</bib>", xout);
}
