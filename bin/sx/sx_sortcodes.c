#include <ctype128.h>
#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>

void
sx_sortcodes(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *s = sl->signs[i];
      if (oid_list == 2 || (s->type == sx_tle_sign
			    && !s->xref
			    && (!s->U.urev || strcmp(s->U.urev,"0")) && s->inst->valid))
	if (s->oid)
	  {
	    struct sl_token *tp = hash_find(sl->htoken, s->name);
	    printf("%s\t%d\n", s->oid, tp->s);
	  }
    }
  for (i = 0; i < sl->nforms; ++i)
    {
      struct sl_form *f = sl->forms[i];
      struct sl_inst *ip;
      if (oid_list == 2 || !f->U.urev || strcmp(f->U.urev,"0"))
	{
	  for (ip = list_first(f->insts); ip; ip = list_next(f->insts))
	    {
	      if (f->oid && ip->valid)
		{
		  struct sl_token *tp = hash_find(sl->htoken, f->name);
		  printf("%s\t%d\n", f->oid, tp->s);
		  break;
		}
	    }
	}
    }
#if 0
  if (sl && sl->tokens)
    {
      int i;
      for (i = 0; sl->tokens[i]; ++i)
	printf("%s\t%d\n", sl->tokens[i]->t, sl->tokens[i]->s);
    }
#endif
}
