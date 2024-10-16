#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pool.h"

#undef POOL_DEBUG

static struct pool_block *new_block(struct pool *p);

struct pool *
pool_init()
{
  struct pool *p = calloc(1,sizeof(struct pool));
  if (!p)
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  p->base = p->rover = new_block(p);
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

unsigned char *
pool_alloc(size_t len, struct pool *p)
{
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

Pool *
hpool_init(void)
{
  Pool *p = pool_init();
  p->h = hash_create(2048);
  return p;
}

void
hpool_term(Pool *p)
{
  if (p->h)
    hash_free(p->h, NULL);
  pool_term(p);
}
