#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"
#include "atf.tab.h"
#include "blocktok.h"

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
