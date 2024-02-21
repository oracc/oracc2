#include <stdio.h>
#include <sx.h>

void
sx_oid_list(struct sl_signlist *sl)
{
  extern int sortcode_output;
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *s = sl->signs[i];
      if (oid_list == 2 || (s->type == sx_tle_sign
			    && !s->xref
			    && (!s->U.urev || strcmp(s->U.urev,"0")) && s->inst->valid))
	if (s->oid)
	  {
	    if (sortcode_output)
	      printf("%s\t%d\t%s\n", s->oid, (int)(uintptr_t)hash_find(oid_sort_keys, s->oid), s->name);
	    else
	      printf("%s\t%s\n", s->oid, s->name);
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
		  if (sortcode_output)
		    printf("%s\t%d\t%s\n", f->oid, (int)(uintptr_t)hash_find(oid_sort_keys, f->oid), f->name);
		  else
		    printf("%s\t%s\n", f->oid, f->name);
		  break;
		}
	    }
	}
    }
}

void
sx_oid_tab(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *s = sl->signs[i];
      if (s->type == sx_tle_sign && !s->xref)
	if (s->oid)
	  printf("%s\tu\t/%s/signlist/%s/%s/index.html\n",
		 s->oid, sl->project, s->letter->xmlid, s->oid);
    }
  for (i = 0; i < sl->nforms; ++i)
    {
      struct sl_form *f = sl->forms[i];
      if (f->oid)
	{
	  struct sl_inst *ip;
	  for (ip = list_first(f->insts); ip; ip = list_next(f->insts))
	    {
	      if ('f' == ip->type)
		ip = ip->parent_s;
	      if (ip)
		printf("%s\tu\t/%s/signlist/%s/%s/index.html\n",
		       f->oid, sl->project, ip->u.s->letter->xmlid, ip->u.s->oid);
	    }
	}
    }
}
