/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: indexed.c,v 0.3 1997/09/08 14:50:15 sjt Exp s $
*/
#include "index.h"
#include "property.h"

struct indexed *curr_indexed;
Memo *indexed_mm;
static int indexed_id;

extern int swc_flag;

void
begin_indexed ()
{
  if (curr_indexed)
    {
      curr_indexed->next = memo_new (indexed_mm);
      curr_indexed = curr_indexed->next;
    }
  else
    {
      curr_indexed = memo_new (indexed_mm);
      curr_indexed->next = NULL;
    }
  curr_node = curr_indexed->nodes = new_node ();
  curr_node->used_flag = 0;
  curr_indexed->id = indexed_id++;
  /* split-word support just won't work when there are multiple
   * exemplars, i.e., multiple branches 
   */
  curr_node->l.branch_id = branch_id = 1;
  if (!swc_flag)
    start_column = 0;
}

void
end_indexed ()
{
  struct grapheme *gnp;
  struct parallel *pp;

  for (gnp = grapheme_list_base; gnp; gnp = gnp->next)
    {
      /* this is an empty index-material entry */
      if (!gnp->node)
	continue;

      if (gnp->node->end_of_branch_flag)
	{
	  for (pp = gnp->node->parallel_info; pp && pp->parent; pp = pp->parent)
	    ;
	  gnp->node->l.end_column = pp->end_column;
	  if (!gnp->node->boundary && gnp->node->parallel_info->boundary)
	    gnp->node->boundary = gnp->node->parallel_info->boundary;
	}
      else
	{
	  int logo = is_logo(gnp->node->l.end_column);
	  gnp->node->l.end_column = mask_sc(gnp->node->l.start_column);
	  if (logo)
	    gnp->node->l.end_column |= G_LOGO;
	}
      /* the left/right tests in prop setting are for when a sign
	 is double-indexed, e.g., as be2 and BI */
      if (NULL == gnp->node->left
	  || (gnp->node->left->l.start_column == gnp->node->l.start_column
	      && NULL == gnp->node->left->left))
	{
	  set_property (gnp->node->l.properties, PROP_LB);
	  set_property (gnp->node->l.properties, PROP_WB);
	}
      else if (gnp->node->left->boundary == ' ')
	set_property (gnp->node->l.properties, PROP_WB);

      if (NULL == gnp->node->right
	  || (gnp->node->right->l.start_column == gnp->node->l.start_column
	      && NULL == gnp->node->right->right))
	{
	  set_property (gnp->node->l.properties, PROP_LE);
	  set_property (gnp->node->l.properties, PROP_WE);
	}
      else if (gnp->node->boundary == ' '
	       || (gnp->node->right->l.start_column == gnp->node->l.start_column
		   && gnp->node->right->boundary == ' '))
	  set_property (gnp->node->l.properties, PROP_WE);
    }
#if 0
  if (debug_flag)
    dump_graphemes ();
#endif

  if (index_flag)
    add_graphemes ();

  memo_reset (node_mm);
  memo_reset (grapheme_mm);
  memo_reset (parallels_mm);
  curr_node = NULL;
}
