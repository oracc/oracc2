#include <oraccsys.h>
#include <roco.h>
#include <xml.h>
#include "cx.h"

/* Hooks for Roco library to generate backward-compatible XMD */

static void
cx_roco_outer(Roco *r, int i, FILE *fp)
{
  char *dest = expand(((Cx *)r->user)->project, (ccp)r->rows[i][0], "xmd");
  fprintf(fp, "<?destfile %s?>\n", dest);
}

static void
cx_roco_row(Roco *r, int i, FILE *fp)
{
  int j;
  const char **ftags = (r->fields_from_row1
			? (const char **)r->rows[0] : NULL);
  const char *ctag = r->celtag;
#define cxp(ru) ((Cx*)(ru->user))
  fputs("<cat>", fp);
  for (j = 0; j < r->maxcols; ++j)
    {
      if (ftags)
	ctag = ftags[j];
      if (cxp(r)->si_rows[i][j].type == FCELL_SORT)
	{
	  fprintf(fp, "<%s c=\"%ld\">%s</%s>",
		  ctag, cxp(r)->si_rows[i][j].sort,
		  xmlify(ipool_str(cxp(r)->si_pool,cxp(r)->si_rows[i][j].u.index)),
		  ctag);
	}
      else
	{
	  const char *s = cxp(r)->si_rows[i][j].u.str;
	  if (*s)
	    fprintf(fp, "<%s>%s</%s>", ctag, xmlify((uccp)s), ctag);
	}
    }
  fputs("</cat>", fp);
#undef cxp
}

#if 0
static void
cx_roco_o(Roco *r, int i, FILE *fp)
{
  fputs("<cat>", fp);
}
static void
cx_roco_c(Roco *r, int i, FILE *fp)
{
  fputs("</cat>", fp);
}
#endif

void
cx_roco(Cx *c)
{
  roco_xmd_ns = 1;
  roco_xml_row_hooks(cx_roco_outer, cx_roco_row);
}

int
cx_roco_id_index(Cx *c)
{
  int f;
  for (f = 0; c->r->rows[0][f]; ++f)
    {
      uccp f1 = c->r->rows[0][f];
      if (!strcmp((ccp)f1, "id_text")
	  || !strcmp((ccp)f1, "id_composite")
	  || !strcmp((ccp)f1, "o:id"))
	return f;
    }
  return -1;
}
