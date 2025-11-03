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

static const char *
disamb_alpha(Bx *bp, int num)
{
  static const char *alpha[] = {
    "a","b","c","d","e","f","g","h","i","j","k","l","m",
    "n","o","p","q","r","s","t","u","v","w","x","w","y","z"
  };
  if (num < 26)
    return alpha[num];

  int i = 0;
  char temp[32]; 

  /* Process the number from right to left (least significant 'digit' first) */
  while (num > 0)
    {
      int remainder = num % 26;
      temp[i++] = 'a' + remainder;
      num /= 26;
    }
  temp[i] = '\0';

  /* Reverse the temporary string into the final destination buffer */
  char *letters = malloc(i+1);
  list_add(bp->mem, letters);
  int j = 0;
  while (i > 0)
    letters[j++] = temp[--i];

  letters[j] = '\0';
  return letters;
}

void
bib_xml_entry(Bx *bp, Bibentry *ep)
{
  char dis[strlen(" disamb=\"aaaa0\"")];
  if (ep->disamb)
    sprintf(dis, " disamb=\"%s\"", disamb_alpha(bp, ep->disamb-1));
  else
    *dis = '\0';
  fprintf(xout, "<entry type=\"%s\" key=\"%s\"%s>", ep->type, xmlify((uccp)ep->bkey), dis);
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
	bib_xml_entry(bp, bp->ents[i]);
    }
  else
    {
      Bib *bibp;
      for (bibp = list_first(b); bibp; bibp = list_next(b))
	{
	  fprintf(xout, "<file n=\"%s\">", bibp->file);
	  Bibentry *ep;
	  for (ep = list_first(bibp->entries); ep; ep = list_next(bibp->entries))
	    bib_xml_entry(bp, ep);
	  fputs("</file>", xout);
	}
    }
  fputs("</bib>", xout);
}
