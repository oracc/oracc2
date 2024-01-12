#ifndef TRUN_H_
#define TRUN_H_

#include "loch.h"
#include "gsig.h"

/* state variables used during processing GDL input */
struct trun_gdl_state
{
  const char *file;
  int andline_num;
  int in_c;
  int in_n;
  int in_p;
  int in_q;
  int in_xcl;
  int no_d_index;
  struct gsig *curr_c_wgp;
  char role;
  const char *roletext;
};

/* This should be allocated for each sH/g:w; for line-oriented
   programs it can be reset rather than reallocated.  gsb_word
   provides a management structure for grapheme signatures.
 */
typedef struct trun_word
{
  struct gsig *gpp;
  int gpp_alloced;
  int gpp_used;
  Wloc *w;
} Word;

/* This should be allocated for each input */
typedef struct trun
{
  Memo *t_m;
  Memo *l_m;
  Memo *w_m;
  Pool *p;
  struct trun_gdl_state rs;
  struct trun_word rw;
  int multi;
  struct loch *l;
} Trun;

#endif/*TRUN_H_*/
