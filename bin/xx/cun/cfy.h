#ifndef CFY_H_
#define CFY_H_

#include <oraccsys.h>
#include <setjmp.h>

struct cell;
struct class;
struct line;
struct eltline;

/* Global management structure */
typedef struct Cfy
{
  const char *n;
  const char *project;
  const char *pqx;
  Pool *p;
  Pool *hp;
  Hash *hclasses;
  Hash *hsubhead;
  Hash *hsubkeys;
  Hash *hfonts;
  Memo *m_assignment;
  Memo *m_class;
  Memo *m_line;
  Memo *m_cell;
  Memo *m_elt;
  Memo *m_eltline;
  Memo *m_subspec;
  List *body; 	/* list of line or cline pointers for actual output */
  List *line; 	/* list of Elt built by cfy_reader */
  List *cline; 	/* colon-line, i.e., grapheme sequence to use instead
		   of 'line'; one day this might align with
		   'line'--needs ATF support */
  struct eltline **elt_lines; /* NULL-terminated array of lines rewritten as
				 NULL-terminated arrays of Eltline* */
  struct class *c;	/* the class for the initial state */
  FILE *o; 	/* output fp */
  const char *fnt; /* font from CLI -p [period] arg */
  const char *key; /* CLI -k arg */
  const char *config; /* config file name */
  int html;
  int weboutput;
} Cfy;

/* Cuneify output uses the HTML class attribute to render cuneiform
   with different fonts (fnt), script sets (set), and an appropriate
   default magnification for the font (mag). It may also use script
   (scr) information to manage splits and mergers. If the cuneified
   grapheme should link to anything other than osl the project name
   can be given in an asl attribute.

   For TeX the implementation uses macros to achieve the same ends.

   Fonts usually have ligatures, which are managed via the lig hash.

   A class key is a string where each member is given joined by
   dashes, using '*' for absent/wildcard members.  It is used as a
   hash key to test if a specific class combination is already loaded.

   Example: gudea-ss01-150-middle-osl

   When class data is read from attributes any missing members are
   provided from the current class, starting with the system default
   at the root of the process.

   The key is generated and if the class is not already in the hash it
   is added. For fonts, the first time a font is loaded it is added to
   the font hash so fonts are only loaded once regardless of how many
   combinations of mag and script etc., may be used in the run.
 */
typedef struct class
{
  const char *key;
  const char *fnt;
  const char *otf;
  const char *mag;
  const char *scr;
  const char *asl;
  const char *css;
  Hash **lig; /* ligatures are an array of hashes essentially implementing a trie */
} Class;

extern Class *curr_cp;

/* An element is an input item such as a grapheme, ellipsis, ZWNJ,
 * ZWJ, or word-boundary.
 *
 * Support for justifying lines is provided by the FILL type;
 * this will work like TeX's hfil to enable simulation of spacing to
 * the left of graphemes in a continuation line or between graphemes.
 *
 * The ATF soft newline ';', indicating an indented line, is captured
 * in the RETURN type. This is always followed by a FILL.
 *
 * The output structure consists of spans which manage the breakage
 * state. Within each span is a sequence of graphemes, ellipses, and
 * word-spaces, ZWNJ, or ZWJ.
 *
 * The input is read into an array and is only output when the next
 * XTF line is encountered or at the closing XTF tag in the case of
 * the last line.
 *
 * If a reordering line is encountered, that is used as the sequence
 * for grapheme output. Otherwise the output follows the main
 * transliteration stream, obeying any local reordering of graphemes
 * that may be present.
 *
 * Breakage state is recorded in the grapheme structures:
 * conceptually, the states are broken; damaged; or clear.
 *
 * If the font referenced by the current class provides a ligature
 * table, that is checked for immediately adjacent sequences that are
 * in the ligtable--sequences are broken by word boundaries and by
 * ZWNJ.
 *
 * If a ligtable sequence is matched, the breakage state is adjusted
 * to ensure that the ligature falls within the enclosing breakage
 * span. If a sequence mixes breakage states, all of the breakage is
 * set to damaged as an approximation of the state of the ligature
 * sequence as a whole.
 */
typedef enum elt_type { ELT_NOT,
			ELT_L /*line*/,
			ELT_C /*cell*/,
			ELT_W /*word*/,
			ELT_G /*grapheme*/,
			ELT_J /*ZWJ*/,
			ELT_N /*ZWNJ*/,
			ELT_F /*fill*/,
			ELT_R /*return*/,
			ELT_E /*ellipsis (...) in input*/,
			ELT_X /*'x' in input*/,
			ELT_D /*deleted, for ligatures*/,
			ELT_Q /*Quoted literal*/,
			ELT_A /*Assignment, used in subbing rules */,
			ELT_LAST
} Etype;

extern int espaces[];

extern const char *brk_str[];

typedef enum btype { BRK_NOT,
		     BRK_NONE /*clear*/,
		     BRK_HASH /*damaged*/,
		     BRK_LOST /*broken*/
} Btype;

typedef enum gtype { G_NOT,
		     G_V /*g:v value*/,
		     G_S /*g:s sign*/,
		     G_N /*g:n number*/,
		     G_Q /*seQuence member from split @ucun*/,
		     G_X /*none of the above but not ...*/,
		     G_U /*...unknown reading, i.e., $MU notation*/
} Gtype;

typedef struct elt
{
  Etype etype;	  /* the element type */
  Gtype gtype;    /* the grapheme type */
  Btype btype;    /* the breakage type */
  void *data;	  /* cuneiform, Line, or Cell */
  Class *c;	  /* the current class for the grapheme; usually set
		   at start of file but may be switched grapheme by
		   grapheme */
  const char *g_o; /* openers from @g:o */
  const char *g_c; /* closers from @g:c */
  const char *oid;/* OID for linking to sign list: may be a parent
		     sign to the sign that is displayed in u8 */
  const char *xid;/* grapheme id to be output as ref */
  const char *key;/* ASL grapheme key for type ELT_G */
} Elt;

typedef struct line
{
  const char *xid;
  const char *label;
} Line;

typedef struct eltline
{
  Elt **epp;
  int len;
} Eltline;

typedef struct cell
{
  int span;
} Cell;

/* Cuneify config defines substition specifications that are stored in
   this structure */
typedef struct subspec
{
  int start; 	/* did the left spec begin with '^' */
  int end;	/* did the left spec end with '$' */
  Elt **l;	/* left elements */
  Elt **r;	/* right elements */
  int l_len;
  int r_len;
  int terminal; /* 1 if this is a terminal match */
  int has_assignment; /* 1 if assignments are used in the subspec */
  Elt **lrefs;	/* when eltrefs are used in the config they are
		   accessed through these pointers to clones */
} Subspec;

typedef enum ttype { T_NOT, T_OFF, T_ELT, T_BRK, T_G } Ttype;

typedef enum vtype { V_NOT, V_INT, V_STR } Vtype;

typedef struct assignment
{
  int lindex; 		/* index into lhs of sub rule */
  uintptr_t offof; 	/* offsetof of member; UINTPTR_MAX if not set */
  Ttype toktype;	/* token type so we can trap mismatches
			   between the member and the value token */
  Vtype valtype;	/* value type */
  void *value; 		/* value to assign; for vtype=V_INT this is (uintptr_t*)VALUE */
} Assignment;

/* Access the u8 member of the Elt in the data member of the List node lp */
#define elt_grapheme(lp)	(((Elt*)(lp)->data)->etype==ELT_G)
#define elt_btype(lp)		((Elt*)(lp)->data)->btype
#define elt_etype(lp)		((Elt*)(lp)->data)->etype
#define elt_cun(lp)		((Elt*)(lp)->data)->data
#define elt_line(lp)		(Line *)((Elt*)(lp)->data)->data
#define elt_cell(lp)		(Cell*)((Elt*)(lp)->data)->data
#define elt_oid(lp)		((Elt*)(lp)->data)->oid

extern Pool *p;
extern List *cqueue;
extern FILE *outfp;

struct perfnt
{
  const char *name;
  const char *fnt;
};
extern struct perfnt *perfnt(register const char *str, register size_t len);

struct subtok
{
  const char *name;
  size_t memb_or_val; /* offsetof Elt member or value to assign to a member */
  Ttype toktype;
  Vtype valtype;
};  
extern struct subtok *subtok(register const char *str, register size_t len);

extern jmp_buf done;
extern int anchor_start, anchor_end;
extern int trace, verbose;
extern Cfy cfy;
extern const char *brk_str[];

extern void cfy_eH(void *userData, const char *name);
extern void cfy_sH(void *userData, const char *name, const char **atts);
extern void cfy_out_html(Cfy *c);
extern void cfy_out_xml(Cfy *c);
extern Class *cfy_class(Cfy *c, const char *key, Class *cp);
extern void cfy_reader_init(void);
extern void cfy_render(Cfy *c, const char *om);
extern int file_args(const char *htmldir, const char *qpqx, const char *inext,
		     const char *outdir, const char *outext, const char *trans,
		     char **inp, char **outp, char **hdir);
extern Hash **cfy_lig_load(const char *ligfile);
extern void cfy_lig_line(Cfy *c, List *lp);
extern void cfy_reset(void);
extern void cfy_breakage(Cfy *c, Elt **epp);
extern void cfy_ligatures(Cfy*c, Elt **epp);
extern void cfy_subbings(Cfy *c, Eltline *elp);

extern void cfy_cfg_elt_g(Mloc m, Cfy *c, uccp g);
extern void cfy_cfg_elt_f(Mloc m, Cfy *c);
extern void cfy_cfg_elt_l(Mloc m, Cfy *c);
extern void cfy_cfg_elt_r(Mloc m, Cfy *c);
extern void cfy_cfg_elt_w(Mloc m, Cfy *c);
extern void cfy_cfg_elt_j(Mloc m, Cfy *c);
extern void cfy_cfg_elt_n(Mloc m, Cfy *c);
extern void cfy_cfg_elt_q(Mloc m, Cfy *c, uccp q);

extern int cfy_cfg_load(Cfy *c, const char *cfgfile);
extern void cfy_cfg_stash(Mloc m, Cfy *c);
extern const char *elts_one_key(Elt *e);
extern void cfy_cfg_asgn(Mloc m, Cfy *c, int nth, const char *memb, const char *val);

#endif/*CFY_H_*/
