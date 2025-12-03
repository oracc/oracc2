#include <oraccsys.h>
#include <xml.h>
#include "bx.h"
#include "bib.h"

static FILE *xout;

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

static void
bib_xml_one_name(Name *np)
{
#define xu(s) xmlify((uccp)s)
  const char *xwas = (np->was ? (ccp)xmlify((uccp)np->was) : NULL);
  const char *was = "", *sames = "", *nmxml = "";

  char wasbuf[xwas ? strlen(xwas)+8 : 0];
  if (np->was)
    sprintf((char*)(was=wasbuf), " was=\"%s\"", xwas);

  char samesbuf[11];
  if (np->sames)
    strcpy((char*)(sames=samesbuf), " sames=\"1\"");

  char nmxmlbuf[11];
  if (np->bm_name_xml)
    strcpy((char*)(nmxml=nmxmlbuf), " nmxml=\"1\"");
  
  fprintf(xout,
	  "<name nmkey=\"%s\" orig=\"%s\" last=\"%s\" rest=\"%s\" init=\"%s\"%s%s%s/>",
	  xu(np->nkey),
	  xu(np->orig),
	  xu(np->last),
	  xu(np->rest),
	  xu(np->init),
	  was,
	  sames,
	  nmxml);
#undef xu
}

static void
bib_xml_names(Bibentry *ep, Ftype f, Nameinfo *nip)
{
  if (nip)
    {
      fprintf(xout, "<%s all=\"%s\" num=\"%d\"><data>%s</data>",
	      fldnames[f], xmlify((uccp)nip->allnames), nip->nnames,
	      xmlify((uccp)ep->fields[f]->data));
      int i;
      for (i = 0; i < nip->nnames; ++i)
	bib_xml_one_name(nip->names[i]);
      fprintf(xout, "</%s>", fldnames[f]);
    }
}

void
bib_xml_entry(Bx *bp, Bibentry *ep, int sort)
{
  char dis[strlen(" disamb=\"aaaa0\"")]; *dis = '\0';
  if (ep->disamb)
    sprintf(dis, " disamb=\"%s\"", disamb_alpha(bp, ep->disamb-1));

  char bid[strlen(" xml:id=\"B123456\"0")]; *bid = '\0';
  if (ep->bid)
    sprintf(bid, " xml:id=\"%s\"", ep->bid);
  
  const char *same = ep->sameauth ? " sameauth=\"1\"" : "";
  fprintf(xout, "<entry type=\"%s\" key=\"%s\" sort=\"%d\"%s%s%s>",
	  ep->type, xmlify((uccp)ep->bkey), sort, bid,
	  dis, same);
  if (bp->mode == BX_ICF)
    fprintf(xout, "<icf>%s</icf>", ep->icf->str);
  int i;
  for (i = 0; i < f_top; ++i)
    {
      if (ep->fields[i])
	{
	  switch ((Ftype)i)
	    {
	    case f_author:
	      bib_xml_names(ep, i, ep->nm_author);
	      break;
	    case f_editor:
	      bib_xml_names(ep, i, ep->nm_editor);
	      break;
	    case f_bookauthor:
	      bib_xml_names(ep, i, ep->nm_bookauthor);
	      break;
	    case f_translator:
	      bib_xml_names(ep, i, ep->nm_translator);
	      break;
	    default:
	      fprintf(xout, "<%s>%s</%s>",
		      fldnames[i], xmlify((uccp)ep->fields[i]->data), fldnames[i]);
	      break;
	    }
	}
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
	bib_xml_entry(bp, bp->ents[i], i);
    }
  else
    {
      Bib *bibp;
      int i;
      for (i = 0, bibp = list_first(b); bibp; bibp = list_next(b), ++i)
	{
	  fprintf(xout, "<file n=\"%s\">", bibp->file);
	  Bibentry *ep;
	  for (ep = list_first(bibp->entries); ep; ep = list_next(bibp->entries))
	    bib_xml_entry(bp, ep, i);
	  fputs("</file>", xout);
	}
    }
  fputs("</bib>", xout);
}
