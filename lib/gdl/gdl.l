%option prefix="gdl"
%option never-interactive noyywrap yylineno nounput

%{
#include <ctype.h>
#include <mesg.h>
#include <pool.h>
#include <sll.h>
#include "gdl.h"
#include "gdlstate.h"

/* test */

#define GDLLTYPE Mloc
#include "gdl.tab.h"
#define YY_USER_ACTION gdl_user_action();
extern Mloc gdllloc;
const char *gdlfile = "<stdin>";
const char *currgdlfile = NULL;
extern const char *gdl_lexnames[];
extern int gdl_lexfld[];
extern int gdl_bilingual, gdl_legacy, gdl_lexical, gdl_unicode;
int gdlflextrace = 0;
int g_literal_flag = 0, g_logoforce_flag = 0;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static void gdl_user_action(void);
static int gdl_grapheme_type(void);
static int CIN = 0; /* 1 if in compound */ /* update: this is probably no longer necessary */
static int GIN = 0; /* 1 if in grapheme */
static int QIN = 0; /* non-zero if in a(BA) qualifier */
static int SIN = 0; /* left context is space */

#define gconst gdllval.text=gdltext
#define gsave  gdllval.text=gdlsave(gdltext)
#define gsaveg(garg)  gdllval.text=gdlsave((garg))
int term = 0;
#define yyterminate() return (term = !term) ? END : YY_NULL

/* Simple stack for nesting of compounds and valuquals */
#define CVQ_MAX 6
static int cvq_stack[CVQ_MAX] = { 0, 0, 0, 0, 0, 0 };
static int cvq_top = -1;
static int cvq_pop(void);
static int cvq_push(int);
static void cvq_reset(void);
/* LISTNUM		([A-Z][A-Z]+[0-9]+[A-Z0-9a-z_'"/+*]*) */
/* NUMGRAPH	[0-9XNn]+(\/[0-9])?\(({GRAPHEME}|{COMPOUND})\) */
%}

	     	/* 20260519 remove '*' because it conflicts with collated flag */
L		([A-Z][A-Z]+[0-9][0-9][0-9]+[A-Z0-9a-z_'"/+^]*(′|″)*){M}*
B		("["|"]"|\xe2\xb8[\xa2-\xa5])
S		\xe2\xb8[^\xa2-\xa5]|([0-9][a-wyz]*)
G		([a-ru-zA-RU-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|[stST],?|{S}|\xc3[^\x97]|\xe2[^\xb8\x88])+
CG		([A-RU-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|[ST],?|{S}|\xc3[^\x97]|\xe2[^\xb8\x88])+
M		("@"([a-wyz]+[0-9]*|90|180|270)|"~"[a-z0-9]+|"\\"[a-z]*[0-9]+)
NNUM		([N][0-9][0-9]?[ACF]?)
GRAPHEME 	({G}|"15")
FRACTION	[0-9]\/[0-9]

L_UHS		"\xe2\xb8\xa2"
R_UHS		"\xe2\xb8\xa3"
L_LHS		"\xe2\xb8\xa4"
R_LHS		"\xe2\xb8\xa5"

UFLAG	"\xe2\x80"[\xa0\xa1\xa2\xa3]

NOTEMARK	"^"[0-9]+"^"

%x C
%s Q
%s S
%x TC
%x TD

%%

[=^][aeiuAEIU]		{ if (gdl_unicode) { mesg_vwarning(currgdlfile,gdllineno,
						"non-Unicode accent codes %s",gdltext); }}

{G}({G}|{B})*{G}	{ GIN=1; return gdl_grapheme_type(); }
{G}({G}|{B})*/{M}	{ GIN=1; return gdl_grapheme_type(); }
{GRAPHEME} 		{ GIN=1; return gdl_grapheme_type(); }

"|"/[ \t]	       	{ /*"*/ gdllval.text=(char*)gdl_lexnames[(int)*yytext]; return gdl_lexfld[(int)*yytext]; }
"|"			{ BEGIN cvq_push(C); GIN=0; CIN=1; return C_O; }
<C>"|"/[ \t]   		{ BEGIN cvq_pop(); CIN=0; return C_C; }
<C>"|"			{ BEGIN cvq_pop(); CIN=0; return C_C; }
<C>"\xe2\x88\x98"	{ GIN=0; return C_CIRCLE; }
<C>{NNUM}/{M} 		{ GIN=1; gsave; return NNUM; }
<C>{NNUM} 		{ GIN=1; gsave; return NNUM; }
<C>{CG}			{ GIN=1; return gdl_grapheme_type(); }
<C>{CG}({CG}|{B})*{CG}	{ GIN=1; return gdl_grapheme_type(); }
<C>{CG}({CG}|{B})*/{M}	{ GIN=1; return gdl_grapheme_type(); }
<C>{M}			{ GIN=1; gsave; return MOD; }
<C>[0-9nN]+(\/[0-9]+)?/"("	{ GIN=1; gsave; return NUMBER; }
<C>"."			{ GIN=0; return C_PERIOD; }
<C>"+"			{ GIN=0; return C_PLUS; }
<C>":"/"("	       	{ GIN=0; return C_COLON; }
<C>":"			{ GIN=0; return C_COLON; }
<C>"&"			{ GIN=0; return C_ABOVE; }
<C>"%"			{ GIN=0; return C_CROSSING; }
<C>@/[^a-z0-9]+		{ GIN=0; return C_OPPOSING; }
<C>"4\xc3\x97"		{ GIN=0; return C_4TIMES; }
<C>"3\xc3\x97"		{ GIN=0; return C_3TIMES; }
<C>"\xc3\x97"	       	{ GIN=0; return C_TIMES; }
<C>"x"	       		{ GIN=0; return C_TIMES; }
<C>[a-wyz]    		{ GIN=0; mesg_vwarning(currgdlfile,gdllineno,"lowercase graphemes not allowed in |...|"); }
<C>"("			{ return GIN ? QLP : CLP; }
<C>")"			{ return CRP; }
<C>[ \t]+	       	{ CIN=GIN=0; mesg_vwarning(currgdlfile,gdllineno,"missing '|' in compound");
			  cvq_reset(); }

<Q>")"/[}>]	       	{ return QRP; }
<Q>")"			{ return QRP; }
<Q>[ \t]+	       	{ QIN=GIN=0; mesg_vwarning(currgdlfile,gdllineno,"missing ')' in qv");
			  cvq_reset(); }

":"/[ \t\n\(]  		{ /*"*/ GIN=1; gsave; return PUNCT; }
":."			{ /*"*/ GIN=1; gsave; return PUNCT; }
":'"			{ /*"*/ GIN=1; gsave; return PUNCT; }
":\""			{ /*"*/ GIN=1; gsave; return PUNCT; }
"::"      		{ /*"*/ GIN=1; gsave; return PUNCT; }
"/"/[ \t\n\(]		{ /*"*/ GIN=1; gsave; return PUNCT; }

{L}			{ gsave; return LISTNUM; }

[0-9nN]+(\/[0-9]+)?/"("	{ GIN=1; gsave; return NUMBER; }

{NNUM}/{M}	       	{ GIN=1; gsave; return NNUM; }
{NNUM} 			{ GIN=1; gsave; return NNUM; }

[0-9]+(\/[0-9]+)?	{ GIN=1; gsave; return !strncmp(gdllval.text,"00",2) ? ZERO : BARENUM; }

{M}			{ GIN=1; gsave; return MOD; }

	/* Determinatives and glosses */

"{{"[0-9+]:	       	{ gconst; GIN=0; return e_L_dbl_cur; }
"{{"			{ gconst; GIN=0; return e_L_dbl_cur; }
"}}"			{ gconst; GIN=0; return e_R_dbl_cur; }
"{("			{ gconst; GIN=0; return e_L_cur_par; }
")}"			{ gconst; GIN=0; return e_R_cur_par; }
"(x("			{ gconst; GIN=0; return e_L_par_par; }
")x)"			{ gconst; GIN=0; return e_R_par_par; }
"{"			{ GIN=0; return '{'; }
"{+"			{ GIN=0; return DET_PHON; }
"{-"			{ GIN=0; return DET_SEME; }
"}"			{ GIN=0; return '}'; }

,			{ return ','; }
&[0-9]+			{ gsave; return CELLSPAN; }
&			{ return '&'; }
![a-z]+			{ gsave; return FTYPE; }

	/* Delimiters */

"..."			{ GIN=0; gsave; return ELLIPSIS; }
"--"			{ GIN=0; return ENHYPHEN; }
[-+:.]     		{ GIN=0; return gdltext[0]; }

"/"/{G}			{ return '/'; } /* alternation */

"//"			{ return LINEBREAK; }

[ \t]+/[#"~=@^]	       	{ GIN=0; SIN=1; gsave; return SPACE; }
[ \t]+			{ GIN=0; gsave; return SPACE; }

	/* Inline comment/dollar */

"($"			{ BEGIN TD; return L_inl_dol; }
<TD>"$)"	       	{ BEGIN 0; return R_inl_dol; }

"(#"			{ BEGIN TC; return L_inl_cmt; }
<TC>"#)"       		{ BEGIN 0; return R_inl_cmt; }

<TD>([^$]|\$[^)])*     	{ gsave; return TEXT; }
<TC>([^#]|\#[^)])*   	{ gsave; return TEXT; }

	/* Anything in this section either does not return a token or
 	   only does so in some circumstances */

"("			{ if (GIN) { GIN=0; return QLP; } else { gdl_state_o(e_L_par); } }
")"			{ GIN=0; if (QIN) { return QRP; } else { gdl_state_c(e_R_par); } }

<INITIAL,C>"<("		{ GIN=0; gdl_state_o(e_L_ang_par); }
<INITIAL,C>")>"	       	{ GIN=0; gdl_state_c(e_R_ang_par); }
<INITIAL,C>"<<"		{ GIN=0; gdl_state_o(e_L_dbl_ang); }
<INITIAL,C>">>"		{ GIN=0; gdl_state_c(e_R_dbl_ang); }
<INITIAL,C>"<"		{ GIN=0; gdl_state_o(e_L_ang); }
<INITIAL,C>">"		{ GIN=0; gdl_state_c(e_R_ang); }
<INITIAL,C>"["		{ GIN=0; gdl_break_o(e_L_squ); }
<INITIAL,C>"]"		{ GIN=0; gdl_break_c(e_R_squ); }
<INITIAL,C>{L_UHS}	{ GIN=0; gdl_break_o(e_L_uhs); }
<INITIAL,C>{R_UHS}	{ GIN=0; gdl_break_c(e_R_uhs); }
<INITIAL,C>{L_LHS}	{ GIN=0; gdl_break_o(e_L_lhs); }
<INITIAL,C>{R_LHS}	{ GIN=0; gdl_break_c(e_R_lhs); }

	/* The meta-delimiter ';' == indented line */
<INITIAL,C>";"	       	{ GIN=0; gdl_indent(); }

	/* Sign state force characters */
~~/{G}			{ g_logoforce_flag = 1; }
"~"/{G}			{ if (gdl_lexical) { gsave; mesg_vwarning(currgdlfile, gdllineno,
			   "no space after ~ field; use ~~ for logogram force with atf: use lexical\n");
			     gdllval.text=(char*)gdl_lexnames[(int)*yytext]; return gdl_lexfld[(int)*yytext];
			   } else { g_logoforce_flag = 1; } }
"$"/{G}			{ g_literal_flag = 1; }

	/* Grapheme meta flags, sometimes doubling as lex field shorthands */
<INITIAL,C,Q>[!?]      	{ gdl_lex_flag((uccp)yytext); }
<INITIAL,C>"*"		{ /*"*/ if (GIN) { gdl_lex_flag((uccp)yytext); } else { GIN=1; gsave; return(PUNCT); } }
<INITIAL,C>^[#"~=@^]	{ return gdl_lexfld[(int)*yytext]; }
"#"			{ if (SIN) { SIN=0;
				        gdllval.text=(char*)gdl_lexnames[(int)*yytext];
			  		return gdl_lexfld[(int)*yytext]; } else { gdl_lex_flag((uccp)yytext); } }
<C>"#"			{ gdl_lex_flag((uccp)yytext); }
[:"~=@^]   		{ if (SIN) { SIN=0;
					gdllval.text=(char*)gdl_lexnames[(int)*yytext];
					return gdl_lexfld[(int)*yytext]; } else { gdl_lex_flag((uccp)yytext); } }
[#"=@^]/{G} 		{ if (gdl_lexical) {
		       	    mesg_vwarning(currgdlfile, gdllineno,
			  	       	  "no space after %c field\n", *yytext);
			    return gdl_lexfld[(int)*yytext]; }
			  else { gdl_lex_flag((uccp)yytext); } }

{UFLAG}			{ gdl_lex_flag((uccp)yytext); }

	/* Language shifts */
%[-a-z0-9]+("/"[gn])?	{ gsave; gdl_lang_tag(yytext); }
_			{ gsave; gdl_lang_flip(); }

	/* Note marks */
{NOTEMARK}		{ gdl_note_mark(gdltext); }

	/* Inline variation note marks */
"("[0-9]+::[0-9]+")"   	{ gsave; GIN=0; gdl_var_mark(e_LR_var, gdllval.text); }
"("[0-9]+:		{ gsave; GIN=0; gdl_var_mark(e_L_var, gdllval.text); }
:[0-9+]")"		{ gsave; GIN=0; gdl_var_mark(e_R_var, gdllval.text); }

	/* General administrative rules */

\n			{ return '\n'; }

"\xc3\x97"	       	{ mesg_vwarning(currgdlfile, gdllineno, "gdl.l: misplaced ×\n"); }

.			{ mesg_vwarning(currgdlfile, gdllineno, "gdl.l: unmatched character %c\n", *gdltext); }

<<EOF>>			{ gdl_line_wrapup(gdllloc); yyterminate(); }

%%

void
gdl_lex_closers(void)
{
  if (CIN)
      mesg_vwarning(currgdlfile,gdllineno,"missing '|' in compound (discovered at end of line)");
  else if (QIN)
    mesg_vwarning(currgdlfile,gdllineno,"missing ')' in qv (discovered at end of line)");
  GIN=CIN=QIN=0;
  cvq_reset();
}

int
gdl_grapheme_type(void)
{
  char *g = gdltext;
  gsaveg(g);

  if (strlen(g) == 1
      && ('n' == *g || 'N' == *g))
      return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
  else if (isdigit(*g))
    {
      char *d = g+1;
      while (*d && isdigit(*d))
        ++d;
      if (*d == '(')
        return NUMBER;
      else if (!strncmp(g, "00", 2))
	return ZERO;
      else
	return BARENUM;
    }
  else if (sll_is_signlist(g))
    return LISTNUM;
  else
    return GRAPHEME;
}

void
gdl_setup_buffer(char *buf)
{
  (void)gdl_scan_string(buf);
}

void
gdl_wrapup_buffer(void)
{
  gdl_delete_buffer(YY_CURRENT_BUFFER);
}

static void
gdl_user_action(void)
{
  if (gdlflextrace)
    fprintf(stderr, "%s:%d: gdllex: YY_START=%d; text=::%s:: GIN=%d; CIN=%d; QIN=%d; SIN=%d\n",
		  gdlfile, gdllineno, YY_START, gdltext, GIN, CIN, QIN, SIN);

#if 1
 /*  SIN=0; */
#else
  if (SIN)
    {
      BEGIN cvq_push(S);
      SIN=0;
    }
  else
    BEGIN cvq_pop();
#endif

  gdllloc.file = currgdlfile;
  gdllloc.line = gdllineno;
}

void
gdl_lex_init(const char *file, int line)
{
  CIN = GIN = QIN = 0;
  cvq_top = -1;
  currgdlfile = gdlfile = file;
  gdllineno = line;
}

void
gdl_decr_qin(void)
{
  if (--QIN < 0)
    { /* underflow condition */
      QIN = 0;
      BEGIN 0;
    }
  else
    {
      BEGIN cvq_pop();
    }
  if (gdlflextrace)
    fprintf(stderr, "gdl_decr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

void
gdl_incr_qin(void)
{
  ++QIN;
  BEGIN cvq_push(Q);
  if (gdlflextrace)
    fprintf(stderr, "gdl_incr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

int
cvq_push(int s)
{
  if (++cvq_top < CVQ_MAX)
    {
      cvq_stack[cvq_top] = YY_START;
      return s;
    }
  else
    fprintf(stderr, "%s:%d: cvq_push: cvq stack overflow (CVQ_MAX=%d)\n", currgdlfile, gdllineno, CVQ_MAX);
  return 0;
}

int
cvq_pop(void)
{
  if (cvq_top >= 0)
    return cvq_stack[cvq_top--];
  else
    fprintf(stderr, "%s:%d: cvq_pop: cvq stack underflow\n", currgdlfile, gdllineno);
   return 0;
}

void
cvq_reset(void)
{
  cvq_top = -1;
  BEGIN 0;
}
