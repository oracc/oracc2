#include <oraccsys.h>
#include "cfy.h"

static FILE *o;

static void cx_head(Cfy *c);
static void cx_body(Cfy *c);
static void cx_foot(Cfy *c);
static void cx_line_o(Cfy *c, Line *l);
static void cx_line_c(Cfy *c);

static void cx_elt_L(Cfy*c, Elt *e);
static void cx_elt_W(Cfy*c, Elt *e);
static void cx_elt_G(Cfy*c, Elt *e);
static void cx_elt_J(Cfy*c, Elt *e);
static void cx_elt_N(Cfy*c, Elt *e);
static void cx_elt_F(Cfy*c, Elt *e);
static void cx_elt_R(Cfy*c, Elt *e);
static void cx_elt_X(Cfy*c, Elt *e);
static void cx_elt_D(Cfy*c, Elt *e);

typedef void (cx_elt)(Cfy*c,Elt*e);
cx_elt* cx_elt_p[] = { cx_elt_L, cx_elt_W, cx_elt_G, cx_elt_J,
		       cx_elt_N, cx_elt_F, cx_elt_R, cx_elt_X,
		       cx_elt_D };

void
cfy_out_xml(Cfy *c, FILE *fp)
{
  o = fp;
  cx_head(c);
  cx_body(c);
  cx_foot(c);
}

static void
cx_head(Cfy *c)
{
  fprintf(c->o,
	  "<cfy xmlns=\"http://oracc.org/ns/1.0/cuneify\" xml:id=\"cfy.%s\""
	  " n=\"%s\" project=\"%s\" id=\"%s\" fnt=\"%s\" key=\"%s\">",
	  c->pqx, c->n, c->project, c->pqx, c->fnt, c->key);
}

static void
cx_body(Cfy *c)
{
  List *lp;
  for (lp = list_first(c->body); lp; lp = list_next(lp))
    {
      Elt *ep = list_first(lp);
      cx_line_o(c, (Line *)ep);
      for (ep = list_next(lp); ep; ep = list_next(lp))
	cx_elt_p[ep->etype](c, ep);
      cx_line_c(c);
    }
}

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
  fputs("</l>", c->o);
}

static void
cx_foot(Cfy *c)
{
  fprintf(c->o, "</cfy>");
}

/* cx_elt_? */

static void cx_elt_L(Cfy *c, Elt *e){} /* unused stub */

static void
cx_elt_W(Cfy *c, Elt *e)
{
  fprintf(c->o, "<w/>");
}

static void
cx_elt_G(Cfy *c, Elt *e)
{
  fprintf(c->o, "<g u=\"%s\" o=\"%s\" r=\"%s\"", e->cun, e->oid, e->xid);
  if (e->btype)
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
cx_elt_X(Cfy *c, Elt *e)
{
  fprintf(c->o, "<x r=\"%s\"", e->xid);
  if (e->btype)
    fprintf(c->o, " brk=\"%s\"", brk_str[e->btype]);
  fputs("/>", c->o);  
}

static void
cx_elt_D(Cfy *c, Elt *e)
{
  /* deleted so do nothing */;
}

