#ifndef NXCOMMON_H_
#define NXCOMMON_H_

#include <hash.h>
#include <memo.h>
#include <pool.h>

typedef struct nx_num
{
  unsigned long long n;		/* numerator, can be huge when multiplying up ancient
				   values, but can it be this huge? needs testing */
  int d;			/* denominator, at most the value of a single unit of the 
				   smallest fraction, 10800; if this is -1 the mult was
				   'n' */
} nx_num;

typedef struct nx
{
  Memo *m_sys;
  Memo *m_step;
  Memo *m_inst;
  Memo *m_nx_step;
  Memo *m_nx_number;
  Hash *e;			/* global environment */
  List *elist;
  Pool *p;
  struct ns_sys *sys;
  int nsys;
  Hash *ir;			/* global inst registry which indexes inst text to sys
				   that it can be part of */
  FILE *testfp;
} nx;

extern nx *nxp;

#endif/*NXCOMMON_H_*/
