#include <oraccsys.h>
#include <gvl.h>

int gvl_ct_depth_mode = 0;

static void gvl_ct_descend(Node *np, List *lp);
static Node *gvl_ct_oidnode(Node *np);

static void
gvl_ct_addcun(Node *np, List *lp)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (!strcmp(np->name, "g:c")
	  && (gvl_ct_depth_mode > 0
	      || (gvl_ct_depth_mode < 0 && !gp->oid)))
	{
	  gvl_ct_descend(np, lp);
	}
      else if (gvl_ct_depth_mode && gp->deep)
	gvl_ct_descend(gp->deep, lp);
      else if (gp->oid)
	{
	  if (!gp->utf8)
	    gp->utf8 = gvl_cuneify_gv(gp);
	  list_add(lp, (char*)gp->utf8);
	}
      else
	list_add(lp, "q99");
    }
  else
    list_add(lp, "q99");
}

static Node *
gvl_ct_oidnode(Node *np)
{
  if (strlen(np->name) == 3)
    {
      switch (np->name[2])
	{
	case 'c':
	case 'l':
	case 'p':
	case 's':
	case 'v':
	  return np;
	  break;
	case 'n':
	  if (np->user && ((gvl_g*)np->user)->oid)
	    return np;
	  /* FALL THROUGH TO QUALIFIER PROCESSING */
	case 'q':
	  return np->kids ? np->kids->next : NULL;
	  break;
	default:
	  return NULL;
	}
    }
  else
    return NULL;
}

static void
gvl_ct_descend(Node *np, List *lp)
{
  Node *npp;
  for (npp = np->kids; npp; npp = npp->next)
    {
      Node *oidnode = gvl_ct_oidnode(npp);
      if (oidnode)
	gvl_ct_addcun(oidnode, lp);
      else
	gvl_ct_descend(npp, lp);
    }
}

const char *
gvl_cuneify_tree(Tree *tp)
{
  if (tp && tp->root && tp->root->kids)
    {
      List *lp = list_create(LIST_SINGLE);
      const char *sig = NULL;
      Node *np;
      for (np = tp->root->kids; np; np = np->next)
	{
	  Node *oidnode = gvl_ct_oidnode(np);
	  if (oidnode)
	    gvl_ct_addcun(oidnode, lp);
	  else if (np->kids)
	    gvl_ct_descend(np, lp);
	}
      sig = (const char *)list_concat(lp);
      list_free(lp, NULL);
      return sig;
    }
  return NULL;
}
