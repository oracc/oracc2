#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include <prop.h>
#include "gdlstate.h"
#include "gdl.h"

/**gdl_group is called to handle +:/ groups.  When called, there must
 * be a preceding grapheme, and if the grapheme which may be a group
 * with the same type as +:/ or any other grapheme type.
 *
 * If the preceding grapheme is not a matching group, a new group is
 * created with the preceding grapheme as its first child, and the
 * group type is set to `type'.  The pointer gdl_group_attach is set
 * to the group and graphemes encountered while gdl_group_attach is
 * non-NULL are attached to gdl_group_attach.
 *
 * When gdl_delim is called with a delim that does not match the
 * current group type, the current node is reset to the group's
 * parent.
 *
 * When a +:/ is found while gdl_group_attach is non-NULL and the
 * group type does not match the +:/, that group is closed by
 * resetting curr to group->rent.
 *
 */

Node *gdl_group_attach = NULL;

static int
gdl_group_type(Node *np, int type)
{
  Prop *p = prop_find_kv(np->props, "g:type", NULL);
  if (p && type == *p->u.k->v)
    return 1;
  else
    return 0;
}

void
gdl_group(Mloc mp, int type)
{
  int same_type = 0;

  gdl_hc(0);

  /* Ensure the attach point is correct and remember if the new group
     is the same as the last group if there is one */
  if (gdl_group_attach && !(same_type = gdl_group_type(gdl_group_attach, type)))
    {
      tree_curr(gdl_group_attach->rent);
      gdl_group_attach = NULL;
    }

  if (!same_type)
    {
      Node *gp = NULL;
      const char *delim = (type=='/'?"/":(type==':'?":":"+"));
      if (lgp)
	{
	  gp = node_group(lgp, lgp, NULL);
	  gp->name = "g:gg";
	  const char *v = (type == '/' ? "alternation" : (type == ':' ? "reordering" : "ligature"));
	  gdl_prop_kv(gp, GP_ATTRIBUTE, PG_GDL_INFO, "g:type", v);
	  gdl_prop_kv(gp->kids, GP_ATTRIBUTE, PG_GDL_INFO, "g:delim", delim);
#if 1
	  tree_curr(gp);
	  gdl_group_attach = gp;
#else
	  /* this shouldn't be necessary any more */
	  if (!strcmp(rt->name, "g:det"))
	    {
	      gdl_post_det_gp_attach = gp;
	      tree_curr(rt);
	    }
	  else
	    {
	      gdl_post_det_gp_attach = NULL;
	      tree_curr(gp->rent);
	    }
#endif
	}
    }
}
