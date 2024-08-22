#ifndef NS_H_
#define NS_H_

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
  struct nx_step *steps;
} nx_sys;

typedef struct nx_step
{
  nx_num mult;
  uchar *name;
  struct nx_inst *insts;
  struct nx_step *next;
  struct nx_step *prev;
  struct nx_ns *ns;
} nx_step;

typedef struct nx_inst
{
  uchar *text;			/* the grapheme or word in the instance */
  uchar *name;			/* the name of the corresponding step */
  nx_num count;			/* the count for the step; can be integer or fraction */
  struct nx_step *step;		/* pointer to the step structure in the nx_ns */
  struct nx_inst *next;
  struct nx_inst *prev;
  char a_or_d;			/* 'a' or 'd' for aš or diš */
} nx_inst;

extern nx *nxp;

extern int nsflextrace, ns_flex_debug;
extern int nslex(void);
extern int nsparse(void);

extern void nsb_sys(uchar *t);

#endif /*NS_H_ */
