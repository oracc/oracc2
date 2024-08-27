#ifndef NS_H_
#define NS_H_

#include "nxcommon.h"

typedef unsigned char uchar;

struct ns_sys;
struct ns_step;
struct ns_inst;

typedef struct ns_sys
{
  uchar *name;
  uchar *base;
  uchar *conv;
  Hash *e;			/* system environment */
  List *elist;
  struct ns_step *steps;
  struct ns_step *last;
  uchar *det;
  Hash *i;			/* inst text pointing to sys copy of inst */
  int aev_done;
} ns_sys;

typedef struct ns_step
{
  nx_num aev;	/* ancient equivalence value, i.e., this step
		   expressed in terms of the system base--this is for
		   1x the unit; nx_step->aev has the multiplied
		   version for the instance */
  nx_num mult;
  uchar *unit;
  struct ns_sys *sys;
  struct ns_step *next;
  struct ns_step *prev;
  struct ns_step *alt;
  struct ns_inst *insts;
  struct ns_inst *last;  
  char a_or_d;
} ns_step;

typedef struct ns_inst
{
  struct ns_step *step;		/* pointer to the step structure in the ns_ns */
  struct ns_inst *next;
  struct ns_inst *prev;
  struct ns_inst *ir_next;	/* inst are registered in nx->ir; ns_inst->text is the
				   key and the value is ns_inst*.  Multiple occurrences
				   of ns_inst->text in nx->ir are linked in the order
				   they are read from ns.dat using ns_inst->ir_next. */
  struct ns_inst *ir_last;	/* ns_sys->ir_last is only used in the inst that is the
				   value in the nx->ir registry; it points to the end of
				   the inst list so we can add new inst to the list
				   without traversing the list to the end all the time */
  const uchar *text;		/* the grapheme or word in the instance */
  const uchar *unit;		/* the name of the corresponding step */
  nx_num aev;			/* the aev for the count * the ns_step->aev */
  nx_num count;			/* the count for the step; can be integer or fraction */
  char a_or_d;			/* 'a' or 'd' for aš or diš */
} ns_inst;

typedef enum ns_inst_method { NS_INST_AUTO , NS_INST_DATA } ns_inst_method;

extern int nsflextrace, ns_flex_debug;
extern int nslex (void);
extern int nsparse (void);

extern void nsb_sys (uchar * t);
extern void nsb_det (uchar * t);
extern void nsb_equiv (uchar * b, uchar * c);
extern void nsb_env (uchar * b, uchar * c);
extern void nsb_step (uchar * a, uchar * b, uchar * c);
extern void nsb_inst_g (const uchar * g, const uchar * n, const uchar * u, ns_inst_method meth);
extern void nsb_inst_u (uchar * x, uchar * g, uchar * u, ns_inst_method meth);
extern void nsb_wrapup (void);

#endif /* NS_H_ */
