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

static void
lx_union(Hash *r, Hash *l)
{
  hash_merge(r, l);
}

Lxfile *
lx_process(List *todo)
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
      printf("running_list %c %s\n", op, lxp->fn);
      switch (op)
	{
	case '+':
	  lx_union(r->seen, lxp->seen); 		/* set union is directly into r->seen */
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
  r->items = malloc(r->seen->key_count * sizeof(Lx));
  const char **keys = hash_keys(r->seen);
  int i;
  for (i = 0; keys[i]; ++i)
    r->items[i] = *(Lx*)(hash_find(r->seen, (uccp)keys[i]));
  
  return r;
}
