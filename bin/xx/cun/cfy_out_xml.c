#include <oraccsys.h>
#include "cfy.h"

static FILE *o;

extern void cx_head(Cfy *c);
extern void cx_body(Cfy *c);
extern void cx_foot(Cfy *c);
extern void cx_line(Cfy *c, Line *l);
extern void cx_elt(Cfy *c, Elt *e);

void
cfy_out_xml(Cfy *c, FILE *fp)
{
  o = fp;
  cx_head(c);
  cx_body(c);
  cx_foot(c);
}

void
cx_head(Cfy *c)
{
  fprintf(o, "<cfy xmlns=\"http://oracc.org/ns/1.0/cuneify\">");
}

void
cx_body(Cfy *c)
{
  List *lp;
  for (lp = list_first(c->body); lp; lp = list_next(lp))
    {
      Elt *ep = list_first(lp);
      cx_line(c, (Line *)ep->u8);
      for (ep = list_next(lp); ep; ep = list_next(lp))
	cx_elt(c, ep);
    }
}

void
cx_line(Cfy *c, Line *l)
{
}

void
cx_elt(Cfy *c, Elt *e)
{
}

void
cx_foot(Cfy *c)
{
  fprintf(o, "</cfy>");
}
