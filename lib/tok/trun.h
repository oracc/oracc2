#ifndef TRUN_H_
#define TRUN_H_

/* This should be allocated for each input */
struct trun
{
  struct loch *l;
  Pool *p;
  struct trun_gdl_state s;
} Trun;

/* state variables used during processing GDL input */
struct trun_gdl_state
{
  int in_c;
  int in_n;
  int in_p;
  int in_q;
  char role;
  const char *roletext;
};

/* This should be allocated for each sH/g:w; for line-oriented
   programs it can be reset rather than reallocated.  gsb_word
   provides a management structure for grapheme signatures.
 */
struct gsb_word
{
  struct gsig *gpp;
  struct gsig *curr_c_wgp;
  int gpp_alloced;
  int gpp_used;
  int no_d_index;
  struct gsb_input *in;
};

#endif/*TRUN_H_*/
