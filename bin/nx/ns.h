#ifndef NS_H_
#define NS_H_

typedef unsigned char uchar;

struct nx_step;
struct nx_inst;

typedef struct nx_num
{
  unsigned long long n;		/* numerator, can be huge when multiplying up
				   ancient values, but can it be this huge?
				   needs testing */
  int d;			/* denominator, at most the value of a single unit of the
				   smallest fraction, 10800; if this is -1 the mult was 'n' */
} nx_num;

typedef struct nx_ns
{
  uchar *name;
  uchar *base;
  uchar *conv;
  struct nx_step *steps;
} nx_ns;

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



#endif /*NS_H_ */
