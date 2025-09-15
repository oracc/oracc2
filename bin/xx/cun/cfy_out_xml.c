#include <oraccsys.h>
#include "cfy.h"
#include "cfy_common.h"

static void cx_head(Cfy *c);
static void cx_foot(Cfy *c);

static void cx_elt_W(Cfy*c, Elt *e);
static void cx_elt_G(Cfy*c, Elt *e);
static void cx_elt_J(Cfy*c, Elt *e);
static void cx_elt_N(Cfy*c, Elt *e);
static void cx_elt_F(Cfy*c, Elt *e);
static void cx_elt_R(Cfy*c, Elt *e);
static void cx_elt_E(Cfy*c, Elt *e);
static void cx_elt_X(Cfy*c, Elt *e);

cc_elt* cx_elt_p[] = { NULL, NULL,
		       cc_cell_o,
		       cx_elt_W, cx_elt_G, cx_elt_J, cx_elt_N,
		       cx_elt_F, cx_elt_R, cx_elt_E, cx_elt_X,
		       NULL };

static void cx_l_o(Cfy*c, Line *l);
static void cx_l_c(Cfy*c);
static void cx_c_o(Cfy*c, Cell *cp);
static void cx_c_c(Cfy*c);
static void cx_b_o(Cfy*c, Btype b);
static void cx_b_c(Cfy*c);

Tagfuncs cx_tags = {
  .l_o=cx_l_o, .l_c=cx_l_c,
  .c_o=cx_c_o, .c_c=cx_c_c,
  .b_o=cx_b_o, .b_c=cx_b_c
};

void
cfy_out_xml(Cfy *c)
{
  cc_elt_p = cx_elt_p;
  cc_tags = &cx_tags;
  cx_head(c);
  cc_body(c);
  cx_foot(c);
}

static void
cx_head(Cfy *c)
{
  fprintf(c->o,
	  "<cfy xmlns=\"http://oracc.org/ns/1.0/cuneify\" xml:id=\"cfy.%s\""
	  " xmlns:cfy=\"http://oracc.org/ns/1.0/cuneify\""
	  " n=\"%s\" project=\"%s\" id=\"%s\" cfy:fnt=\"%s\" cfy:key=\"%s\">",
	  c->pqx, c->n, c->project, c->pqx, c->fnt, c->key);
}

static void
cx_foot(Cfy *c)
{
  fprintf(c->o, "</cfy>");
}

/* This is not needed in the common output functions because it is
   effectively a status note on each node which is used for
   debugging */
static const char *
cx_breakage(Elt *e)
{
  static char buf[10];
  if (brk_str[e->btype])
    sprintf(buf, " brk=\"%s\"", brk_str[e->btype]);
  else
    *buf = '\0';
  return buf;
}

/* cx_elt_? */

static void
cx_elt_W(Cfy *c, Elt *e)
{
  fprintf(c->o, "<w%s/>", cx_breakage(e));
}

static void
cx_elt_G(Cfy *c, Elt *e)
{
  fprintf(c->o, "<g u=\"%s\" o=\"%s\" r=\"%s\"", (uccp)e->data, e->oid, e->xid);
  if (brk_str[e->btype])
    fprintf(c->o, " brk=\"%s\"", brk_str[e->btype]);
  fputs("/>", c->o);
}

static void
cx_elt_J(Cfy *c, Elt *e)
{
  fprintf(c->o, "<j/>");  
}

static void
cx_elt_N(Cfy *c, Elt *e)
{
  fprintf(c->o, "<n/>");  
}

static void
cx_elt_F(Cfy *c, Elt *e)
{
  fprintf(c->o, "<f/>");  
}

static void
cx_elt_R(Cfy *c, Elt *e)
{
  fprintf(c->o, "<r/>");  
}

static void
cx_elt_E(Cfy *c, Elt *e)
{
  fprintf(c->o, "<x r=\"%s\"%s", e->xid, cx_breakage(e));
  fputs(">...</x>", c->o);  
}

static void
cx_elt_X(Cfy *c, Elt *e)
{
  fprintf(c->o, "<x r=\"%s\"%s", e->xid, cx_breakage(e));
  fputs("/>", c->o);  
}

static void
cx_l_o(Cfy *c, Line *l)
{
  fprintf(c->o,
	  "<l xml:id=\"cfy.%s\" label=\"%s\">",
	  l->xid, l->label);
}

static void
cx_l_c(Cfy *c)
{
  fputs("</l>", c->o);
}

static void
cx_c_o(Cfy *c, Cell *cp)
{
  fprintf(c->o, "<c span=\"%d\">", cp->span);
}

static void
cx_c_c(Cfy *c)
{
  fputs("</c>", c->o);
}

static void
cx_b_o(Cfy *c, Btype b)
{
  fprintf(c->o, "<b brk=\"%s\">", brk_str[b]);
}

static void
cx_b_c(Cfy *c)
{
  fputs("</b>", c->o);
}

