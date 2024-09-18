#include <oraccsys.h>
#include <gutil.h>
#include <gvl.h>
#include <signlist.h>
#include <sx.h>

static int nums_cmp(const void *a, const void *b)
{
  struct sl_token *t1 = ((struct sl_number*)a)->t;
  struct sl_token *t2 = ((struct sl_number*)b)->t;
  if (t1->s > t2->s)
    return 1;
  else if (t1->s < t2->s)
    return -1;
  else
    return 0;
}

static void
sx_num_data(struct sl_signlist *sl, struct sl_number *np, struct sl_token *tp)
{
  np->t = tp;
  np->rep = (uccp)tp->gdl->kids->kids->text;
  np->set = (uccp)tp->gdl->kids->kids->next->text;
  if (!gvl_looks_like_sname(np->set))
    np->set = pool_copy(utf_ucase(np->set), sl->p);
}

void
sx_numbers(struct sl_signlist *sl)
{
  sl->numbers = calloc(list_len(sl->nums), sizeof (struct sl_number));
  struct sl_token *t;
  int i;
  for (i=0,t = list_first(sl->nums); t; t = list_next(sl->nums),++i)
    sx_num_data(sl, &sl->numbers[i], t);
  qsort(sl->numbers, i, sizeof(struct sl_number), nums_cmp);
  int j;
  for (j = 0; j < i; ++j)
    {
      const char *oid = sl->numbers[j].t->gsig;
      if ('q' == *oid)
	{
	  struct sl_value *v = hash_find(sl->hventry, sl->numbers[j].t->t);
	  if (v)
	    oid = v->sowner->oid;
	}
      fprintf(stderr, "%s %s %s\n", oid, sl->numbers[j].set, sl->numbers[j].rep);
    }
}
