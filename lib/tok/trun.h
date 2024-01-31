#ifndef TRUN_H_
#define TRUN_H_

#include "loch.h"
#include "gsig.h"

/* state variables used during processing GDL input; note that
   per-word state has to be in struct trun_word. */
struct trun_gdl_state
{
  const char *file;	/* current input file */
  int F_done;
  int andline_num;
  int in_w;
  int in_xcl;
  int note_skip;
  int printing;
  List *k;
};

/* This should be allocated for each sH/g:w; for line-oriented
   programs it can be reset rather than reallocated.  trun_word
   provides a management structure for grapheme signatures.
 */
typedef struct trun_word
{
  struct gsig *gpp;
  int gpp_alloced;
  int gpp_used;
  struct lsig *lpp; /* A single orthographic word-form can expand to
		       multiple lemmata with COFs */
  int lpp_alloced;
  int lpp_used;
  int no_d_index;
  int wgp_c_index;
  int in_c;
  int in_n;
  int in_p;
  int in_q;
  char role;
  const char *roletext;
  Wloc *w;
} Word;

/* This should be allocated for each input */
typedef struct trun
{
  int multi;
  Memo *t_m;
  Memo *l_m;
  Memo *w_m;
  Memo *c_m;
  Memo *a_m;
  Pool *p;
  struct trun_gdl_state rs;
  struct trun_word *rw;
  struct loch *l;
  FILE *o; 		/* output for run */
} Trun;

#endif/*TRUN_H_*/
