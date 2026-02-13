#include <oraccsys.h>
#include "gdl.h"
#include "gt.h"

/* gt -- GDL Tokens: an interface to the GDL library for applications
 * including gx and sx
 */

static Gt_cfg gtcfg;

void
gt_config(Hash *h, Memo *m)
{
  gtcfg.h = h;
  gtcfg.m = m;
}

static Tree *
gt_gdl(Mloc *locp, unsigned char *s)
{
  Tree *tp = NULL;
  mesg_init();
  tp = gdlparse_string(locp, (char*)s);
  gdlparse_reset();
  return tp;
}

/* SL comment:
  *
 * All of the list entries in @listdef are processed here as literals;
 * this means that when outputting <sl:name> we need to use
 * tokp->gdl->text if tokp->gdl->kids is NULL.
 *
 * We could reparse literals if they occur in another context, but it
 * should be safe to use the literal because listdef entries have a
 * limited syntax.
 *
 * 2024-09-18: overload 'literal': if it is -1 it means we are adding
 * a homophone base, can't be a number.
 *
 */
Gt *
gt_token(Mloc *locp, unsigned char *t, int literal, void *user)
{
  if (!t)
    return NULL;

  Gt *tokp = NULL;
  
  if (!(tokp = hash_find(gtcfg.h, t)))
    {
      Tree *tp;
      const char *gsig = NULL, *deep = NULL, *sign = NULL;
      tokp = memo_new(gtcfg.m);
      tokp->t = t;
      if (literal > 0)
	tp = gdl_literal(locp, (char*)t);
      else
	{
	  extern int gdlsig_depth_mode;
	  tp = gt_gdl(locp, t);

	  /* deep? or a special mode for unicode rendering? */
	  gdlsig_depth_mode = -1;

	  gsig = gdlsig(tp);
	  sign = (ccp)sll_snames_of((uccp)gsig);
  
	  gdlsig_depth_mode = 1;
	  deep = gdlsig(tp);
	  if (deep)
	    {
	      if (!strchr(deep, '.'))
		{
		  free((void*)deep);
		  deep = NULL;
		}
	    }		
	}
      tokp->priority = 100;
      if (gdl_word_mode)
	{
	  tokp->gdl = tp->root->kids;
	}
      else
	{
	  tokp->gdl = tp->root;
	  tokp->gdl->name = "g:w";
	}
      if (!tokp->gdl->mloc)
	tokp->gdl->mloc = mloc_mloc(locp);

      Node *np;
      for (np = tokp->gdl; np; np = np->next)
	{
	  gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "form", np->text);
	  gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", "sux");
	}
      
      tokp->gsh = gsort_prep(tp);
      tokp->gsig = gsig;
      tokp->deep = deep;
      tokp->sign = sign;
      tokp->user = user;
      hash_add(gtcfg.h, t, tokp);
    }
  return tokp;
}

int
gt_toks_gcmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  Gt *t1 = hash_find(gtcfg.h, (uccp)cc1);
  Gt *t2 = hash_find(gtcfg.h, (uccp)cc2);
  return gsort_cmp(&t1->gsh, &t2->gsh);
}

int
gt_toks_scmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  Gt *t1 = hash_find(gtcfg.h, (uccp)cc1);
  Gt *t2 = hash_find(gtcfg.h, (uccp)cc2);
  return t1->s - t2->s;
#if 0
  int a1 = t1->s;
  int b1 = t2->s;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
#endif
}
