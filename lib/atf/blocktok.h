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
