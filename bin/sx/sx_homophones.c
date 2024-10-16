#include <stddef.h>
#include <gutil.h>
#include <signlist.h>
#include <sx.h>

int htrace = 0;

extern Hash *oid_sort_keys;
static int
spv_cmp(const void *a, const void *b)
{
  struct sl_split_value *spv1 = (*(struct sl_split_value**)a);
  struct sl_split_value *spv2 = (*(struct sl_split_value**)b);
  if (spv1->oid && spv2->oid)
    {
      int a1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)spv1->oid);
      int b1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)spv2->oid);
      if (a1 < b1)
	return -1;
      else if (a1 > b1)
	return 1;
      else
	return 0;
    }
  return 0;
}

static const char *
sx_h_id(struct sl_signlist *sl)
{
  char buf[10];
  static int id = 1;
  sprintf(buf, "h%06d", id++);
  return (ccp)pool_copy((uccp)buf, sl->p);
}

static void
sx_h_sub(struct sl_signlist *sl, Hash *xh, unsigned const char *vname, int xvalue, const char *oid, const char *ref)
{
  unsigned const char *b = pool_copy(g_base_of(vname), sl->p);
  int index = g_index_of(vname, b);
  struct sl_split_value *spv = memo_new(sl->m_split_v);
  spv->v = vname;
  spv->b = b;
  spv->i = index;
  spv->oid = oid;
  spv->ref = (ref ? ref : oid);
  List *lp;
  if (xvalue)
    {
      if (!(lp = hash_find(xh, b)))
	hash_add(xh, b, (lp = list_create(LIST_SINGLE)));
    }
  else
    {
      if (!(lp = hash_find(sl->homophones, b)))
	{
	  hash_add(sl->homophones, b, (lp = list_create(LIST_SINGLE)));
	  hash_add(sl->homophone_ids, b, (void*)sx_h_id(sl));
	}
    }
  if (htrace)
    fprintf(stderr, "htrace: adding %s with base %s/index %d and OID %s\n", vname, spv->b, spv->i, oid);
  list_add(lp, spv);
}

void
sx_homophones(struct sl_signlist *sl)
{
  int i;
  Hash *xhomophones = hash_create(1024);
  sl->homophones = hash_create(1024);
  sl->homophone_ids = hash_create(1024);
  
  for (i = 0; i < sl->nvalues; ++i)
    {
      if (sl->values[i]->parents)
	{
	  if (sl->values[i]->parents->signs)
	    {
	      struct sl_inst *ip;
	      for (ip = list_first(sl->values[i]->parents->signs);
		   ip;
		   ip = list_next(sl->values[i]->parents->signs))
		sx_h_sub(sl, xhomophones, sl->values[i]->name,
			 sl->values[i]->xvalue, ip->u.s->oid, NULL);
	    }
	  if (sl->values[i]->parents->forms)
	    {
	      struct sl_inst *ip, *vip;
	      for (ip = list_first(sl->values[i]->parents->forms);
		   ip;
		   ip = list_next(sl->values[i]->parents->forms))
		if (ip->lv && ip->lv->hventry
		    && (vip = hash_find(ip->lv->hventry,sl->values[i]->name))
		    && !vip->inherited)
		  sx_h_sub(sl, xhomophones, sl->values[i]->name,
			   sl->values[i]->xvalue, ip->u.f->oid, ip->iid);
	    }
	}
    }
 if (xhomophones->key_count)
    {
      const char **keys;
      int nkeys, i, j;
      keys = hash_keys2(xhomophones, &nkeys);
      for (i = 0; i < nkeys; ++i)
	{
	  List *lp = hash_find(xhomophones, (uccp)keys[i]);
	  List *lph = NULL;
	  struct sl_split_value **spv = NULL;
	  
	  if (list_len(lp) > 1)
	    {
	      spv = (struct sl_split_value**)list2array(lp);
	      qsort(spv, list_len(lp), sizeof(struct sl_split_value *), (cmp_fnc_t)spv_cmp);
	    }
	  else
	    {
	      spv = malloc(sizeof(struct sl_split_value*));
	      spv[0] = list_first(lp);
	    }

	  if (!(lph = hash_find(sl->homophones, (uccp)keys[i])))
	    {
	      hash_add(sl->homophones, (uccp)keys[i], (lph = list_create(LIST_SINGLE)));
	      hash_add(sl->homophone_ids, (uccp)keys[i], (void*)sx_h_id(sl));
	    }
	  for (j = 0; j < list_len(lp); ++j)
	    list_add(lph, spv[j]);

	  free(spv);
	}
    }
}
