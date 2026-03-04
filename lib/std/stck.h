#ifndef STCK_H_
#define STCK_H_

#include <stdint.h>

typedef struct stck
{
  intptr_t *stack;
  int alloced;
  int nalloc;
  int top;
} Stck;

extern intptr_t stck_peek(Stck *s);
extern intptr_t stck_pop(Stck *s);
extern void stck_push(Stck *s, intptr_t tok);
extern Stck *stck_init(int nalloc);
extern void stck_reset(Stck *s);
extern void stck_term(Stck *s);
extern int stck_len(Stck *s);

#endif/*STCK_H*/
