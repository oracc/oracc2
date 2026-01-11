#ifndef POOL_H_
#define POOL_H_ 1
#define POOL_BLOCK_SIZE        (8192*8)
#define POOL_OWN_BLOCK_CUTOFF 	128

#include <stdlib.h>
#include <hash.h>

typedef enum pooltype { POOL_CHR , POOL_SZT } Pooltype;

struct pool
{
  struct pool_block *rover;
  struct pool_block *base;
  Pooltype type;
  Hash *h;
};

typedef struct pool Pool;

struct pool_block
{
  unsigned char *mem;
  unsigned char *top;
  unsigned char *last_begin;
  unsigned char *used;
  struct pool_block *next;
};

#define hpool_term(p) pool_term((p))

extern Pool* pool_init(void);
extern void pool_reset(Pool*);
extern void pool_term(Pool*);
extern unsigned char *pool_alloc(size_t len, Pool *p);
extern unsigned char *pool_copy(const unsigned char *s, Pool*p);

extern Pool* hpool_init(void);
extern unsigned char *hpool_copy(const unsigned char *s, Pool*p);

extern Pool *ipool_init(void);
extern size_t ipool_copy(register const unsigned char *s, Pool *p);
extern size_t ipool_len(Pool *p);
#define ipool_str(pl,ix) ((pl)->base->mem+ix)

extern Pool *ihpool_init(void);
extern size_t ihpool_copy(register const unsigned char *s, Pool *p);

#endif
