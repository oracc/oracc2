#ifndef NS_H_
#define NS_H_

#include <hash.h>
#include <memo.h>
#include <pool.h>

typedef unsigned char uchar;

struct nx_sys;
struct nx_step;
struct nx_inst;

typedef struct nx
{
  Memo *m_sys;
  Memo *m_step;
  Memo *m_inst;
  Hash *e; /* global environment */
  List *elist;
  Pool *p;
  struct nx_sys *sys;
  int nsys;
} nx;

typedef struct nx_num
{
  unsigned long long n;		/* numerator, can be huge when multiplying up
				   ancient values, but can it be this huge?
				   needs testing */
  int d;			/* denominator, at most the value of a single unit of the
				   smallest fraction, 10800; if this is -1 the mult was 'n' */
} nx_num;

typedef struct nx_sys
{
  uchar *name;
  uchar *base;
  uchar *conv;
  Hash *e; /* system environment */
  List *elist;
  struct nx_step *steps;
  struct nx_step *last;
} nx_sys;

typedef struct nx_step
{
  nx_num mult;
  uchar *unit;
  struct nx_step *next;
  struct nx_step *prev;
  struct nx_sys *sys;
  struct nx_inst *insts;
  struct nx_inst *last;
} nx_step;

typedef struct nx_inst
{
  struct nx_step *step;		/* pointer to the step structure in the nx_ns */
  struct nx_inst *next;
  struct nx_inst *prev;
  uchar *text;			/* the grapheme or word in the instance */
  uchar *unit;			/* the name of the corresponding step */
  nx_num count;			/* the count for the step; can be integer or fraction */
  char a_or_d;			/* 'a' or 'd' for aš or diš */
} nx_inst;

extern nx *nxp;

extern int nsflextrace, ns_flex_debug;
extern int nslex(void);
extern int nsparse(void);

extern void nsb_sys(uchar *t);
extern void nsb_equiv(uchar *b, uchar *c);
extern void nsb_env(uchar *b, uchar *c);
extern void nsb_step(uchar *b, uchar *c);
extern void nsb_inst_g(uchar *g, uchar *n, uchar *u);
extern void nsb_inst_u(uchar *x, uchar *g, uchar *u);

#endif /*NS_H_ */
