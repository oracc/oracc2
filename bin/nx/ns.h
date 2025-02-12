#ifndef NS_H_
#define NS_H_

#include "nxcommon.h"

struct ns_sys;
struct ns_step;
struct ns_inst;

typedef struct ns_sys
{
  uchar *name;
  uchar *base;
  uchar *conv;
  Hash *e;			/* system environment */
  List *ilist;			/* List of all inst registered for this sys */
  struct ns_step *steps;
  struct ns_step *last;
  struct ns_step *base_step;
  uchar *det;
  Hash *i;			/* inst text pointing to sys copy of inst */
  int aev_done;
  int lnum;
} ns_sys;

typedef struct ns_step
{
  nx_num aev;	/* ancient equivalence value, i.e., this step
		   expressed in terms of the system base--this is for
		   1x the unit; nx_inst->aev has the multiplied
		   version for the instance */
  nx_num mult;
  uchar *unit;
  enum nx_numtok type;
  struct ns_sys *sys;
  struct ns_step *next;
  struct ns_step *prev;
  struct ns_step *alt;
  struct ns_inst *insts;
  struct ns_inst *fracs;
  struct ns_inst *last;
  struct nw_tab *nwp;
  char a_or_d;
  const char *axis;
} ns_step;

typedef struct ns_inst
{
  struct ns_step *step;		/* pointer to the step structure in the ns_sys */
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
  const char *oid;		/* the OID of the grapheme or word */
  const uchar *ucun;		/* the Unicode for a grapheme in UTF-8 */
  const uchar *unit;		/* the name of the corresponding step */
  nx_num aev;			/* the aev for the count * the ns_step->aev */
  nx_num mult;			/* the mult for the inst when ns_step has fraction insts */
  nx_num count;			/* the count for the step; can be integer or fraction */
  char a_or_d;			/* 'a' or 'd' for aš or diš */
  const char *axis;	       	/* axis--full meta-system e.g. A@c */
} ns_inst;

typedef enum ns_inst_method { NS_INST_AUTO , NS_INST_DATA } ns_inst_method;

extern Pool *nspool;
extern int nsflextrace, ns_flex_debug;
extern int nslex (void);
extern int nsparse (void);

extern void nsb_sys (uchar * t);
extern void nsb_det (uchar * t);
extern void nsb_equiv (uchar * b, uchar * c);
extern void nsb_env (uchar * b, uchar * c);
extern void nsb_step (uchar * a, uchar * b, uchar * c);
extern void nsb_inst_g (const uchar * g, const uchar * n, const uchar * u, ns_inst_method meth);
extern ns_inst *nsb_inst_u (const uchar * x, uchar * g, uchar * u, ns_inst_method meth);
extern void nsb_wrapup (void);
extern void nsb_frac(const uchar *f);

#endif /* NS_H_ */
