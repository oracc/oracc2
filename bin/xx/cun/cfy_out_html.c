#include <oraccsys.h>
#include "cfy.h"
#include "cfy_iterator.h"

static void ch_head(Cfy *c);
static void ch_foot(Cfy *c);

static void ch_elt_Hp(Cfy*c, Elt *e);
static void ch_elt_S(Cfy*c, Elt *e);
static void ch_elt_Sp(Cfy*c, Elt *e);
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
		       NULL, NULL, NULL, ch_elt_S, ch_elt_Sp, ch_elt_Hp };

static void ch_l_o(Cfy*c, Line *l);
static void ch_l_c(Cfy*c);
static void ch_h_o(Cfy*c);
static void ch_h_c(Cfy*c);
static void ch_c_o(Cfy*c, Cell *cp);
static void ch_c_c(Cfy*c);
static void ch_b_o(Cfy*c, Btype b);
static void ch_b_c(Cfy*c);
static void ch_label(Cfy *c, const char *l, int print);

Tagfuncs ch_tags = {
  .l_o=ch_l_o, .l_c=ch_l_c,
  .h_o=ch_h_o, .h_c=ch_h_c,
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
	      "<title>Cuneified %s</title>",
	      c->n);
      fputs("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/fonts.css\"/>"
	    "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p4-cuneify.css\"/>", c->o);
      if (html_css)
	fprintf(c->o,
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"/>",
		html_css);
      if (c->c->css)
	fprintf(c->o,
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"/>",
		c->c->css);
      fputs("<script type=\"text/javascript\" src=\"/js/p4.js\">&#160;</script>"
	    "</head><body onload=\"p4Onload()\">", c->o);
    }
  fprintf(c->o,
	  "<div id=\"p4Cuneify\" "
	  "data-cfy-fnt=\"%s\" data-cfy-mag=\"%s\" data-cfy-scr=\"%s\" data-proj=\"%s\"",
	  c->c->fnt, c->c->mag, c->c->scr, c->project);
  if (c->c->ffs)
    fprintf(c->o, " data-cfy-ffs=\"%s\"", c->c->ffs);
  fputc('>', c->o);
  char format_options[strlen(" cfy-boxed cfy-ruled cfy-crules0")];
  *format_options = '\0';
  if (c->c->rbox.e)
    strcat(format_options, " cfy-boxed");
  if (c->c->rline.e)
    strcat(format_options, " cfy-ruled");
  if (c->c->rcol.e)
    strcat(format_options, " cfy-crules");
  const char *justify = cfy_justify_class(c->c->justify);
  fprintf(c->o,
	  "<h1 class=\"p3h2 border-top heading\">"
	  "<span class=\"cfy-generic\">Cuneified </span>"
	  "<span class=\"cfy-specific\">%s</span></h1><table class=\"cfy-table%s%s\">",
	  c->n,
	  format_options,
	  justify);
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
ch_elt_S(Cfy *c, Elt *e)
{
  fprintf(c->o, "<span class=\"zws\"> </span>");
}

static void
ch_elt_Hp(Cfy *c, Elt *e)
{
  fputs("<span class=\"cfy-pseudo-h\"></span>", c->o);
}

static void
ch_elt_Sp(Cfy *c, Elt *e)
{
  const char *lw = "";
  if ((ELT_Jp == c->c->justify || ELT_Jcp == c->c->justify)
      && ci_j == ((Line*)c->elt_lines[ci_i]->epp[0]->data)->last_w)
    lw = " cfy-penult";
  fprintf(c->o, "<span class=\"cfy-pseudo-s%s\"></span>", lw);
}

static void
ch_elt_W(Cfy *c, Elt *e)
{
  if (!e->prev || e->prev->etype != ELT_R)
    {
      const char *lw = "";
      /*fprintf(stderr, "ch_elt_W processing epp[%d]\n", ci_j);*/
      if ((ELT_Jp == c->c->justify || ELT_Jcp == c->c->justify)
	  && ci_j == ((Line*)c->elt_lines[ci_i]->epp[0]->data)->last_w)
	lw = " cfy-penult";
      fprintf(c->o, "<span class=\"ws%s\"> </span>", lw);
    }
}

/* Try to map an LGS grapheme ID to a corresponding MTS word ID.
 *
 * No reasonable ID will be more than 22 characters including terminating '\0', i.e.,
 * P123456.99999.9999.99 (line.word.grapheme)
 */
static const char *
lgs_word(Cfy *c, const char *lgs_g)
{
  static char buf[32];
  strncpy(buf, lgs_g, 31);
  if (!buf[31]) /* reasonable ID */
    {
      char *dot = strrchr(buf, '.');
      if (dot)
	{
	  *dot = '\0';
	  const char *w = hash_find(c->hlgs, (uccp)buf);
	  return w ? w : lgs_g;
	}
    }
  return lgs_g;
}

static void
ch_elt_G(Cfy *c, Elt *e)
{
  if (!strcmp(e->data, "X"))
    {
      if (e->g_o || e->g_c)
	{
	  fputs("<span class=\"roman gray cfy-brack\"", c->o);
	  if (e->g_o)
	    fprintf(c->o, " data-bracko=\"%s\"", e->g_o);
	  if (e->g_c)
	    fprintf(c->o, " data-brackc=\"%s\"", e->g_c);
	  fputs(">X</span>", c->o);
	}
      else
	{
	  fputs("<span class=\"roman gray\">X</span>", c->o);
	}
    }
  else
    {
      const char *otf_str = " ";
      fprintf(c->o,
	      "<span id=\"c.%s\" data-ref=\"%s\" data-oid=\"%s\" data-asl=\"%s\""
	      " onclick=\"cfySL(event)\" oncontextmenu=\"cfyHi(event); return false;\"",
	      e->xid, c->hlgs ? lgs_word(c, e->xid) : e->xid, e->oid,
	      e->c->asl ? e->c->asl : ('9' == e->oid[2] ? "pcsl" : "osl"));
      if (e->title)
	fprintf(c->o, " title=\"%s\"", e->title);
      if (e->otf)
	{
	  if (isdigit(*e->otf))
	    {
	      static char saltbuf[8] = { ' ', 's', 'a', 'l', 't', '\0', '\0', '\0' };
	      saltbuf[5] = e->otf[0];
	      saltbuf[6] = e->otf[1];
	      otf_str = saltbuf;
	    }
	  else
	    {
	      static char cvbuf[6] = { ' ', 'c', 'v', '0', '0', '\0' };
	      strcpy(cvbuf+3,e->otf+2);
	      otf_str = cvbuf;
	    }
	}
      if (e->g_o || e->g_c)
	{
	  fprintf(c->o, " class=\"cfy-cun%scfy-brack\"", otf_str);
	  if (e->g_o)
	    fprintf(c->o, " data-bracko=\"%s\"", e->g_o);
	  if (e->g_c)
	    fprintf(c->o, " data-brackc=\"%s\"", e->g_c);
	}
      else
	fprintf(c->o, " class=\"cfy-cun%s\"", otf_str);
      fprintf(c->o, ">%s</span>", (ccp)e->data);
    }
}

static void
ch_elt_J(Cfy *c, Elt *e)
{
  fprintf(c->o, "<j/>");  
}

static void
ch_elt_N(Cfy *c, Elt *e)
{
  fprintf(c->o, "<span class=\"zwnj\">%s</span>", (char*)e->data);
}

static void
ch_elt_F(Cfy *c, Elt *e)
{
  fprintf(c->o, "<f/>");  
}

static void
ch_elt_R(Cfy *c, Elt *e)
{
  fputs("<span class=\"br\"/>", c->o);
}

static void
ch_elt_X(Cfy *c, Elt *e)
{
#if 1
  fprintf(stderr, "untrapped ch_elt_X\n");
  fprintf(c->o, "<span style=\"color: red\">UNTRAPPED ch_elt_X</span>");
#else
  fprintf(c->o, "<x r=\"%s\"", e->xid);
  if (e->btype)
    fprintf(c->o, " brk=\"%s\"", brk_str[e->btype]);
  fputs(">...</x>", c->o);
#endif
}

static void
ch_elt_E(Cfy *c, Elt *e)
{
  fputs("<span class=\"roman\">. . .</span>",c->o);
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
ch_h_o(Cfy *c)
{
  fputs("<th>", c->o);
}

static void
ch_h_c(Cfy *c)
{
  fputs("</th>", c->o);
}

static void
ch_c_o(Cfy *c, Cell *cp)
{
  if (cp && cp->class)
    fprintf(c->o, "<td colspan=\"%d\" class=\"%s\">", cp->span, cp->class);
  else if (cp)
    fprintf(c->o, "<td colspan=\"%d\">", cp->span);
  else
    fputs("<td>", c->o);
  fputs("<p>", c->o);
}

static void
ch_c_c(Cfy *c)
{
  fputs("</p></td>", c->o);
}

static void
ch_b_o(Cfy *c, Btype b)
{
  fprintf(c->o, "<span class=\"broken\">");
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
    fprintf(c->o, "<span class=\"cfy-lnum\">%s</span>", l);
}
