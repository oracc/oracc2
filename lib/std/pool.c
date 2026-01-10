#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pool.h"

#undef POOL_DEBUG

static struct pool_block *new_block(struct pool *p);

struct pool *
pool_init(void)
{
  struct pool *p = calloc(1,sizeof(struct pool));
  if (!p)
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  p->base = p->rover = new_block(p);
  p->type = POOL_CHR;
  return p;
}

void
pool_reset(struct pool *p)
{
  p->rover = p->base;
  p->base->used = p->base->mem;
}

void
pool_term(struct pool *p)
{
  struct pool_block *pbp,*tmp;
  if (!p)
    return;
  for (pbp = p->base; pbp; pbp = tmp)
    {
      tmp = pbp->next;
      free(pbp->mem);
      free(pbp);
    }
  if (p->h)
    hash_free(p->h, NULL);
  free(p);
}

static struct pool_block *
new_block(struct pool *p)
{
  struct pool_block *new;
  new = malloc(sizeof(struct pool_block));
  if (!new)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->used = new->mem = calloc(1,POOL_BLOCK_SIZE);
  if (!new->mem)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->top = new->mem + POOL_BLOCK_SIZE;
  new->next = NULL;
  if (p->rover)
    p->rover->next = new;
  return new;
}

static struct pool_block *
new_block2(struct pool *p, size_t len)
{
  struct pool_block *new;
  new = malloc(sizeof(struct pool_block));
  if (!new)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->used = new->mem = malloc(len);
  if (!new->mem)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->top = new->mem + POOL_BLOCK_SIZE;
  new->next = NULL;
  if (p->rover)
    p->rover->next = new;
  return new;
}

static struct pool_block *
re_block(struct pool *p)
{
  if (!p->base)
    {
      struct pool_block *new;
      new = malloc(sizeof(struct pool_block));
      if (!new)
	{
	  (void)fputs("out of core",stderr);
	  exit(2);
	}
      new->used = new->mem = calloc(1,POOL_BLOCK_SIZE);
      if (!new->mem)
	{
	  (void)fputs("out of core",stderr);
	  exit(2);
	}
      new->top = new->mem + POOL_BLOCK_SIZE;
      new->next = NULL;
      p->base = p->rover = new;
    }
  else
    {
      p->base->top += POOL_BLOCK_SIZE;
      p->base->mem = realloc(p->base->mem, p->base->top - p->base->mem);
    }
  return p->base;
}

unsigned char *
pool_alloc(size_t len, struct pool *p)
{
  if (p->type != POOL_CHR)
    {
      fprintf(stderr, "pool_alloc: internal error: only implemented for Pooltype == P_CHR\n");
      return NULL;
    }
	
  if (len >= POOL_BLOCK_SIZE)
    {
      /* fprintf(stderr, "len = %ld\n", len); */
      p->rover = new_block2(p,len);
    }
  else {
    if ((p->rover->used+len) >= p->rover->top)
      p->rover = new_block(p);
  }
  p->rover->last_begin = p->rover->used;
  p->rover->used += len;
  return p->rover->last_begin;
}

unsigned char *
pool_copy(register const unsigned char *s, Pool *p)
{
  size_t len;
  
  if (!s)
    return NULL;
  len = strlen((char *)s) + 1;
  if (len >= POOL_BLOCK_SIZE)
    {
      /* fprintf(stderr, "len = %ld\n", len); */
      p->rover = new_block2(p,len);
    }
  else {
    if ((p->rover->used+len) >= p->rover->top)
      p->rover = new_block(p);
  }
  p->rover->last_begin = memcpy(p->rover->used, s, len);
  p->rover->used += len;
  return p->rover->last_begin;
}

unsigned char *
hpool_copy(register const unsigned char *s, Pool *p)
{
  if (s && p && p->h)
    {      
      unsigned char *t = hash_find(p->h, (ucp)s);
      if (!t)
	{
	  t = (ucp)pool_copy((ucp)s, p);
	  hash_add(p->h, (uccp)t, (void*)t);
	}
      return t;
    }
  return NULL;
}

size_t
ipool_copy(register const unsigned char *s, Pool *p)
{
  size_t len;
  
  if (!s)
    return -1;
  
  len = strlen((char *)s) + 1;

  if ((p->rover->used+len) >= p->rover->top)
    p->rover = re_block(p);

  p->rover->last_begin = memcpy(p->rover->used, s, len);
  p->rover->used += len;
  
  return p->rover->last_begin - p->rover->mem;
}

size_t
ihpool_copy(register const unsigned char *s, Pool *p)
{
  if (s && p && p->h)
    {      
      size_t t = (size_t)(uintptr_t)hash_find(p->h, (ucp)s);
      if (!t)
	{
	  t = ipool_copy((ucp)s, p);
	  hash_add(p->h, (uccp)t, (void*)(uintptr_t)t);
	}
      return t;
    }
  return 0;
}

Pool *
hpool_init(void)
{
  Pool *p = pool_init();
  p->h = hash_create(2048);
  return p;
}

Pool *
ipool_init(void)
{
  Pool *p = pool_init();
  p->type = POOL_SZT;
  return p;
}

Pool *
ihpool_init(void)
{
  Pool *p = hpool_init();
  p->type = POOL_SZT;
  return p;
}

#if 0
/* not sure I'm using this */
/* Call with p==NULL to reset */
unsigned char *
pool_each(Pool *p)
{
  static Pool_block *pb = NULL;
  static unsigned char *curr = NULL;
  if (!p)
    {
      curr = NULL;
      pb = NULL;
    }
  else if (!pb)
    {
      pb = p->pool_base;
      curr = pb->mem;
    }  
}
#endif
