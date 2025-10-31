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
bib_xml(List *b, FILE *fp)
{
  xout = fp;
  Bib *bp;
  fputs("<bib xmlns=\"http://oracc.org/ns/bib/1.0\">", xout);
  for (bp = list_first(b); bp; bp = list_next(b))
    {
      fprintf(xout, "<file n=\"%s\">", bp->file);
      Bibentry *ep;
      for (ep = list_first(bp->entries); ep; ep = list_next(bp->entries))
	{
	  fprintf(xout, "<entry type=\"%s\" key=\"%s\">", ep->type, xmlify((uccp)ep->key));
	  int i;
	  for (i = 0; i < f_top; ++i)
	    {
	      if (ep->fields[i])
		fprintf(xout, "<%s>%s</%s>", fldnames[i], xmlify((uccp)ep->fields[i]), fldnames[i]);
	    }
	  fputs("</entry>", xout);
	}
      fputs("</file>", xout);
    }
  fputs("</bib>", xout);
}
