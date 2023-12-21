#include <stdlib.h>
#include <memo.h>

void
memo_exec(Memo *m, void (*f)(void *))
{
  struct memo_block *b;
  for (b = m->mem_base; b; b = b->next)
    {
      int i;
      if (b->next)
	{
	  for (i = 0; i < m->elements_per_alloc; ++i)
	    f(&b[i * m->element_size]);
	}
      else
	{
	  for (i = 0; i < m->last_element_used; ++i)
	    f(&b[i * m->element_size]);
	}
    }
}

size_t
memo_len(Memo *m)
{
  struct memo_block *b;
  size_t len = 0;
  for (b = m->mem_base; b; b = b->next)
    {
      if (b->next)
	len += m->elements_per_alloc;
      else
	len += m->last_element_used;
    }
  return len;
}

/* return the memo values as an array of char *; memo must have been
   populated with char ** (duh) */
char **
memo_keys(Memo *m, size_t *nkeys)
{
  struct memo_block *b;
  if (m->element_size == sizeof(char**))
    {
      size_t len = memo_len(m);
      char **k = malloc((1+len) * sizeof(char*));
      size_t used = 0;
      for (b = m->mem_base; b; b = b->next)
	{
	  if (b->next)
	    {
	      memcpy(k + used, b->mem, b->bytes_in_block);
	      used += m->elements_per_alloc;
	    }
	  else
	    memcpy(k + used, b->mem, (m->last_element_used * m->element_size));
	}
      *nkeys = len;
      k[len] = NULL;
      return k;
    }
  else
    return NULL;
}
