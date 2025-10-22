#include <oraccsys.h>
#include "cfy.h"
#include "cfy_iterator.h"

extern const char *pi_file;

static int dp_o_o, dp_s_o, dp_c_o;
static int need_wrapper;

static void ch_head(Cfy *c);
static void ch_foot(Cfy *c);

static void ch_elt_H(Cfy*c, Elt *e);
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

ci_elt* ch_elt_p[] = { NULL, ch_elt_H, NULL,
		       ci_cell_o,
		       ch_elt_W, ch_elt_G, ch_elt_J, ch_elt_N,
		       ch_elt_F, ch_elt_R, ch_elt_E, ch_elt_X,
		       NULL, NULL, NULL, ch_elt_S, ch_elt_Sp, ch_elt_Hp,
};

static void ch_l_o(Cfy*c, Line *l);
static void ch_l_c(Cfy*c);
static void ch_h_o(Cfy*c);
static void ch_h_c(Cfy*c);
static void ch_c_o(Cfy*c, Cell *cp);
static void ch_c_c(Cfy*c);
static void ch_b_o(Cfy*c, Btype b);
static void ch_b_c(Cfy*c);
static void ch_d_o(Cfy*c);
static void ch_d_c(Cfy*c);
static void ch_label(Cfy *c, const char *l, int print);

Tagfuncs ch_tags = {
  .l_o=ch_l_o, .l_c=ch_l_c,
  .h_o=ch_h_o, .h_c=ch_h_c,
  .c_o=ch_c_o, .c_c=ch_c_c,
  .b_o=ch_b_o, .b_c=ch_b_c,
  .d_o=ch_d_o, .d_c=ch_d_c,
  .l=ch_label
};

/* the user hook is called after the <div> is generated but we want to
   switch the <h1> up before the <div> */
void
treexml_o_cfy(Node *np, void *user)
{
  Div *dp = np->user;

  /* If the div level is a grid, output the wrapper and set the
     grid_open flag for the level */
  if (dp->c->need_style)
    {
      if (
	  (!strcmp(dp->name, "object") && dp->c->c->grid.o && !dp_o_o)
	  || (!strcmp(dp->name, "surface") && dp->c->c->grid.s && !dp_s_o)
	  || (!strcmp(dp->name, "column") && dp->c->c->grid.c && !dp_c_o)
	  )
	{
	  fprintf(dp->c->o, "<div class=\"%s-flex\"", dp->name);
	  fputs(" style=\"display: flex; align-items: flex-start;\"", dp->c->o);
	  fputs(">", dp->c->o);
	  switch (*dp->name)
	    {
	    case 'o': dp_o_o = 1; break;
	    case 's': dp_s_o = 1; break;
	    case 'c': dp_c_o = 1; break;
	    }
	}
    }

  treexml_o_generic(np, user);  
}

void
treexml_c_cfy(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
  Div *dp = np->user;
  if (dp->b == XB_OBJ || dp->b == XB_SRF || dp->b == XB_COL)
    {
      if ((dp_c_o && strcmp(dp->name, "column"))
	  || (!dp_c_o && dp_s_o && strcmp(dp->name, "surface"))
	  || (!dp_c_o && !dp_s_o && dp_o_o && strcmp(dp->name, "object")))
	{
	  fputs("</div>", dp->c->o);
	  switch (*dp->name)
	    {
	    case 'o': dp_o_o = 0; break;
	    case 's': dp_s_o = 0; break;
	    case 'c': dp_c_o = 0; break;
	    }
	}
    }
}

static void
cfy_out_html_attr(Node *np, void *user)
{
  Div *dp = np->user;
  fprintf(dp->c->o, " class=\"%s\" n=\"%s\"", dp->name, dp->text);
}

static void
cfy_out_html_user(Node *np, void *user)
{
  Div *dp = np->user;

  if (dp->head)
    ch_elt_H(dp->c, dp->head);

  if (dp->elt_lines)
    ci_div(dp->c, dp);
}

void
cfy_out_html_config(void)
{
  ci_elt_p = ch_elt_p;
  ci_tags = &ch_tags;
  nodeh_register(treexml_a_handlers, NS_CFY, (nodehandler)cfy_out_html_attr);
  nodeh_register(treexml_u_handlers, NS_CFY, (nodehandler)cfy_out_html_user);
}

void
cfy_out_html(Cfy *c)
{
  cfy_out_html_config();
  ch_head(c);
  ci_body(c);
  ch_foot(c);
}

static void
ch_ruledata_sub(Cfy *c, const char *ch, Rule *rp)
{
  if (rp->w)
    fprintf(c->o, " data-r%s-w=\"%s\"", ch, rp->w);
  if (rp->c)
    fprintf(c->o, " data-r%s-c=\"%s\"", ch, rp->c);
  if (rp->s)
    fprintf(c->o, " data-r%s-s=\"%s\"", ch, rp->s);
}

static void
ch_ruledata(Cfy *c)
{
  ch_ruledata_sub(c, "b", &c->c->rbox);
  ch_ruledata_sub(c, "l", &c->c->rline);
  ch_ruledata_sub(c, "c", &c->c->rcol);
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
#if 0
      if (c->need_style)
	html_style(c);
#endif
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
  if (c->c->ffs && strcmp(c->c->ffs, "0"))
    fprintf(c->o, " data-cfy-ffs=\"%s\"", c->c->ffs);
  if (c->c->ruledata)
    ch_ruledata(c);
  if (c->c->width)
    fprintf(c->o, " data-cfy-width=\"%s\"", c->c->width);
  if (c->c->colwidths)
    fprintf(c->o, " data-cfy-colwidths=\"%s\"", c->c->colwidths);
  fputc('>', c->o);
  if (!cfy.bare)
    fprintf(c->o,
	    "<h1 class=\"p3h2 border-top heading\">"
	    "<span class=\"cfy-generic\">Cuneified </span>"
	    "<span class=\"cfy-specific\">%s</span></h1>", c->n);
}

static void
ch_foot(Cfy *c)
{
  if (!c->weboutput)
    fprintf(c->o, "</div></body></html>");
}

static void
ch_d_o(Cfy *c)
{
  char format_options[strlen(" cfy-boxed cfy-ruled cfy-crule0")];
  *format_options = '\0';
  if (c->c->rbox.e)
    strcat(format_options, " cfy-boxed");
  if (c->c->rline.e)
    strcat(format_options, " cfy-ruled");
  if (c->c->rcol.e)
    strcat(format_options, " cfy-crule");
  const char *justify = cfy_justify_class(c->c->justify);
  fprintf(c->o, "<table class=\"cfy-table%s%s\">",
	  format_options,
	  justify);
}
static void
ch_d_c(Cfy *c)
{
  fputs("</table>", c->o);
#if 0
  if (need_wrapper)
    {
      fputs("</div>", c->o);
      need_wrapper = 0;
    }
#endif
}

/* ch_elt_? */

static void
ch_elt_S(Cfy *c, Elt *e)
{
  fprintf(c->o, "<span class=\"zws\"> </span>");
}

/* If a transliteration uses @h1 or @h2 etc, @h1 should be at the
   start of a block because they are output as <h1> outside the table;
   @h2 etc., are output as <tr> within the transliteration */
static void
ch_elt_H(Cfy *c, Elt *e)
{
  Heading *hp = e->data;
  if (hp->level == 1)
    {
      /* <div class=\"cfy-wrapper\"> */
      fprintf(c->o, "<h1 class=\"cfy-heading\">%s</h1>", hp->text);
#if 0
      need_wrapper = 1;
#endif
    }
  else
    fprintf(c->o, "<span class=\"cfy-heading\">%s</span>", hp->text);
}

static void
ch_elt_Hp(Cfy *c, Elt *e)
{
  if (!e->indented)
    fputs("<span class=\"cfy-pseudo-h\"></span>", c->o);
}

static void
ch_elt_Sp(Cfy *c, Elt *e)
{
  if (!e->indented)
    {
      const char *lw = "";
      if ((ELT_Jp == c->c->justify || ELT_Jcp == c->c->justify)
	  && ci_j == ((Line*)curr_div->elt_lines[ci_i]->epp[0]->data)->last_w)
	lw = " cfy-penult";
      fprintf(c->o, "<span class=\"cfy-pseudo-s%s\"></span>", lw);
    }
}

static void
ch_elt_W(Cfy *c, Elt *e)
{
  if (!e->prev || e->prev->etype != ELT_R)
    {
      const char *lw = "";
      /*fprintf(stderr, "ch_elt_W processing epp[%d]\n", ci_j);*/
      if ((ELT_Jp == c->c->justify || ELT_Jcp == c->c->justify)
	  && ci_j == ((Line*)curr_div->elt_lines[ci_i]->epp[0]->data)->last_w)
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

/* Take care not to emit a class attribute that has both salt/cvNN and
   a font-feature-setting from the cfy key, e.g., ss01. This is so
   that fonts.css can contain all of the ffs stuff without it
   conflicting */
static const char *
ch_cun_class(Cfy *c, Elt *e)
{
  char *otf_str = "";
  static char *buf = NULL;
  static int buf_len = 0;

  if (!c)
    {
      free(buf);
      buf = NULL;
      buf_len = 0;
      return NULL;
    }
  
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

  if (e->c && e->c != c->c)
    {
      const char *o = (*otf_str ? otf_str : (!strcmp(e->c->ffs, "0") ? "" : e->c->ffs));
      if (!o)
	o = "";
      int need_len = strlen("ofs- -ofs--0")+strlen(e->c->fnt)+strlen(o)+strlen(e->c->mag);
      if (need_len >= buf_len)
	{
	  buf = realloc(buf, need_len *2);
	  buf_len = need_len * 2;
	}
      sprintf(buf, "ofs-%s%s%s ofs-%s", e->c->fnt, (o && *o)? " " : "", o, e->c->mag);
    }
  else
    {
      const char *ffs = strlen(otf_str) ? otf_str : "cfy-ffs";
      int need_len = strlen("cfy-cun") + strlen(ffs);
      if (need_len >= buf_len)
	{
	  buf = realloc(buf, need_len * 2);
	  buf_len = need_len * 2;
	}
      sprintf(buf, "cfy-cun %s", ffs);
    }

  return buf;
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
      const char *asl =
	e->c->asl
	? ('.' == *e->c->asl ? c->project : e->c->asl)
	: ('9' == e->oid[2] ? "pcsl" : "osl");
      fprintf(c->o,
	      "<span id=\"c.%s\" data-ref=\"%s\" data-oid=\"%s\" data-asl=\"%s\""
	      " onclick=\"cfySL(event)\" oncontextmenu=\"cfyHi(event); return false;\"",
	      e->xid, c->hlgs ? lgs_word(c, e->xid) : e->xid, e->oid, asl);
      if (e->title)
	fprintf(c->o, " title=\"%s\"", e->title);
      const char *cun_class = ch_cun_class(c, e);
      if (e->g_o || e->g_c)
	{
	  fprintf(c->o, " class=\"%s cfy-brack\"", cun_class);
	  if (e->g_o)
	    fprintf(c->o, " data-bracko=\"%s\"", e->g_o);
	  if (e->g_c)
	    fprintf(c->o, " data-brackc=\"%s\"", e->g_c);
	}
      else
	fprintf(c->o, " class=\"%s\"", cun_class);
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
  if (!strcmp(e->atype, "dollar"))
    /* FIXME: should handle Amarna red dots coded as ($r$) [if Jake uses that] */;
  else
    {
      fprintf(stderr, "%s:%d: untrapped ch_elt_X\n", pi_file, e->line);
      fprintf(c->o, "<span style=\"color: red\">UNTRAPPED ch_elt_X</span>");
    }
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
  ch_c_o(NULL,NULL);
  if (l)
    fprintf(c->o,
	    "<tr id=\"cfy.%s\" data-label=\"%s\">",
	    l->xid, l->label);
  else
    fprintf(c->o, "<tr class=\"cfy-heading\">");
}

static void
ch_l_c(Cfy *c)
{
  /*if (!need_wrapper)*/
    fputs("</tr>", c->o);
}

static void
ch_h_o(Cfy *c)
{
  fputs("<th class=\"td00\">", c->o);
}

static void
ch_h_c(Cfy *c)
{
  fputs("</th>", c->o);
}

static void
ch_c_o(Cfy *c, Cell *cp)
{
  static int nth;

  if (!c)
    {
      nth=0;
      return;
    }
  
  if (cp && cp->class)
    fprintf(c->o, "<td colspan=\"%d\" class=\"cfy-content %s td%02d\">", cp->span, cp->class, ++nth);
  else if (cp)
    fprintf(c->o, "<td colspan=\"%d\" class=\"cfy-content td%02d\">", cp->span, ++nth);
  else
    fprintf(c->o, "<td class=\"td%02d\">", ++nth);
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
