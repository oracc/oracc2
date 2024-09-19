#include <oraccsys.h>
#include <gutil.h>
#include <gvl.h>
#include <signlist.h>
#include <sx.h>

static Hash *numsets;
static uchar eng[4];

static int
nums_frac_cmp(const uchar *n1, const uchar *d1, const uchar *n2, const uchar *d2)
{
  double dbl1 = strtod((ccp)n1,NULL) / strtod((ccp)d1,NULL), dbl2 = strtod((ccp)n2,NULL) / strtod((ccp)d2,NULL);
  if (dbl1 > dbl2)
    return 1;
  else if (dbl1 < dbl2)
    return -1;
  return 0;
}

static int
nums_rep_cmp(const uchar *r1, const uchar *r2)
{
  const uchar *s1 = (uccp)strchr((ccp)r1,'/'), *s2 = (uccp)strchr((ccp)r2,'/');
  if (s1 && s2)
    return nums_frac_cmp(r1, s1+1, r2, s2+1);
  else if (s2)
    return 1; /* integer must be > frac */
  else if (s1)
    return -11; /* frac must be < integer */
  else
    return atoi((ccp)r1) - atoi((ccp)r2);
}

static int
nums_cmp(const void *a, const void *b)
{
  const struct sl_number *n1 = a;
  const struct sl_number *n2 = b;

  if (n1->setsort != n2->setsort)
    return n1->setsort - n2->setsort;

  if (strcmp((ccp)n1->rep, (ccp)n2->rep))
    return nums_rep_cmp(n1->rep, n2->rep);
  
  return n1->t->s - n2->t->s;
}

static void
sx_num_data(struct sl_signlist *sl, struct sl_number *np, struct sl_token *tp)
{
  np->t = tp;
  np->rep = (uccp)tp->gdl->kids->kids->text;
  np->set = (uccp)tp->gdl->kids->kids->next->text;

  if (strchr((ccp)np->set, '~'))
    {
      uchar buf[strlen((ccp)np->set)+1];
      strcpy((char*)buf, (ccp)np->set);
      uchar *tilde = (uchar*)strchr((ccp)buf, '~');
      *tilde = '\0';
      np->set = pool_copy(buf, sl->p);
    }

  if (!gvl_looks_like_sname(np->set))
    np->set = pool_copy(g_uc((uchar*)np->set), sl->p);
  
  struct sl_token *set_tok = hash_find(sl->htoken, np->set);
  if (set_tok)
    np->setsort = set_tok->s;
  else
    fprintf(stderr, "sx_num_data: no token for np->set %s\n", np->set);  

  if (!hash_find(numsets, np->set))
    hash_add(numsets, np->set, "");
  np->oid = (uccp)tp->gsig;
  if ('q' == *np->oid)
    {
      struct sl_sign *s = hash_find(sl->hsentry, tp->t);
      if (s)
	np->oid = (uccp)s->oid;
      else
	{
	  struct sl_form *f = hash_find(sl->hfentry, tp->t);
	  if (f)
	    np->oid = (uccp)f->oid;
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
	      if (++i == sl->nnumsets)
		{
		  fprintf(stderr,
			  "sx_numsets: numset overflow (have %d but trying to use #%d)\n",
			  sl->nnumsets, i);
		  return;
		}
	      sl->numsets[i].last = sl->numsets[i].from = j;
	    }
	  else
	    sl->numsets[i].last = j;
	}
    }
}

static struct sl_token *
reset_num_tok(struct sl_signlist *sl, struct numvmap_tab *np, struct sl_token *tp)
{
  char buf[strlen((ccp)np->asif)+strlen(tp->gdl->text)];
  char *t = (char*)tp->gdl->text, *b = buf;
  while ('(' != *t)
    *b++ = *t++;
  *b++ = *t++;
  strcpy(b, (ccp)np->asif);
  b += strlen(b);
  while (')' != *t)
    ++t;
  while (*t)
    *b++ = *t++;
  *b = '\0';
  struct sl_token *tokp = asl_bld_token(NULL, sl, pool_copy((uccp)buf, sl->p), 0);
  tokp->s = tp->s;
  tokp->priority = tp->priority;
  return tokp;
}

void
sx_numbers(struct sl_signlist *sl)
{
  memset(eng, '\0', 4);
  wctomb((char*)eng, 0x014b);
  
  const uchar **k = (const uchar **)hash_keys2(sl->hnums, &sl->nnumbers);
  sl->numbers = calloc(sl->nnumbers, sizeof(struct sl_number));
  numsets = hash_create(100);
  int i, j;
  Hash *seen = hash_create(sl->nnumbers/10);
  for (i=j=0; k[i]; ++i)
    {
      struct sl_token *t = hash_find(sl->hnums, k[i]);
      if (t && t->gdl && t->gdl->kids && t->gdl->kids->kids &&  t->gdl->kids->kids->next)
	{
	  const uchar *set = (uccp)t->gdl->kids->kids->next->text;
	  struct numvmap_tab *np = numvmap((ccp)set, strlen((ccp)set));
	  if (np)
	    t = reset_num_tok(sl, np, t);
	  sx_num_data(sl, &sl->numbers[j], t);
	  if (sl->numbers[j].oid)
	    {
	      char key[strlen((ccp)sl->numbers[j].oid)+strlen((ccp)sl->numbers[j].set)+2];
	      sprintf(key, "%s:%s", sl->numbers[j].oid, sl->numbers[j].set);
	      if (!hash_find(seen, (uccp)key))
		{
		  hash_add(seen, pool_copy((uccp)key, sl->p), "");
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
