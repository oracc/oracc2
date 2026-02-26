#ifndef _BLOCKTOK_H
#define _BLOCKTOK_H

typedef enum block_levels {
  B_XTF, B_TEXT , B_OBJECT, B_SURFACE, B_COLUMN, B_LINE, B_CELL, B_FIELD,
  B_AGROUP, B_WORD, B_GRAPHEME , B_DIVISION , B_MILESTONE, B_H1, B_H2, B_H3,
  B_TR_TRANS, B_TR_SWITCH, B_TR_LABEL, B_TR_UNIT, B_TR_SPAN, B_TR_NOTE,
  B_TR_H1, B_TR_H2, B_TR_H3, B_bl_top
} Block_level;

enum e_tu_types
{
  etu_trans , etu_note , 
  etu_h1, etu_h2, etu_h3 ,
  etu_label , etu_unit , etu_span,
  etu_dollar , 
  etu_interlinear , etu_parallel , etu_unitary , etu_labeled ,
  etu_top
};

typedef struct block_token
{
  const char *name;
  int bison;
  const char *n;
  enum block_levels type;
  const char *full;
  const char *abbr;
  const char *nano;
} Blocktok;

extern struct block_token *blocktok(register const char*str,
				    register size_t len);
#endif /*_BLOCKTOK_H*/
