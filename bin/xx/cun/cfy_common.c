#include <oraccsys.h>
#include "cfy.h"

typedef void (cc_elt)(Cfy*c,Elt*e);
typedef void (cc_tag)(Cfy*c,void*v);

cc_elt**cc_elt_p;
cc_tag**cc_tag_p;

static Btype last_b, curr_b;
static int in_cell;
const char *brk_str[] = { NULL, NULL , "#", "[]" };

void
cc_body(Cfy *c)
{
  int i, j;
  for (i = 0; c->elt_lines[i]; ++i)
    {
      cc_line_o(c, c->elt_lines[i]->epp[0]->data);
      for (j = 1; c->elt_lines[i]->epp[j]; ++j)
	{
	  if (last_b != c->elt_lines[i]->epp[j]->btype)
	    cc_b_switch(c, c->elt_lines[i]->epp[j]->btype);
	  if (cc_elt_p[c->elt_lines[i]->epp[j]->etype])
	    cc_elt_p[c->elt_lines[i]->epp[j]->etype](c, c->elt_lines[i]->epp[j]);
	}
      if (brk_str[last_b])
	cc_b_closer(c, 1);
      cc_line_c(c);
    }
}

void
cc_b_switch(Cfy *c, Btype this_b)
{
  if (brk_str[last_b])
    cc_tags->b_c(c);
  if (brk_str[this_b])
    cc_tags->b_o(c, this_b);
  last_b = curr_b = this_b;
}

/* if reset==0 don't reset curr_b; this is used when closing a
   cell because then we are just suspending the breakage type to
   balance the XML output */
void
cc_b_closer(Cfy *c, void *reset)
{
  cc_tags->b_c(c);
  if (reset)
    last_b = curr_b = BRK_NONE;
  else
    last_b = BRK_NONE;
}

void
cc_line_o(Cfy *c, Line*l)
{
  cc_tags->l_o(c, l);
}

void
cc_line_c(Cfy*c)
{
  if (in_cell)
    {
      if (brk_str[last_b])
	cc_tags->b_c(c);
      cc_tags->c_c(c);
      in_cell = 0;
    }
  cc_tags->l_c(c);
}

void
cc_cell_o(Cfy *c, Elt *e)
{
  Cell *cp = e->data;
  if (in_cell)
    {
      if (brk_str[last_b])
	cx_b_closer(c, 0);
      cc_tags->c_c;
    }
  cc_tags->c_o(c,cp);
  if (last_b != curr_b)
    cx_b_switch(c, curr_b);
  in_cell = 1;
}

