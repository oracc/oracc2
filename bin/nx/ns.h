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
  Hash *e;			/* global environment */
  List *elist;
  Pool *p;
  struct nx_sys *sys;
  int nsys;
  Hash *ir;			/* global inst registry which indexes inst text to sys
				   that it can be part of */
} nx;

typedef struct nx_num
{
  unsigned long long n;		/* numerator, can be huge when multiplying up ancient
				   values, but can it be this huge? needs testing */
  int d;			/* denominator, at most the value of a single unit of the 
				   smallest fraction, 10800; if this is -1 the mult was
				   'n' */
} nx_num;

typedef struct nx_sys
{
  uchar *name;
  uchar *base;
  uchar *conv;
  Hash *e;			/* system environment */
  List *elist;
  struct nx_step *steps;
  struct nx_step *last;
  Hash *i;			/* inst text pointing to sys copy of inst */
} nx_sys;

typedef struct nx_step
{
  nx_num mult;
  uchar *unit;
  struct nx_sys *sys;
  struct nx_step *next;
  struct nx_step *prev;
  struct nx_inst *insts;
  struct nx_inst *last;
  char a_or_d;
} nx_step;

typedef struct nx_inst
{
  struct nx_step *step;		/* pointer to the step structure in the nx_ns */
  struct nx_inst *next;
  struct nx_inst *prev;
  struct nx_inst *ir_next;	/* inst are registered in nx->ir; nx_inst->text is the
				   key and the value is nx_inst*.  Multiple occurrences
				   of nx_inst->text in nx->ir are linked in the order
				   they are read from ns.dat using nx_inst->ir_next. */
  struct nx_inst *ir_last;	/* nx_sys->ir_last is only used in the inst that is the
				   value in the nx->ir registry; it points to the end of
				   the inst list so we can add new inst to the list
				   without traversing the list to the end all the time */
  const uchar *text;		/* the grapheme or word in the instance */
  const uchar *unit;		/* the name of the corresponding step */
  nx_num count;			/* the count for the step; can be integer or fraction */
  char a_or_d;			/* 'a' or 'd' for aš or diš */
} nx_inst;

typedef enum ns_inst_method { NS_INST_AUTO , NS_INST_DATA } ns_inst_method;

extern nx *nxp;

extern int nsflextrace, ns_flex_debug;
extern int nslex (void);
extern int nsparse (void);

extern void nsb_sys (uchar * t);
extern void nsb_equiv (uchar * b, uchar * c);
extern void nsb_env (uchar * b, uchar * c);
extern void nsb_step (uchar * b, uchar * c);
extern void nsb_inst_g (const uchar * g, const uchar * n, const uchar * u, ns_inst_method meth);
extern void nsb_inst_u (uchar * x, uchar * g, uchar * u, ns_inst_method meth);
extern void nsb_wrapup (void);

#endif /* NS_H_ */
