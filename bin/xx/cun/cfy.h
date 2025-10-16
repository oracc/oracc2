#ifndef CFY_H_
#define CFY_H_

#include <oraccsys.h>
#include <setjmp.h>
#include <tree.h>
#include <xml.h>

struct cell;
struct class;
struct line;
struct eltline;

/* Global management structure */
typedef struct cfy
{
  const char *n;
  const char *project;
  const char *proxy; /* set when text project is different from
			previously set project */
  const char *pqx;
  const char *infile;
  Pool *p;
  Pool *hp;
  Hash *hclasses;
  Hash *hconfigs; /* hash of config paths to loaded config structures */
  Hash *hfonts;
  Hash *hlgs;
  Memo *m_cfg;
  Memo *m_class;
  Memo *m_fnt;
  Memo *m_heading;
  Memo *m_line;
  Memo *m_cell;
  Memo *m_div;
  Memo *m_elt;
  Memo *m_eltline;
  Tree *body; 	/* The body of the composite or transliteration */
  List *divs_with_lines;
  List *lines;  /* The list belonging to the body-node where lines should attach */
  List *mline; 	/* mts-line, list of Elt built by cfy_reader */
  List *cline; 	/* colon-line, i.e., grapheme sequence to use instead
		   of 'mline'; doesn't align with 'mline' but
		   cfylgs.sh provides a crude bolt-on alignment */
  List *line;	/* current line to add elts to  */
  struct class *c;	/* the class for the text-wide state; does not
			   change as inline classes change */
  struct class *fontclasses[100]; /* array of classes where [1] is class for font switch %01 etc */
  FILE *o; 	/* output fp */
  const char *fnt; /* font from CLI -p [period] arg */
  const char *key; /* CLI -k arg */
  const char *arg_ccf;		/* from the CLI */
  const char *period_ccf;	/* from the perfnt table */
  const char *project_ccf;	/* from the project */
  const char *protocol_ccf; 	/* from the text */
  const char *proxy_ccf; 	/* from the proxy project if defined */
  const char *proxypro_ccf; 	/* from the text from the proxy project */
  const char *text_ccf;		/* from the hash of texts and ccfs in a ccf */
  struct xpd *xpd;
  struct cfg *cfg;		/* the active config data */
  int coverage;
  const char *cov_list;
  int bare;
  int html;
  int weboutput;
  int no_output;
} Cfy;

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
			ELT_H /*@h(eading)*/,
			ELT_L /*line*/,
			ELT_C /*cell*/,
			ELT_W /*word*/,
			ELT_G /*grapheme*/,
			ELT_J /*ZWJ*/,
			ELT_N /*ZWNJ*/,
			ELT_F /*fill*/,
			ELT_R /*return, ATF ';'*/,
			ELT_E /*ellipsis (...) in input*/,
			ELT_X /*'x' in input*/,
			ELT_D /*deleted, for ligatures*/,
			ELT_Q /*Quoted literal*/,
			ELT_A /*Assignment, used in subbing rules */,
			ELT_S /* ZW-space */,
			ELT_Sp/* space-pseudo-element */,
			ELT_Hp/* hyphen-pseudo-element */,
			ELT_Rb/* ruling-boxed */,
			ELT_Rl/* ruling-line */,
			ELT_Rc/* ruling-column */,
			ELT_Jc/* justify-centre */,
			ELT_Jl/* justify-left */,
			ELT_Jp/* justify-penult */,
			ELT_Jr/* justify-right */,
			ELT_Js/* justify-spread */,
			ELT_Jcp/* justify-char-penult */,
			ELT_Jcs/* justify-char-spread */,
			ELT_BC /* body-composite */,
			ELT_BT /* body-transliteration */,
			ELT_Bo /* body-object */,
			ELT_Bs /* body-surface */,
			ELT_Bc /* body-column */,
			ELT_LAST
} Etype;

typedef struct cfg
{
  const char *path; 	/* path to loaded file */
  const char *key; 	/* cfy-key */
  struct class *c;	/* Class for the key */
  const char *pfs[10];  /* percent-font-switch %10..%19 indexed as 0..9 */
  struct class *pc[10]; /* Classes for percent-font-switch */
  Memo *m_subspec;
  Memo *m_assignment;
  Hash *hsubhead;
  Hash *hsubkeys;
  Hash *pqxccf;		/* hash mapping (qualified) PQX to cfg files */
  Cfy *cfy;
} Cfg;

typedef struct fnt
{
  const char *name;
  const char *full;
  Hash *uni;
  Hash *uni_seen;
  Hash **ligs; /* ligatures are an array of hashes essentially implementing a trie */
} Fnt;

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

   In the 'asl' member of the key, the special value '.' means use the
   project sign list.
 */
typedef struct rule
{
  Etype e;
  const char *w; /* width */
  const char *c; /* colour */
  const char *s; /* style */
} Rule;

typedef struct class
{
  const char *key;
  const char *fnt;
  const char *ffs; /* font-feature-settings */
  const char *mag;
  const char *scr;
  const char *asl;
  const char *css;
  Rule rbox;
  Rule rline;
  Rule rcol;
  Etype justify;
  int ruledata;
  const char *width;
  const char *colwidths;
  Fnt *fntp; 
  Cfy *cfyp;
} Class;

extern Class *curr_cp;

extern int espaces[];

extern const char *brk_str[];

typedef enum btype { BRK_NONE /*clear*/,
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

typedef enum ltype { LOC_ARG,
		     LOC_PRJ,
		     LOC_TXT
} Ltype;

/* XTF-body types; in the input we say Xtfbody but when internalized
   and unified in the Cfy structures we call them Div */
typedef enum xbtype { XB_NONE , XB_CMP, XB_TRA,
		      XB_OBJ, XB_SRF, XB_COL, XB_TOP
} Xbtype;
typedef struct xtfbody
{
  const char *name;
  Xbtype b;
  Etype e;
  /*const char *xid;*/ /*needed?*/
  const char *text;
  List *lines;
  struct eltline **elt_lines; /* NULL-terminated array of lines rewritten as
				 NULL-terminated arrays of Eltline* */
} Xtfbody;
typedef Xtfbody Div;
extern struct xtfbody *xtfbody(register const char *str, register size_t len);

typedef struct elt
{
  Etype etype;	  /* the element type */
  Gtype gtype;    /* the grapheme type */
  Btype btype;    /* the breakage type */
  const char *atype;    /* type from an attribute, e.g., @type on g:nonw or @g:type on g:x */
  struct elt *prev;/* the previous element */
  void *data;	  /* cuneiform, Line, Cell, or Div */
  Class *c;	  /* the current class for the grapheme; usually set
		   at start of file but may be switched grapheme by
		   grapheme */
  const char *g_o; /* openers from @g:o */
  const char *g_c; /* closers from @g:c */
  const char *oid;/* OID for linking to sign list: may be a parent
		     sign to the sign that is displayed in u8 */
  const char *xid;/* grapheme id to be output as ref */
  const char *key;/* ASL grapheme key for type ELT_G */
  const char *title; /* value or sign name */
  const char *otf; /* from \1 or \cv01 etc */
  short line; /* don't need to store pi_file as long as cfy is a single-shot program */
} Elt;

typedef struct heading
{
  const char *text;
  const char *xid;
  int level;
} Heading;

typedef struct line
{
  const char *xid;
  const char *label;
  int last_w;
} Line;

typedef struct eltline
{
  Elt **epp;
  int len;
} Eltline;

typedef struct cell
{
  int span;
  const char *class;
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
  const char *ccf;
};
extern struct perfnt *perfnt(register const char *str, register size_t len);

struct fontname
{
  const char *name;
  const char *full;
};
extern struct fontname *fontname(register const char *str, register size_t len);

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
extern const char *html_css;

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
extern void cfy_cfg_elt_J(Mloc m, Cfy *c, uccp J);
extern void cfy_cfg_elt_n(Mloc m, Cfy *c);
extern void cfy_cfg_elt_q(Mloc m, Cfy *c, uccp q);
extern void cfy_cfg_elt_s(Mloc m, Cfy *c);
extern void cfy_cfg_elt_Jl(Mloc m, Cfy *c);
extern void cfy_cfg_elt_Jp(Mloc m, Cfy *c);
extern void cfy_cfg_elt_Jr(Mloc m, Cfy *c);
extern void cfy_cfg_elt_Js(Mloc m, Cfy *c);

extern int cfy_cfg_run(Cfy *c);
extern int cfy_cfg_text(Cfy *c);
extern int cfy_cfg_load(Cfy *c, const char *cfgfile);
extern void cfy_cfg_stash(Mloc m, Cfy *c);
extern const char *elts_one_key(Elt *e);
extern void cfy_cfg_asgn(Mloc m, Cfy *c, int nth, const char *memb, const char *val);
extern Fnt *cfy_class_fnt(Cfy *c, Class *ncp);

extern int cfy_cfg_key(Mloc m, Cfy *c, const char *k);
extern int cfy_cfg_justify(Mloc m, Cfy *c, Etype e);
extern void cfy_cfg_width(Mloc m, Cfy *c, const char *w);

extern int cfy_r(Mloc m, Cfy *c, Etype e, const char *unit, const char *style, const char *colour);

extern void cfy_uni_check(Cfy *c, uccp u);
extern Hash *cfy_uni_load(Cfy *c, const char *unifile);

extern void cfy_lgs(Cfy *c);

extern const char *cfy_justify_class(Etype j);
extern void cfy_charspace(Cfy *c, Eltline *elp);
extern Elt *elt_clone(Cfy *c, Elt *ep);

extern void cfy_cfg_col_unit(Mloc m, Cfy *c, const char *u);
extern void cfy_cfg_col_wrap(Mloc m, Cfy *c);

extern void cfy_cfg_font(Mloc m, Cfy *c, const char *pc, const char *nm);
extern char *cfy_class_key(const char *fnt, const char *otf, const char *mag, const char *scr, const char *asl);

extern Div *cfy_body(Cfy *c, Xtfbody *xp, const char **atts);
extern Tree *cfy_body_init(Cfy *c, const char *name);
extern void cfy_body_term(void);
extern void cfy_body_debug(Cfy *c);
extern void cfy_body_lines(Cfy *c);

extern int ci_i, ci_j;

extern Div *curr_div;

#endif/*CFY_H_*/
