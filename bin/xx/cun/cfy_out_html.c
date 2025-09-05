#include <oraccsys.h>
#include "cfy.h"

static void ch_head(Cfy *c);
static void ch_body(Cfy *c);
static void ch_foot(Cfy *c);
static void ch_line_o(Cfy *c, Line *l);
static void ch_line_c(Cfy *c);

static void ch_elt_L(Cfy*c, Elt *e);
static void ch_elt_C(Cfy*c, Elt *e);
static void ch_elt_W(Cfy*c, Elt *e);
static void ch_elt_G(Cfy*c, Elt *e);
static void ch_elt_J(Cfy*c, Elt *e);
static void ch_elt_N(Cfy*c, Elt *e);
static void ch_elt_F(Cfy*c, Elt *e);
static void ch_elt_R(Cfy*c, Elt *e);
static void ch_elt_X(Cfy*c, Elt *e);
static void ch_elt_D(Cfy*c, Elt *e);

typedef void (ch_elt)(Cfy*c,Elt*e);
ch_elt* ch_elt_p[] = { ch_elt_L, ch_elt_C, ch_elt_W, ch_elt_G, ch_elt_J,
		       ch_elt_N, ch_elt_F, ch_elt_R, ch_elt_X,
		       ch_elt_D };

static int in_cell;

void
cfy_out_html(Cfy *c)
{
  ch_head(c);
  ch_body(c);
  ch_foot(c);
}

static void
ch_head(Cfy *c)
{
  /* paradoxically, weboutput skips the html head/body because those
   are provided by P4 */
  if (!c->weboutput)
    {
      /*const char *cfyfam = cfy_fam(c->project);*//*REFACTOR WITH c->c->css*/
      fprintf(c->o,
	      "<html >"
	      "<head><meta charset=\"utf-8\"/>"
	      "<title>Cuneified %s</title>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/fonts.css\"/>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p4-cuneify.css\"/>",
	      c->n);
      if (c->c->css)
	fprintf(c->o,
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"/>",
		c->c->css);
      fputs("<script type=\"text/javascript\" src=\"/js/p4.js\">&#160;</script>"
	    "</head><body onload=\"p4Onload()\">", c->o);
    }
  fprintf(c->o,
	  "<div id=\"p4Cuneify\" "
	  "data-cfy-fnt=\"%s\" data-cfy-mag=\"%s\" data-cfy-scr=\"%s\" data-proj=\"%s\">",
	  c->c->fnt, c->c->mag, c->c->scr, c->project);
  fprintf(outfp,
	  "<h1 class=\"p3h2 border-top heading\">"
	  "<span class=\"cfy-generic\">Cuneified </span>"
	  "<span class=\"cfy-specific\">%s</span></h1><table class=\"cfy-table\">",
	  c->n);
}

static void
ch_body(Cfy *c)
{
#if 1
  int i, j;
  for (i = 0; c->elt_lines[i]; ++i)
    {
      ch_line_o(c, c->elt_lines[i]->epp[0]->data);
      for (j = 1; c->elt_lines[i]->epp[j]; ++j)
	ch_elt_p[c->elt_lines[i]->epp[j]->etype](c, c->elt_lines[i]->epp[j]);
      ch_line_c(c);
    }
#else
  List *lp;
  for (lp = list_first(c->body); lp; lp = list_next(lp))
    {
      Elt *ep = list_first(lp);
      ch_line_o(c, (Line *)ep);
      for (ep = list_next(lp); ep; ep = list_next(lp))
	ch_elt_p[ep->etype](c, ep);
      ch_line_c(c);
    }
#endif
}

static void
ch_line_o(Cfy *c, Line *l)
{
  fprintf(c->o,
	  "<tr id=\"cfy.%s\" data-label=\"%s\">",
	  l->xid, l->label); 
}

static void
ch_line_c(Cfy*c)
{
  if (in_cell)
    {
      fputs("</td>", c->o);
      in_cell = 0;
    }
  fputs("</tr>", c->o);
}

static void
ch_foot(Cfy *c)
{
  fprintf(c->o, "</table></div>");
  if (!c->weboutput)
    fprintf(c->o, "</body></html>");
}

/* ch_elt_? */

static void ch_elt_L(Cfy *c, Elt *e){} /* unused stub */

static void
ch_elt_C(Cfy *c, Elt *e)
{
  Cell *cp = e->data;
  if (in_cell)
    fputs("</td>", c->o);
  fprintf(c->o, "<td span=\"%d\">", cp->span);
  in_cell = 1;
}

static void
ch_elt_W(Cfy *c, Elt *e)
{
  fputc(' ', c->o);
}

static void
ch_elt_G(Cfy *c, Elt *e)
{
  fprintf(c->o, "<g u=\"%s\" o=\"%s\" r=\"%s\"", (ccp)e->data, e->oid, e->xid);
  if (e->btype)
    fprintf(c->o, " brk=\"%s\"", brk_str[e->btype]);
  fputs("/>", c->o);
}

static void
ch_elt_J(Cfy *c, Elt *e)
{
  fprintf(c->o, "<j/>");  
}

static void
ch_elt_N(Cfy *c, Elt *e)
{
  fprintf(c->o, "<n/>");  
}

static void
ch_elt_F(Cfy *c, Elt *e)
{
  fprintf(c->o, "<f/>");  
}

static void
ch_elt_R(Cfy *c, Elt *e)
{
  fprintf(c->o, "<r/>");  
}

static void
ch_elt_X(Cfy *c, Elt *e)
{
  fprintf(c->o, "<x r=\"%s\"", e->xid);
  if (e->btype)
    fprintf(c->o, " brk=\"%s\"", brk_str[e->btype]);
  fputs("/>", c->o);  
}

static void
ch_elt_D(Cfy *c, Elt *e)
{
  /* deleted so do nothing */;
}

