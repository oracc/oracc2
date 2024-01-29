#ifndef OSE_TYPES_H_
#define OSE_TYPES_H_ 1
#include <limits.h>
#include <stdint.h>

typedef unsigned char  Char;
typedef uint32_t       Four_bytes;
typedef uint16_t       Two_bytes;
typedef int16_t        Two_bytes_s;
typedef unsigned char  One_byte;
typedef signed char    One_byte_s;


#define PQ_MAX 		0x00200000
#define PQ_BIT 		(1<<31)
#define setQ(x)		((x)|=PQ_BIT)
#define isQ(t)		((t)&PQ_BIT)

#define TWO_BYTES_MAX 65535
#define TWO_BYTES_s_MAX 32767
#define TEXT_ID_MAX	    	PQ_MAX
#define UNIT_ID_MAX		TWO_BYTES_MAX
#define WORD_ID_MAX	      	TWO_BYTES_MAX
#define START_COLUMN_MAX      	TWO_BYTES_MAX
#define END_COLUMN_MAX        	TWO_BYTES_MAX
#define BRANCH_ID_MAX	      	TWO_BYTES_s_MAX
#define FOUR_BYTES_MAX		USHORT_MAX

#define TEXTID_MASK 	0xffffff
#define PQVal(t)	((t)&PQ_BIT)
#define idVal(t)	((int)((t)&TEXTID_MASK))

#define X_FACTOR	2000000u /* X-numbers are P-numbers of 2 million and above */
#define XIFY_ID(x)     	((x)+X_FACTOR)
#define UNEX_ID(x)	((idVal(x))-X_FACTOR)

#define loc8 \
  Four_bytes text_id; \
  Two_bytes  unit_id; \
  Two_bytes  word_id; \

struct location8
{
  loc8
};

struct location16
{
  loc8
  Two_bytes_s branch_id;
  Two_bytes   start_column;
  Two_bytes   end_column;
  Two_bytes   properties;
};

struct location24
{
  loc8
  Two_bytes_s branch_id;
  Two_bytes   start_column;
  Two_bytes   end_column;
  Two_bytes   properties;
  Two_bytes   sentence_id;
  Two_bytes   clause_id;
  Two_bytes   phrase_id;
  Two_bytes   lemma_id;
};

#undef loc8

/* These must be ordered so that earlier items have larger granularity */
enum datum_type { d_cat , d_cbd , d_tra , d_lem , d_tok , d_txt , d_uni , d_NONE };

enum result_granularity 
  {
    g_text , g_record , g_field , g_word , g_grapheme , 
    g_not_set
  };

enum result_id_domain
  {
    i_text , i_composite , i_biblio
  };

#endif /*OSE_TYPES_H_*/
