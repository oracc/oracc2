#include <oraccsys.h>
#include "lx.h"

static Hash *
lx_inter(Hash *r, Hash *l)
{
  const char **keys = hash_keys(r);
  int i;
  Hash *new_r = hash_create(r->key_count);
  for (i = 0; keys[i]; ++i)
    {
      if (hash_find(l, (uccp)keys[i]))
	hash_add(new_r, (uccp)keys[i], hash_find(r, (uccp)keys[i]));
    }
  return new_r;
}

static Hash *
lx_minus(Hash *r, Hash *l)
{
  const char **keys = hash_keys(r);
  int i;
  Hash *new_r = hash_create(r->key_count);
  for (i = 0; keys[i]; ++i)
    {
      if (!hash_find(l, (uccp)keys[i]))
	hash_add(new_r, (uccp)keys[i], hash_find(r, (uccp)keys[i]));
    }
  return new_r;
}

static Hash *
lx_union(Hash *r, Hash *l)
{
  const char **keys = hash_keys(r);
  int i;
  Hash *new_r = hash_create(r->key_count);
  for (i = 0; keys[i]; ++i)
    {
      Lx *lpp = hash_find(l, (uccp)keys[i]);
      if (!lpp)
	hash_add(new_r, (uccp)keys[i], hash_find(r, (uccp)keys[i]));
      else
	{
	  /* We need to take care if adding a proxy list into another
	     list; we don't change the ATF source--if that is in both
	     regular list and proxy the regular list wins.  But if the
	     CAT source is different in the proxy it must be because
	     it was explicitly given in the proxy file, so we override
	     the host CAT field. */
	  Lx *npp = hash_find(r, (uccp)keys[i]);
	  if (npp->proxy && strcmp(lpp->c, npp->c))
	    lpp->c = npp->c;
	  hash_add(new_r, (uccp)keys[i], npp);
	}
    }
  free(keys);
  keys = hash_keys(l);
  for (i = 0; keys[i]; ++i)
    {
      if (!hash_find(r, (uccp)keys[i]))
	hash_add(new_r, (uccp)keys[i], hash_find(l, (uccp)keys[i]));      
    }
  return new_r;  
}

Lxfile *
lx_set_ops(List *todo)
{
  if (list_len(todo) == 1)
    return list_first(todo);
  /* running list r -- we run this as a hash table until operations
     are complete then create the array */
  Lxfile *r = calloc(1, sizeof(Lxfile));
  List_node *lnp = todo->first;
  Hash *h_first = ((Lxfile*)lnp->data)->seen;
  r->seen = hash_create(2 * h_first->key_count);
  hash_merge(r->seen, h_first);
  for (lnp = lnp->next; lnp; lnp = lnp->next->next)
    {
      char op = ((const char *)lnp->data)[0];
      Lxfile *lxp = (Lxfile *)lnp->next->data;
      fprintf(stderr, "running_list %c %s\n", op, lxp->fn);
      switch (op)
	{
	case '+':
	  r->seen = lx_union(r->seen, lxp->seen); 	/* set union */
	  break;
	case '-':
	  r->seen = lx_minus(r->seen, lxp->seen); 	/* set difference */
	  break;
	case '/':
	  r->seen = lx_inter(r->seen, lxp->seen);	/* set intersection */
	  break;
	}
    }

  /* Now set up the running list's items array */
  r->nitems = r->seen->key_count;
  r->items = malloc(r->nitems * sizeof(Lx));
  const char **keys = hash_keys(r->seen);
  int i;
  for (i = 0; keys[i]; ++i)
    r->items[i] = *(Lx*)(hash_find(r->seen, (uccp)keys[i]));
  
  return r;
}
