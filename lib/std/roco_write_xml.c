#include <stdlib.h>
#include <stdio.h>
#include <roco.h>
#include <xmlify.h>

static const char *html_ns = "http://www.w3.org/1999/xhtml";
int roco_html_ns = 0;

void
roco_write_xml(FILE *fp, Roco *r)
{
  size_t i;
  int start_row = r->fields_from_row1;
  const char **ftags = (r->fields_from_row1
			? (const char **)r->rows[0] : NULL);
  const char *ctag = r->celtag;
  if ('-' != *r->xmltag)
    {
      if (roco_html_ns)
	fprintf(fp, "<%s xmlns=\"%s\">", r->xmltag, html_ns);
      else
	fprintf(fp, "<%s>", r->xmltag);
    }
  for (i = start_row; i < r->nlines; ++i)
    {
      if (roco_format)
	{
	  roco_row_format(fp, (uccp*)r->rows[i]);
	}
      else
	{
	  fprintf(fp, "<%s\n>", r->rowtag);
	  int j;
	  for (j = 0; r->rows[i][j] != NULL; ++j)
	    {
	      if (ftags)
		ctag = ftags[j];
	      if (*r->rows[i][j])
		fprintf(fp, "<%s>%s</%s>",
			ctag, roco_xmlify ? xmlify(r->rows[i][j]) : r->rows[i][j], ctag);
	      else
		fprintf(fp, "<%s/>", ctag);
	    }
	  fprintf(fp, "</%s\n>", r->rowtag);
	}
    }
  if ('-' != *r->xmltag)
    fprintf(fp, "</%s>", r->xmltag);
}
