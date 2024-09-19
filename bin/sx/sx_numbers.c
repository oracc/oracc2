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
    np->set = pool_copy(gvl_ucase((uchar*)np->set), sl->p);
  if (!hash_find(numsets, np->set))
    hash_add(numsets, np->set, "");
  np->oid = (uccp)tp->gsig;
  if ('q' == *np->oid)
    {
      struct sl_form *f = hash_find(sl->hfentry, tp->t);
      if (f)
	{
	  np->oid = (uccp)f->oid;
	}
      else
	{
	  struct sl_value *v = hash_find(sl->hventry, tp->t);
	  if (v)
	    {
	      if (v->sowner)
		np->oid = (uccp)v->sowner->oid;
	      else if (v->fowners)
		np->oid = (uccp)((struct sl_inst *)list_first(v->fowners))->u.f->oid;
	    }
	  else
	    fprintf(stderr, "sx_numbers: failed to set oid for %s(%s)\n", np->rep, np->set);
	}
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
	  if (strcmp((ccp)sl->numbers[j-1].set, (ccp)sl->numbers[j].set))
	    {
	      ++i;
	      sl->numsets[i].last = sl->numsets[i].from = j;
	    }
	  else
	    sl->numsets[i].last = j;
	}
    }
}

void
sx_numbers(struct sl_signlist *sl)
{
  const uchar **k = (const uchar **)hash_keys2(sl->hnums, &sl->nnumbers);
  sl->numbers = calloc(sl->nnumbers, sizeof(struct sl_number));
  numsets = hash_create(100);
  int i, j;
  Hash *seen = hash_create(sl->nnumbers/10);
  for (i=j=0; k[i]; ++i)
    {
      struct sl_token *t = hash_find(sl->hnums, k[i]);
      if (t && t->gdl && t->gdl->kids && t->gdl->kids->kids)
	{
	  sx_num_data(sl, &sl->numbers[j], t);
	  if (sl->numbers[j].oid)
	    {
	      char key[strlen(sl->numbers[j].oid)+strlen(sl->numbers[j].set)+2];
	      sprintf(key, "%s:%s", sl->numbers[j].oid, sl->numbers[j].set);
	      if (!hash_find(seen, key))
		{
		  hash_add(seen, pool_copy(key, sl->p), "");
		  ++j;
		}
	    }
	}
    }
  sl->nnumbers = j;
  
  qsort(sl->numbers, sl->nnumbers, sizeof(struct sl_number), nums_cmp);
  sx_numsets(sl);
  for (i = 0; i < sl->nnumsets; ++i)
    {
      fprintf(stderr, "set %d = %s\n", i, sl->numbers[sl->numsets[i].from].set);
      int j;  
      for (j = sl->numsets[i].from; j <= sl->numsets[i].last; ++j)
	fprintf(stderr, "\t%s %s %s < %s\n", sl->numbers[j].oid,
		sl->numbers[j].set, sl->numbers[j].rep, sl->numbers[j].t->t);
    }
}
