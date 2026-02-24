#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"
#include "atf.tab.h"
#include "blocktok.h"

static void
atf_implicit(const char *n)
{
  Node *np = atf_push(n);
  if (!strcmp(n, "object"))
    atf_xprop(np, "type", "tablet");
  else if (!strcmp(n, "surface"))
    atf_xprop(np, "type", "obverse");
  else if (!strcmp(n, "column"))
    atf_xprop(np, "n", "0");
  atf_xprop(np, "implicit", "1");
}

static void
m_types(const char *s, const char **typep, const char **subtp)
{
  char *p = (char*)pool_copy((uccp)s, atfmp->pool);
  *typep = p;
  while (*p && !isspace(*p))
    ++p;
  if (*p)
    *p++ = '\0';
  *subtp = p;
  while (*p && !isspace(*p))
    ++p;
  if (*p)
    *p = '\0';      
}

/* Find the ancestor-or-self which is a block token at the level of arg2;
 * if not found, set tree_curr to the first ancestor that wasn't a block.
 */
static Node *
ancestor_or_self_level(Node *np, Block_level b)
{
  while (1)
    {
      if (!np->user)
	break;
      if (((Block*)np->user)->bt->type == b)
	return np;
      else if (np->rent)
	{
	  np = np->rent;
	  if (!blocktok(np->name, strlen(np->name)))
	    break;
	}
      else
	break;
    }
  tree_curr(np);
  return NULL;
}

/* Ensure that abt->curr appropriate for attaching the new block */
static void
set_block_curr(Block *bp)
{
  if (EDOC_COMPOSITE == atfp->edoc)
    tree_curr(node_ancestor_or_self(abt->curr, "composite"));
  else
    {
      switch (bp->bt->type)
	{
	case B_OBJECT:
	  tree_curr(abt->root->kids); /* add '->last' here? */
	  break;
	case B_SURFACE:
	  {
	    Node *np = ancestor_or_self_level(abt->curr, B_OBJECT);
	    if (!np)
	      atf_implicit("object");
	    else
	      tree_curr(np);
	  }
	  break;
	case B_COLUMN:
	  {
	    Node *np = ancestor_or_self_level(abt->curr, B_SURFACE);
	    if (!np)
	      {
		Node *op = ancestor_or_self_level(abt->curr, B_OBJECT);
		if (!op)
		  atf_implicit("object");
		atf_implicit("surface");
	      }
	    else
	      tree_curr(np);
	  }  
	  break;
	case B_MILESTONE:
	  {
	    Node *np = ancestor_or_self_level(abt->curr, B_COLUMN);
	    if (!np)
	      {
		Node *sp = ancestor_or_self_level(abt->curr, B_SURFACE);
		if (!sp)
		  {
		    Node *op = ancestor_or_self_level(abt->curr, B_OBJECT);
		    if (!op)
		      atf_implicit("object");
		    atf_implicit("surface");
		  }
		atf_implicit("column");
	      }
	    else
	      tree_curr(np);
	  }
	  break;
	default:
	  /* other types may not need any reset of curr */
	  break;
	}
	
    }
}

void
atf_bld_block(Mloc l, int ytok, char *rest)
{
  Block *bp = memo_new(atfmp->mblocks);
  bp->bt = curr_bt;

  if ('=' == *rest)
    ++rest;

  if (Y_M == ytok)
    {
      bp->text = rest;
      m_types(rest, &bp->type, &bp->subt);
    }
  else
    {
      if (strcspn(rest, "*!?"))
	{
	  bp->flag = rest;
	  while ('!' == *rest || '?' == *rest || '*' == *rest)
	    ++rest;
	  if (*rest)
	    {
	      if (isspace(*rest))
		{
		  *rest++ = '\0';
		  while (isspace(*rest))
		    ++rest;
		  if (*rest)
		    bp->text = rest;
		}
	      else
		{
		  int len = rest-bp->flag;
		  char *f = (char*)pool_alloc(1+len, atfmp->pool);
		  strncpy(f, bp->flag, len);
		  f[len] = '\0';
		  bp->flag = f;
		  bp->text = rest;
		}
	    }
	}
    }

  set_block_curr(bp);
  
  if (atfp->edoc == EDOC_TRANSLITERATION)
    bp->np = atf_push(bp->bt->name);
  else
    bp->np = atf_add(bp->bt->name);

  bp->np->user = bp;
  if (bp->type)
    {
      atf_xprop(bp->np, "type", bp->type);
      if (bp->subt)
	atf_xprop(bp->np, "subtype", bp->subt);
    }
  atf_input(l, LT_BLOCK, bp);
}
