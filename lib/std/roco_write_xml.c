#include <stdlib.h>
#include <stdio.h>
#include <roco.h>
#include <xmlify.h>

static const char *esp_ns = "http://oracc.org/ns/esp/1.0";
static const char *html_ns = "http://www.w3.org/1999/xhtml";
static const char *ood_ns = "http://oracc.org/ns/ood/1.0";
static const char *xmd_ns = "http://oracc.org/ns/xmd/1.0";

int roco_esp_ns = 0;
int roco_html_ns = 0;
int roco_ood_ns = 0;
int roco_xmd_ns = 0;
int roco_no_void_xml = 0;

#if 1
Roco_row_hook roco_row_hook_outer = NULL, roco_row_hook = NULL;
void
roco_xml_row_hooks(Roco_row_hook outer, Roco_row_hook rh)
{
  roco_row_hook_outer = outer;
  roco_row_hook = rh;
}

static void
roco_row_dflt(Roco *r, int i, FILE *fp)
{
  int j;
  const char **ftags = (r->fields_from_row1
			? (const char **)r->rows[0] : NULL);
  const char *ctag = r->celtag;
  for (j = 0; r->rows[i][j] != NULL; ++j)
    {
      if (ftags)
	ctag = ftags[j];
      if (*r->rows[i][j])
	fprintf(fp, "<%s>%s</%s>",
		ctag, roco_xmlify ? xmlify(r->rows[i][j]) : r->rows[i][j], ctag);
      else if (!roco_no_void_xml)
	fprintf(fp, "<%s/>", ctag);
      /* else don't emit an empty tag for void data */
    }
}
#else
Roco_row_hook roco_row_hook_outer = NULL, roco_row_hook_o = NULL, roco_row_hook_c = NULL;
void
roco_xml_row_hooks(Roco_row_hook outer, Roco_row_hook o, Roco_row_hook c)
{
  roco_row_hook_outer = outer;
  roco_row_hook_o = o;
  roco_row_hook_c = c;
}
#endif

void
roco_write_xml(FILE *fp, Roco *r)
{
  size_t i;
  int start_row = r->fields_from_row1;
  if ('-' != *r->xmltag)
    {
      fprintf(fp, "<%s", r->xmltag);
      if (roco_html_ns)
	fprintf(fp, " xmlns=\"%s\"", html_ns);
      if (roco_esp_ns)
	fprintf(fp, " xmlns:esp=\"%s\"", esp_ns);
      if (roco_ood_ns)
	fprintf(fp, " xmlns:o=\"%s\"", ood_ns);
      if (roco_xmd_ns)
	fprintf(fp, " xmlns=\"%s\" xmlns:xmd=\"%s\"", xmd_ns, xmd_ns);
      if (r->class)
	fprintf(fp, " class=\"%s\"", r->class);
      fputc('>', fp);
    }
  for (i = start_row; i < r->nlines; ++i)
    {
      if (roco_format)
	{
	  roco_row_format(fp, (uccp*)r->rows[i]);
	}
      else
	{
	  if (roco_row_hook_outer)
	    roco_row_hook_outer(r,i,fp);
	  fprintf(fp, "<%s\n>", r->rowtag);
#if 1
	  if (roco_row_hook)
	    roco_row_hook(r,i,fp);
	  else
	    roco_row_dflt(r,i,fp);
#else
	  if (roco_row_hook_o)
	    roco_row_hook_o(r,i,fp);
	  int j;
	  for (j = 0; r->rows[i][j] != NULL; ++j)
	    {
	      if (ftags)
		ctag = ftags[j];
	      if (*r->rows[i][j])
		fprintf(fp, "<%s>%s</%s>",
			ctag, roco_xmlify ? xmlify(r->rows[i][j]) : r->rows[i][j], ctag);
	      else if (!roco_no_void_xml)
		fprintf(fp, "<%s/>", ctag);
	      /* else don't emit an empty tag for void data */
	    }
	  if (roco_row_hook_c)
	    roco_row_hook_c(r,i,fp);
#endif
	  fprintf(fp, "</%s\n>", r->rowtag);
	}
    }
  if ('-' != *r->xmltag)
    fprintf(fp, "</%s>", r->xmltag);
}
