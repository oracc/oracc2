#ifndef _BLOCKTOK_H
#define _BLOCKTOK_H

/* This used to be from xmlnames.h when the XTF schema was built in to ox */

enum e_type {
  e_column, e_composite, e_div, e_enum_top, e_h, e_include, e_m, e_object,
  e_referto, e_score, e_surface, e_transliteration, e_variant, e_variants,
  e_xh_div, e_xh_h1, e_xh_h2, e_xh_h3, e_xh_span, e_xtr_translation,

  /* Non-x support for $-lines and #-lines */
  e_nonx, e_cmt,
  
  /* additional types for lines */
  e_l_group , e_l_mts , e_l_nts , e_l_lgs , e_l_gus , e_l_bil , e_l_xmp ,

  /* sub-line content & , word nonw */
  e_l_cell , e_l_fld , e_l_wrd , e_l_now ,

  e_top
};

enum block_levels { 
  B_XTF, B_TEXT , B_OBJECT, B_SURFACE, B_COLUMN, B_LINE, B_CELL, B_FIELD,
  B_AGROUP, B_WORD, B_GRAPHEME , B_DIVISION , B_MILESTONE, B_H1, B_H2, B_H3,
  B_TR_TRANS, B_TR_SWITCH, B_TR_LABEL, B_TR_UNIT, B_TR_SPAN, B_TR_NOTE,
  B_TR_H1, B_TR_H2, B_TR_H3, B_bl_top
};

enum e_tu_types
{
  etu_trans , etu_note , 
  etu_h1, etu_h2, etu_h3 ,
  etu_label , etu_unit , etu_span,
  etu_dollar , 
  etu_interlinear , etu_parallel , etu_unitary , etu_labeled ,
  etu_top
};

#define C(x) x,

/* h=hash q=query b=bang s=star */
#define F0 C(f_none)
#define F1 C(f_h)C(f_q)C(f_b)C(f_s)
#define F2 C(f_hq)C(f_hb)C(f_hs)C(f_qb)C(f_qs)C(f_bs)
#define F3 C(f_hqb)C(f_hbs)C(f_hqs)C(f_qbs)
#define F4 C(f_hqbs)

enum f_type { F0 F1 F2 F3 F4 f_type_top };

/* h=uflag1 q=uflag2 b=uflag3 s=uflag4 */
#define UF0 C(uf_none)
#define UF1 C(uf_h)C(uf_q)C(uf_b)C(uf_s)
#define UF2 C(uf_hq)C(uf_hb)C(uf_hs)C(uf_qb)C(uf_qs)C(uf_bs)
#define UF3 C(uf_hqb)C(uf_hbs)C(uf_hqs)C(uf_qbs)
#define UF4 C(uf_hqbs)

enum uf_type { UF0 UF1 UF2 UF3 UF4 uf_type_top };

#undef C

struct flags
{
  enum f_type type;
  int h;
  int q;
  int b;
  int s;
  int nattr;
  /*struct token *t;*/
  const char *atf;
  /*struct fattr a[4];*/
};

struct uflags
{
  enum uf_type type;
  int h;
  int q;
  int b;
  int s;
  int nattr;
  /*struct token *t;*/
  const char *atf;
  /*struct fattr a[4];*/
};

struct block_token
{
  const char *name;
  enum e_type etype;
  const char *n;
  enum block_levels type;
  const char *full;
  const char *abbr;
  const char *nano;
};
extern struct block_token *blocktok(register const char*str,
				    register size_t len);
#endif /*_BLOCKTOK_H*/
