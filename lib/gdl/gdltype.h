#ifndef GDLTYPE_H_
#define GDLTYPE_H_

#include <pool.h>
#include <tree.h>
#include <hash.h>
#include <stck.h>
#include <sll.h>
#include <gvl.h>
#include "gsort.h"

#ifndef uccp
#define uccp unsigned const char *
#endif

#ifndef ccp
#define ccp const char *
#endif

typedef enum bracket_e {
  e_L_none,
  e_L_squ, e_R_squ,
  e_L_cur, e_R_cur,
  e_L_par, e_R_par,
  e_L_cur_par, e_R_cur_par,
  e_L_par_par, e_R_par_par,
  e_L_par_exp_p, e_R_par_exp_p,
  e_L_par_exp_m, e_R_par_exp_m,
  e_L_ang, e_R_ang,
  e_L_dbl_ang, e_R_dbl_ang,
  e_L_dbl_cur, e_R_dbl_cur,
  e_L_ang_par, e_R_ang_par,
  e_L_ang_par_s, e_R_ang_par_s,
  e_L_uhs , e_R_uhs,
  e_L_lhs , e_R_lhs,
  e_L_inl_dol , e_R_inl_dol,
  e_L_inl_cmt , e_R_inl_cmt,
  e_L_var, e_R_var, e_LR_var,
  e_L_top
} Bracket_e;

typedef struct bracket
{
  int tok;
  gdlstate_t oc;
  gdlstate_t s;
  const char *str;
} Bracket;

#define GLP_O1	0x01
#define GLP_O2	0x02
#define GLP_C	0x04

extern Bracket bracket_data[]; /* safe because r/o */

enum gdlpropvals { GP_ATTRIBUTE, GP_IMPLICIT, GP_TRACKING,
		   GP_DET_SEME , GP_DET_SEMI, GP_DET_PHON,
		   GP_BREAK_FULL, GP_BREAK_PART,
		   GP_GLOSS_DOCGLO, GP_GLOSS_LING, GP_GLOSS_SURRO,
		   GP_STATE_IMPLIED, GP_STATE_MAYBE, GP_STATE_SUPPLIED, GP_STATE_EXCISED,
		   GP_SEXIFY, GP_STREAM , GP_INFO
};

typedef struct gdlstack
{
  int i;
  Node *np;
} Gstck;
#define gstck_i(x) ((Gstck*)(x))->i
#define gstck_np(x) ((Gstck*)(x))->np

typedef struct lcstack
{
  struct lang_context *lang;
  int state;
} Lstck;

/* function pointer type for switching gdl_grapheme_type between
   legacy and strict without impacting performance for strict-mode
   data */
typedef int (*gdl_grapheme_type_p)(char **cp);

/* same for gdl_delim which needs to process future state */
typedef Node *(*gdl_delim_p)(Tree *, const char *);

/* and for gdl_graph_node */
typedef Node *(*gdl_graph_node_p)(Mloc *, Tree *, const char *, const char *);

/* GDL state defs */
#define gs_empty	0x0000000000000000

#define gs_f_query	0x0000000000000001
#define gs_f_bang	0x0000000000000002
#define gs_f_star	0x0000000000000004
#define gs_f_hash	0x0000000000000008
#define gs_f_uf1	0x0000000000000010
#define gs_f_uf2	0x0000000000000020
#define gs_f_uf3	0x0000000000000040
#define gs_f_uf4	0x0000000000000080
#define gs_f_plus	0x0000000000000100

#define gs_damaged	0x0000000000000200
#define gs_lost		0x0000000000000400

#define gs_maybe	0x0000000000000800
#define gs_supplied	0x0000000000001000
#define gs_excised	0x0000000000002000
#define gs_implied	0x0000000000004000
#define gs_erased	0x0000000000008000
#define gs_cancelled	0x0000000000010000
#define gs_superposed	0x0000000000020000
#define gs_glolin	0x0000000000040000
#define gs_glodoc	0x0000000000080000
#define gs_det		0x0000000000100000

#define gs_damaged_o	0x0000000000200000
#define gs_lost_o	0x0000000000400000
#define gs_maybe_o	0x0000000000800000
#define gs_det_o	0x0000000001000000
#define gs_supplied_o	0x0000000002000000
#define gs_excised_o	0x0000000004000000
#define gs_implied_o	0x0000000008000000
#define gs_erased_o	0x0000000010000000
#define gs_cancelled_o	0x0000000020000000
#define gs_superposed_o	0x0000000040000000
#define gs_glolin_o	0x0000000080000000
#define gs_glodoc_o	0x0000000100000000

#define gs_damaged_c	0x0000000200000000
#define gs_lost_c	0x0000000400000000
#define gs_maybe_c	0x0000000800000000
#define gs_det_c	0x0000001000000000
#define gs_supplied_c	0x0000002000000000
#define gs_excised_c	0x0000004000000000
#define gs_implied_c	0x0000008000000000
#define gs_erased_c	0x0000010000000000
#define gs_cancelled_c	0x0000020000000000
#define gs_superposed_c	0x0000040000000000
#define gs_glolin_c	0x0000080000000000
#define gs_glodoc_c	0x0000100000000000

#define gs_g_semd_i	0x0000200000000000
#define gs_g_semd_e	0x0000400000000000
#define gs_g_phond	0x0000800000000000
#define gs_g_logo	0x0001000000000000
#define gs_g_syll	0x0002000000000000
#define gs_g_mixed	0x0004000000000000
#define gs_g_alpha	0x0008000000000000
#define gs_g_flogo1	0x0010000000000000
#define gs_g_flogo2	0x0020000000000000
#define gs_g_flogo3	0x0040000000000000
#define gs_g_undefined	0x0080000000000000
#define gs_force	0x0100000000000000

#define gs_surro	0x0200000000000000
#define gs_reserved0	0x0400000000000000
#define gs_group_o	0x0800000000000000
#define gs_group_c	0x1000000000000000
#define gs_reserved1	0x2000000000000000
#define gs_reserved2	0x4000000000000000
#define gs_reserved3	0x8000000000000000

typedef unsigned long gdlstate_t;

extern gdlstate_t gs_order_f[];
extern gdlstate_t gs_order_o[];
extern gdlstate_t gs_order_c[];

typedef struct GDL {
  Pool *gdlpool;
  Node *c_last_explicit_group_node;
  Node *gdl_group_attach;
  Node *lgp;   		/* last grapheme node pointer */
  Node *gdl_post_det_gp_attach;
  Node *gdl_recycled_word;
  int c_processing;
  int gdl_corrq;/* set to 1 when a vq has !, e.g., a!(BA) */
  int curr_lang;
  int deep_parse;
  int gdl_ascii;
  int gdl_bilingual;
  int gdl_legacy;
  int gdl_unicode;
  int gdl_lexical;
  int g_literal_flag;
  int g_logoforce_flag;
  int gdl_orig_mode;/* If this is non-zero we use orig graphemes in
		       reconstituting compounds rather than c10e */
  const unsigned char *gdl_legacy_prop;
  int gdl_break_pending;
  int gdl_legacy_accented;
  int gdl_legacy_hash;
  int gdl_legacy_pending;
  int gdl_state_pending;
  int gdl_cell_count;
  int nonw_found;
  Bracket_e gdl_legacy_o;
  Bracket_e gdl_legacy_c;
  gdlstate_t *lst;
  gdlstate_t *dst;
  gdlstate_t pst;
  gdlstate_t rst;
  int grapheme_id;
  int wid_base;
  char gdl_line_id[1024];
  char gdl_word_id[2048];
  char *gid_insertp;
  const char *gdl_pending_varo;
  const char *curr_field;
  const char *curr_sur_id;
  const char *word_lang_tag; /* initialized to "sux" by convention */
  List *wd_list;
  List *c_dangling_gps;
  List *c_explicit_gps;
  List *c_implicit_gps;
  List *modq;
  const char *c_last_implicit_delim;
  int c_delim_sentinel;
  int c_implicit_times_reset;
  Stck *break_stack;
  Stck *glosstck;
  Stck *lang_stack;
  Stck *state_stack;
  Stck *sstck;
  Memo *mgstck;
  Memo *mlstck;
  struct lang_context *gdl_lang_context;
} GDL;

#endif /*GDLTYPE_H_*/
