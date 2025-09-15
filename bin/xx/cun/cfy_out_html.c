#include <oraccsys.h>
#include "cfy.h"
#include "cfy_iterator.h"

static void ch_head(Cfy *c);
static void ch_foot(Cfy *c);

static void ch_elt_W(Cfy*c, Elt *e);
static void ch_elt_G(Cfy*c, Elt *e);
static void ch_elt_J(Cfy*c, Elt *e);
static void ch_elt_N(Cfy*c, Elt *e);
static void ch_elt_F(Cfy*c, Elt *e);
static void ch_elt_R(Cfy*c, Elt *e);
static void ch_elt_E(Cfy*c, Elt *e);
static void ch_elt_X(Cfy*c, Elt *e);

ci_elt* ch_elt_p[] = { NULL, NULL,
		       ci_cell_o,
		       ch_elt_W, ch_elt_G, ch_elt_J, ch_elt_N,
		       ch_elt_F, ch_elt_R, ch_elt_E, ch_elt_X,
		       NULL };

static void ch_l_o(Cfy*c, Line *l);
static void ch_l_c(Cfy*c);
static void ch_c_o(Cfy*c, Cell *cp);
static void ch_c_c(Cfy*c);
static void ch_b_o(Cfy*c, Btype b);
static void ch_b_c(Cfy*c);
static void ch_label(Cfy *c, const char *l, int print);

Tagfuncs ch_tags = {
  .l_o=ch_l_o, .l_c=ch_l_c,
  .c_o=ch_c_o, .c_c=ch_c_c,
  .b_o=ch_b_o, .b_c=ch_b_c,
  .l=ch_label
};

void
cfy_out_html(Cfy *c)
{
  ci_elt_p = ch_elt_p;
  ci_tags = &ch_tags;
  ch_head(c);
  ci_body(c);
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
ch_foot(Cfy *c)
{
  fprintf(c->o, "</table></div>");
  if (!c->weboutput)
    fprintf(c->o, "</body></html>");
}

/* ch_elt_? */

static void
ch_elt_W(Cfy *c, Elt *e)
{
  fputs("<span class=\"ws\"> </span>", c->o);
}

static void
ch_elt_G(Cfy *c, Elt *e)
{
  fprintf(c->o, "<a href=\"javascript://\" onhover=\"p4-cfy-grapheme(evt)\" data-oid=\"%s\" data-ref=\"%s\">%s</a>",
	  e->oid, e->xid, (ccp)e->data);
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
  fputs(">...</x>", c->o);  
}

static void
ch_elt_E(Cfy *c, Elt *e)
{
  fprintf(c->o, "<x r=\"%s\"", e->xid);
  if (e->btype)
    fprintf(c->o, " brk=\"%s\"", brk_str[e->btype]);
  fputs("/>", c->o);  
}

static void
ch_l_o(Cfy *c, Line *l)
{
  fprintf(c->o,
	  "<tr id=\"cfy.%s\" data-label=\"%s\">",
	  l->xid, l->label);
}

static void
ch_l_c(Cfy *c)
{
  fputs("</tr>", c->o);
}

static void
ch_c_o(Cfy *c, Cell *cp)
{
  fprintf(c->o, "<td colspan=\"%d\">", cp->span);
}

static void
ch_c_c(Cfy *c)
{
  fputs("</td>", c->o);
}

static void
ch_b_o(Cfy *c, Btype b)
{
  fprintf(c->o, "<span class=\"%s\">", brk_str[b]);
}

static void
ch_b_c(Cfy *c)
{
  fputs("</span>", c->o);
}

static void
ch_label(Cfy *c, const char *l, int print)
{
  if (print)
    fprintf(c->o, "<span class=\"lnum\">%s</span>", l);
}
