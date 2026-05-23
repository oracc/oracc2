#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"

/* 0 = shallow, -1 = mid, 1 = deep, 2 = atoms
 *
 * shallow means if a unit has no OID return error ("q99")
 *
 * mid means if a unit is a compound, use the OID for the compound if
 * possible, otherwise descend into the compound and try to resolve
 * via the components
 *
 * deep means always descend into compounds
 *
 * atoms means extract all minimal compoents, removing '@t' and other modifiers/allograph markers
 *
 */
int gdlsig_depth_mode = 0;

void gdlsig_descend(Node *np, List *lp);
const char *gdlsig_sep(const char *sep);

const char *
gdlsig_str(Mloc *mp, unsigned char *atf, int frag_ok, int deep)
{
  Tree *tp = NULL;
  int saved_deep = gdlsig_depth_mode;
  const char *sig = NULL;
  
  gdlsig_depth_mode = deep;
  gdlparse_reset();
  tp = gdlparse_string(mp, (char*)atf);
  if (tp)
    sig = gdlsig(tp);
  gdlsig_depth_mode = saved_deep; 
  return sig;
}

void
gdlsig_addoid(Node *np, List *lp)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (!strcmp(np->name, "g:c")
	  && (gdlsig_depth_mode > 0
	      || (gdlsig_depth_mode < 0 && !gp->oid)))
	{
	  gdlsig_descend(np, lp);
	}
      else if (gdlsig_depth_mode && gp->deep)
	gdlsig_descend(gp->deep, lp);
      else if (gp->oid)
	list_add(lp, (char*)gp->oid);
      else
	list_add(lp, "q99");
    }
  else
    list_add(lp, "q99");
}

Node *
gdlsig_oidnode(Node *np)
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
	  if (gdlsig_depth_mode == 2 && np->kids && !strcmp(np->kids->name, "g:b"))
	    return np->kids;
	  else
	    return np;
	  break;
	case 'g':
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
gdlsig_one_node(Node *np, List *lp)
{
  Prop *p = NULL;
  Node *oidnode = gdlsig_oidnode(np);
  if (oidnode)
    {
      gdlsig_addoid(oidnode, lp);
      if ((p = prop_find_kv(oidnode->props, "g:delim", NULL)))
	list_add(lp, (char*)gdlsig_sep((char*)p->u.k->v));
      /* g:q nodes have the (X) component returned as the oidnode but g:delim is on g:q */
      else if (np && !strcmp(np->name, "g:q")
	       && (p = prop_find_kv(np->props, "g:delim", NULL)))
	list_add(lp, (char*)gdlsig_sep((char*)p->u.k->v));
    }
  else if (!strcmp(np->name, "g:det"))
    {
      Prop *d = prop_find_kv(np->props, "g:pos", NULL);
      if (d && !strcmp(d->u.k->v, "post"))
	list_add(lp, ".");
      gdlsig_descend(np, lp);
      if (d && !strcmp(d->u.k->v, "pre"))
	list_add(lp, ".");
      else if ((p = prop_find_kv(np->props, "g:delim", NULL)))
	list_add(lp, (char*)gdlsig_sep((char*)p->u.k->v));
    }
  else if (np->kids)
    {
      gdlsig_descend(np, lp);
      if ((p = prop_find_kv(np->props, "g:delim", NULL)))
	list_add(lp, (char*)gdlsig_sep((char*)p->u.k->v));
    }
}

void
gdlsig_descend(Node *np, List *lp)
{
  Node *npp;
  for (npp = np->kids; npp; npp = npp->next)
    gdlsig_one_node(npp, lp);
}

const char *
gdlsig_sep(const char *sep)
{
  switch (*sep)
    {
    case '.':
    case '-':
    case ':':
    case '+':
      return ".";
      break;
    default:
      if ((unsigned char)(*sep) > 127)
	return "*";
      else
	return sep;
    }
}

const char *
gdlsig(Tree *tp)
{
  if (tp && tp->root && tp->root->kids)
    {
      List *lp = list_create(LIST_SINGLE);
      const char *sig = NULL;
      Node *np;
      for (np = tp->root->kids; np; np = np->next)
	{
	  gdlsig_one_node(np, lp);
	  if (np->next && !strcmp(np->name, "g:w"))
	    list_add(lp, "_");
	}
      sig = (const char *)list_concat(lp);
      list_free(lp, NULL);
      return sig;
    }
  return NULL;
}
