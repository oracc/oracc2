#include <oraccsys.h>
#include <gutil.h>
#include <gvl.h>
#include <signlist.h>
#include <sx.h>

static Hash *numsets;

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
    np->set = pool_copy(gvl_ucase(np->set), sl->p);
  if (!hash_find(numsets, np->set))
    hash_add(numsets, np->set, "");
  np->oid = (ccp)tp->gsig;
  if ('q' == *np->oid)
    {
      struct sl_value *v = hash_find(sl->hventry, tp->t);
      if (v)
	{
	  if (v->sowner)
	    np->oid = (ccp)v->sowner->oid;
	  else if (v->fowners)
	    np->oid = ((struct sl_inst *)list_first(v->fowners))->u.f->oid;
	}
      else
	fprintf(stderr, "sx_numbers: failed to set oid for %s(%s)\n", np->rep, np->set);
    }
}

static void
sx_numsets(struct sl_signlist *sl)
{
  sl->nnumsets = numsets->key_count;
  hash_free(numsets, NULL);
  sl->numsets = calloc(sl->nnumsets, sizeof(struct sl_numset));
  int i = 0;
  int j;
  for (j = 0; j < sl->nnumbers; ++j)
    {
      if (j)
	{
	  if (strcmp(sl->numbers[j-1].set, sl->numbers[j].set))
	    sl->numsets[++i].from = j;
	  else
	    sl->numsets[i].last = j;
	}	  
    }
}

void
sx_numbers(struct sl_signlist *sl)
{
  sl->numbers = calloc(list_len(sl->nums), sizeof(struct sl_number));
  numsets = hash_create(100);
  struct sl_token *t;
  int i;
  for (i=0,t = list_first(sl->nums); t; t = list_next(sl->nums),++i)
    {
      if (t && t->gdl && t->gdl->kids && t->gdl->kids->kids)
	sx_num_data(sl, &sl->numbers[i], t);
      else
	--i;
    }
  sl->nnumbers = i;
  qsort(sl->numbers, sl->nnumbers, sizeof(struct sl_number), nums_cmp);
  sx_numsets(sl);
  for (i = 0; i < sl->nnumsets; ++i)
    {
      fprintf(stderr, "set %d = %s\n", i, sl->numbers[sl->numsets[i].from].set);
      int j;  
      for (j = sl->numsets[i].from; j <= sl->numsets[i].last; ++j)
	fprintf(stderr, "\t%s %s %s\n", sl->numbers[j].oid, sl->numbers[j].set, sl->numbers[j].rep);
    }
}
