#include <oraccsys.h>
#include "cfy.h"
#include "cfy_iterator.h"

int ci_i, ci_j;

ci_elt**ci_elt_p;
Tagfuncs *ci_tags;

int line_cell_pending = 0;

static Btype last_b, curr_b;
static int in_cell;
const char *brk_str[] = { NULL, "#", "[]" };

static void ci_line_o(Cfy *c, Line*l);
static void ci_line_c(Cfy*c);
static void ci_b_switch(Cfy *c, Btype this_b);
static void ci_b_closer(Cfy *c, void *reset);

void
ci_body(Cfy *c)
{
  tree_xml(c->o, c->body); 
}

void
ci_div(Cfy *c, Div *dp)
{
  static Cell content_cell = { 1, "cfy-content" };
  static Elt content_elt = { .data = &content_cell };
  curr_div = dp;
  ci_tags->d_o(c);
  for (ci_i = 0; dp->elt_lines[ci_i]; ++ci_i)
    {
      Elt *epp_curr;
      for (ci_j = 0; (epp_curr = dp->elt_lines[ci_i]->epp[ci_j]); ++ci_j)
	{
	  switch (epp_curr->etype)
	    {
	    case ELT_L:
	      ci_line_o(c, epp_curr->data);
	      break;
	    case ELT_C:
	      ci_cell_o(c, epp_curr);
	      if (last_b != epp_curr->btype)
		ci_b_switch(c, epp_curr->btype);
	    case ELT_H:
	      {
		Cell hcell = { 3, NULL};
		ci_tags->l_o(c, NULL);
		ci_tags->c_o(c, &hcell);
		ci_elt_p[epp_curr->etype](c, epp_curr);
		ci_tags->c_c(c);
	      }
	      break;
	    default:
	      if (ci_elt_p[epp_curr->etype] && line_cell_pending)
		ci_cell_o(c, &content_elt);
	      if (last_b != epp_curr->btype)
		ci_b_switch(c, epp_curr->btype);
	      if (ci_elt_p[epp_curr->etype])
		ci_elt_p[epp_curr->etype](c, epp_curr);
	      break;
	    }
	}
      if (brk_str[last_b])
	ci_b_closer(c, "");
      ci_line_c(c);
    }
  ci_tags->d_c(c);
}

static void
ci_b_switch(Cfy *c, Btype this_b)
{
  if (brk_str[last_b])
    ci_tags->b_c(c);
  if (brk_str[this_b])
    ci_tags->b_o(c, this_b);
  last_b = curr_b = this_b;
}

/* if reset==0 don't reset curr_b; this is used when closing a
   cell because then we are just suspending the breakage type to
   balance the XML output */
static void
ci_b_closer(Cfy *c, void *reset)
{
  ci_tags->b_c(c);
  if (reset)
    last_b = curr_b = BRK_NONE;
  else
    last_b = BRK_NONE;
}

static int
ci_printable_label(const char *l)
{
  const char *sp = strrchr(l, ' ');
  if (sp)
    ++sp;
  else
    sp = l;
  if ('1' == *sp && !isdigit(sp[1]))
    return 1;
  else
    return !(atoi(sp) % 5);      
}

static void
ci_line_o(Cfy *c, Line*l)
{
  /*static Cell label_cell = { 1 , "cfy-label" };*/
  ci_tags->l_o(c, l);
  ci_tags->h_o(c /*, &label_cell*/);
  ci_tags->l(c, l->label, ci_printable_label(l->label));
  ci_tags->h_c(c);
  line_cell_pending = 1;
}

static void
ci_line_c(Cfy*c)
{
  if (in_cell)
    {
      if (brk_str[last_b])
	ci_tags->b_c(c);
      ci_tags->c_c(c);
      in_cell = 0;
    }
  ci_tags->l_c(c);
}

void
ci_cell_o(Cfy *c, Elt *e)
{
  Cell *cp = e ? e->data : NULL;
  if (in_cell)
    {
      if (brk_str[last_b])
	ci_b_closer(c, NULL);
      ci_tags->c_c(c);
    }
  ci_tags->c_o(c,cp);
  if (last_b != curr_b)
    ci_b_switch(c, curr_b);
  in_cell = 1;
  line_cell_pending = 0;
}
