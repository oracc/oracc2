#include <oraccsys.h>
#include "cfy.h"

static void cx_head(Cfy *c);
static void cx_body(Cfy *c);
static void cx_foot(Cfy *c);
static void cx_line_o(Cfy *c, Line *l);
static void cx_line_c(Cfy *c);

static void cx_elt_NOT(Cfy*c, Elt *e);
static void cx_elt_L(Cfy*c, Elt *e);
static void cx_elt_C(Cfy*c, Elt *e);
static void cx_elt_W(Cfy*c, Elt *e);
static void cx_elt_G(Cfy*c, Elt *e);
static void cx_elt_J(Cfy*c, Elt *e);
static void cx_elt_N(Cfy*c, Elt *e);
static void cx_elt_F(Cfy*c, Elt *e);
static void cx_elt_R(Cfy*c, Elt *e);
static void cx_elt_E(Cfy*c, Elt *e);
static void cx_elt_X(Cfy*c, Elt *e);
static void cx_elt_D(Cfy*c, Elt *e);

typedef void (cx_elt)(Cfy*c,Elt*e);
cx_elt* cx_elt_p[] = { cx_elt_NOT,
		       cx_elt_L, cx_elt_C, cx_elt_W, cx_elt_G,
		       cx_elt_J, cx_elt_N, cx_elt_F, cx_elt_R,
		       cx_elt_E, cx_elt_X, cx_elt_D };

static int in_cell;

static Btype last_b, curr_b;

void
cx_b_switch(Cfy *c, Btype this_b)
{
  if (brk_str[last_b])
    fputs("</b>", c->o);
  if (brk_str[this_b])
    fprintf(c->o, "<b brk=\"%s\">", brk_str[this_b]);
  last_b = curr_b = this_b;
}

/* if reset==0 don't reset curr_b; this is used when closing a
   cell because then we are just suspending the breakage type to
   balance the XML output */
void
cx_b_closer(Cfy *c, int reset)
{
  fputs("</b>", c->o);
  if (reset)
    last_b = curr_b = BRK_NONE;
  else
    last_b = BRK_NONE;
}

void
cfy_out_xml(Cfy *c)
{
  cx_head(c);
  cx_body(c);
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
cx_body(Cfy *c)
{
  int i, j;
  for (i = 0; c->elt_lines[i]; ++i)
    {
      cx_line_o(c, c->elt_lines[i]->epp[0]->data);
      for (j = 1; c->elt_lines[i]->epp[j]; ++j)
	{
	  if (last_b != c->elt_lines[i]->epp[j]->btype)
	    cx_b_switch(c, c->elt_lines[i]->epp[j]->btype);
	  cx_elt_p[c->elt_lines[i]->epp[j]->etype](c, c->elt_lines[i]->epp[j]);
	}
      if (brk_str[last_b])
	cx_b_closer(c, 1);
      cx_line_c(c);
    }
}

static void cx_elt_NOT(Cfy *c, Elt *e){} /* unused stub */

static void
cx_line_o(Cfy *c, Line *l)
{
  fprintf(c->o,
	  "<l xml:id=\"cfy.%s\" label=\"%s\">",
	  l->xid, l->label);
}

static void
cx_line_c(Cfy*c)
{
  if (in_cell)
    {
      if (brk_str[last_b])
	cx_b_closer(c, 0);
      fputs("</c>", c->o);
      in_cell = 0;
    }
  fputs("</l>", c->o);
}

static void
cx_foot(Cfy *c)
{
  fprintf(c->o, "</cfy>");
}

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

static void cx_elt_L(Cfy *c, Elt *e){} /* unused stub */

static void
cx_elt_C(Cfy *c, Elt *e)
{
  Cell *cp = e->data;
  if (in_cell)
    {
      if (brk_str[last_b])
	cx_b_closer(c, 0);
      fputs("</c>", c->o);
    }
  fprintf(c->o, "<c span=\"%d\">", cp->span);
  if (last_b != curr_b)
    cx_b_switch(c, curr_b);
  in_cell = 1;
}

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
cx_elt_D(Cfy *c, Elt *e)
{
  /* deleted so do nothing */;
}

