#ifndef NXCOMMON_H_
#define NXCOMMON_H_

#include <hash.h>
#include <memo.h>
#include <pool.h>

typedef unsigned char uchar;

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
  Memo *m_ns_sys;
  Memo *m_ns_step;
  Memo *m_ns_inst;
  Memo *m_nx_inst;
  Memo *m_nx_step;
  Memo *m_nx_number;
  Pool *b;			/* build pool; not reset during input */
  struct ns_sys *sys;
  int nsys;
  Hash *ir;			/* global inst registry which indexes inst text to sys
				   that it can be part of */
  Pool *p;
  const uchar **toks;
  const uchar **data;
  Hash *env;			/* token list environment; initialized
				   in nx_init; freed in nx_term;
				   values overwritten by each
				   assignment; pool_copy must use build pool 'b' */
  Pool *e;			/* hash pool for environment values */
  List *hashes;			/* instance hashes are easier to free with this list */
  List *lists;			/* sys->elists are easier to free with this list */
  FILE *testfp;
  FILE *input;
} nx;

extern nx *nxp;

#endif/*NXCOMMON_H_*/
