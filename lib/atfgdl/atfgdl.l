%option prefix="ag"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include <atf.h>
#include <gdl.h>
#include <gdlstate.h>
#include "ag.h"

#define AGLTYPE Mloc
#include "ag.tab.h"
#define YY_USER_ACTION ag_user_action();
extern Mloc agloc;
const char *agfile = "<stdin>";
const char *curragfile = NULL;
static int last_unmatched_line = 0;

int agflextrace = 0, last_aglineno;
static int s_next;
static int linktext, tra_text;
static char *xs;

#define atfpool(s) (char*)pool_copy((const unsigned char*)(s),atfmp->pool);
#define atfnormpool(s) xs=(char*)normalize_ws((uccp)s);aglval.text=atfpool(xs);free(xs);s=xs=NULL

static int ag_block(const char *atcmd);
static void ag_user_action(void);

#define atfsave aglval.text=atfpool(agtext)
#define atfnorm aglval.text=atfnormpool(agtext)

#define longinit longtext_init(atfmp->pool,agtext)
#define usesave curr_use_str=atfpool(agtext)

#if 1
#define r(x) x
#else
#define r(x) fprintf(stderr, "returning %d\n", x),x
#endif

/* test */

extern const char *gdl_lexnames[];
extern int gdl_lexfld[];
extern int gdl_bilingual, gdl_legacy, gdl_lexical, gdl_unicode;
int g_literal_flag = 0, g_logoforce_flag = 0;

static gdl_grapheme_type_p ag_gdl_grapheme_type;

extern Pool *ag_hpool;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static int ag_gdl_grapheme_type_l(void);
static int ag_gdl_grapheme_type_s(void);
static int CIN = 0; /* 1 if in compound */ /* update: this is probably no longer necessary */
static int GIN = 0; /* 1 if in grapheme */
static int QIN = 0; /* non-zero if in a(BA) qualifier */
static int SIN = 0; /* left context is space */

#define gconst aglval.text=agtext
#define gsave  aglval.text=gdlsave(agtext)
#define gsaveg(garg)  aglval.text=gdlsave((garg))
int term = 0;
#define yyterminate() return (term = !term) ? END : YY_NULL

/* Simple stack for nesting of compounds and valuquals */
#define CVQ_MAX 6
static int cvq_stack[CVQ_MAX] = { 0, 0, 0, 0, 0, 0 };
static int cvq_top = -1;
static int cvq_pop(void);
static int cvq_push(int);
static void cvq_reset(void);

#if 0
static int ag_gdl_init(void);
#endif

static int ag_line_save(const char *t);
%}
PQX	([PQX][0-9]{6})
BOM3	("\xef\xbb\xbf")
BOM2	("\xfe\xff")
LK_TOTO ">>"
LK_FROM "<<"
LK_PLUS "++"
LK_VBAR "||"
PROJECT [a-z][a-z0-9/]+
QID	{PROJECT}:{PQX}
LANG	[a-z][a-z][a-z]?(-[-a-z]+)?
use	"use"[ \t]+			
NOD 	[^ \t\n\r.:]
NON 	[^ \t\n\r]
WHT 	[ \t\n\r]
			/* */
TRA_CHR [@\"?\[\]\{\}]
TRA_CMD (@[a-z0-9]+("["[^\]]+"]")?)|@\?|\?@|@\"|\"@|\{|\}|\^[0-9]+\^
TRA_LAB ^@\([^)@]+\)
TRA_PAR [\n\r]+[ \t]*[\n\r]+
TRA_WHT [ \t\n\r]+
TRA_WRD [^ \n\r\t\@\"\?\[\]\{\}]+
	     	/* 20260519 remove '*' because it conflicts with collated flag */
L		([A-Z][A-Z]+[0-9][0-9][0-9]+[A-Z0-9a-z_'"/+^]*(′|″)*){M}*
B		("["|"]"|\xe2\xb8[\xa2-\xa5])
S		\xe2\xb8[^\xa2-\xa5]|([0-9][a-wyz]*)
G		([a-ru-zA-RU-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|[stST],?|{S}|\xc3[^\x97]|\xe2[^\xb8\x88])+
CG		([A-RU-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|[ST],?|{S}|\xc3[^\x97]|\xe2[^\xb8\x88])+
M		("@"([a-wyz]+[0-9]*|45|90|180|270)|"~"[a-z0-9][a-wyz0-9]*|"\\"[a-z]*[0-9]+)
NNUM		([N][0-9][0-9]?[ACF]?)
GRAPHEME 	({G}|"15")
FRACTION	[0-9]\/[0-9]
L_UHS		"\xe2\xb8\xa2"
R_UHS		"\xe2\xb8\xa3"
L_LHS		"\xe2\xb8\xa4"
R_LHS		"\xe2\xb8\xa5"
UFLAG	"\xe2\x80"[\xa0\xa1\xa2\xa3]
NOTEMARK	"^"[0-9]+"^"

%x s_AMPl
%x s_AMPr
%x s_SCO
%x s_PRO
%x s_ATFPRO
%x s_LINK
%x s_LZR
%x s_T
%x s_WORD
%x s_TRD
%x s_TRA
%x s_IGNORE
%x C
%s Q
%s S
%x TC
%x TD
%x A
%x N

%%

^({BOM3}|{BOM2})?"&"  		{ BEGIN s_AMPl; s_next = 0; }
<s_AMPl>{PQX}			{ atfsave; return r(PQX); }
<s_AMPl>[ \t]+=	       		{ BEGIN s_AMPr; return r('='); }
<s_AMPl>[ \t]+/[^=]    		{ mesg_verr(&aglloc, "no '=' in &-line");
				  BEGIN s_AMPr; return r('='); }
<s_AMPr>[^\n\r]+      		{ BEGIN 0; atfnorm; return r(TEXT); }

<s_T>^[\t ]*/[\n\r]	       	{ ; }
<s_T>^[^\t ][^\n\r]*           	{ BEGIN s_next; s_next=0; atfnorm; return r(TEXT); }
<s_T>.*/\r?\n[ \t]+[\r\n]    	{ BEGIN s_next; s_next=0; atfnorm; return TEXT; }
<s_T>.*/\r?\n[^ \t]+		{ BEGIN s_next; s_next=0; atfnorm; return TEXT; }
<s_T>.*/\r?\n	       		{ BEGIN s_next; s_next=0; atfnorm; return TEXT; }
<s_T>^[\t ]+.+/[\r\n]  		{ BEGIN s_next; s_next=0; atfnorm; return TAB; }
<s_T><<EOF>>			{ return TEXT; }

<s_WORD>[^ \t\n\r]+	     	{ BEGIN s_next;
				  int ret=(s_next==s_TRD?TR_PROJ:WORD); s_next=0; atfsave; return ret; }
<s_WORD>[ \t]+	       		{ ; }

^"@composite"			{ (void)ag_block(agtext); return COMPOSITE; }
^"@score"		       	{ BEGIN s_SCO; (void)ag_block(agtext); return SCORE; }
<s_SCO>"matrix"			{ return MATRIX; }
<s_SCO>"synoptic"	       	{ return SYNOPTIC; }
<s_SCO>"parsed"			{ return PARSED; }
<s_SCO>"unparsed"	       	{ return UNPARSED; }
<s_SCO>"word"			{ return SWORD; }
<s_SCO>[ \t]			{ ; }
<s_SCO>[\n\r]	       		{ BEGIN 0; }

^"#project:"			{ BEGIN s_PRO; return HASH_PROJECT; }
<s_PRO>{PROJECT}/[ \t]*[\n\r]   { BEGIN 0; atfsave; return PROJECT; }
<s_PRO>[ \t]+			{ ; }

^"#atf:"			{ BEGIN s_ATFPRO; }
<s_ATFPRO>lang.*/[\n\r]	       	{ atfsave; return ATF_LANG; }
<s_ATFPRO>{use}mylines		{ usesave; return ATF_MYLINES; }
<s_ATFPRO>{use}line-labels    	{ usesave; return ATF_LINELABELS; }
<s_ATFPRO>{use}alignment-groups { usesave; return ATF_AGROUPS; }
<s_ATFPRO>{use}math	       	{ usesave; return ATF_MATH; }
<s_ATFPRO>{use}ascii		{ usesave; return ATF_ASCII; }
<s_ATFPRO>{use}unicode		{ usesave; return ATF_UNICODE; }
<s_ATFPRO>{use}legacy		{ usesave; return ATF_LEGACY; }
<s_ATFPRO>{use}lexical		{ usesave; return ATF_LEXICAL; }
<s_ATFPRO>{use}cuneiform      	{ ; } /* no longer useful; cuneify is default */
<s_ATFPRO>{use}lemconv		{ fprintf(stderr, "lemconv is not implemented\n"); }/* ever used?*/
<s_ATFPRO>[ \t]			{ ; }
<s_ATFPRO>[\n\r]       		{ BEGIN 0; }

^"#link:"	       		{ linktext = 0; BEGIN s_LINK; return HASH_LINK; }
<s_LINK>"def"			{ s_next=s_LINK; BEGIN s_WORD; return LINK_DEF; }
<s_LINK>"parallel"		{ return LINK_PARALLEL; }
<s_LINK>"source"		{ return LINK_SOURCE; }
<s_LINK>{QID}			{ linktext = 2; atfsave; return QID; }
<s_LINK>=			{ if (linktext==2) BEGIN s_T; else if (linktext==1) BEGIN s_WORD;
				  linktext = 0; return '='; }
<s_LINK>[ \t]			{ ; }
<s_LINK>[\n\r]	       		{ BEGIN 0; }

^"#bib:"			{ BEGIN s_T; return in_preamble ? HASH_PROBIB : HASH_BIB; }
^"#key:"			{ BEGIN s_T; return HASH_KEY; }
^"#note:"       		{ BEGIN s_T; return HASH_NOTE; }
^"#version:"			{ BEGIN s_T; return HASH_VERSION; }

^"#etcsl:"/[ \t]+t     		{ BEGIN s_T; return in_preamble ? HASH_ETCSL_T : HASH_ETCSL_L; }
^"#etcsl:"/[ \t]+l     		{ BEGIN s_T; return HASH_ETCSL_L; }

^"#lemmatizer:"			{ BEGIN s_LZR; return HASH_LEMMATIZER; }

<s_LZR>"do"			{ ; }
<s_LZR>[ \t]+			{ ; }
<s_LZR>"sparse"			{ BEGIN s_T; return LZR_SPARSE; }
<s_LZR>"stop"			{ BEGIN s_WORD; return LZR_SPARSE; }
<s_LZR>[\n\r]	       		{ BEGIN 0; }

^@([a-z][a-z0-9]*) 		{ BEGIN s_T; return ag_block(agtext); }

^"$"  				{ BEGIN s_T; atfsave; return DOLLAR; }

^"#lem:"  			{ BEGIN s_T; atfsave; return LEM; }

^#tr(.[a-z-]+)?:		{ BEGIN s_T; longinit; return TR_INTER; }

^{LK_TOTO}  			{ BEGIN s_T; longinit; return LNK_TOTO; }

^{LK_FROM}  			{ BEGIN s_T; longinit; return LNK_FROM; }

^{LK_PLUS}  			{ BEGIN s_T; longinit; return LNK_PLUS; }

^{LK_VBAR}  			{ BEGIN s_T; longinit; return LNK_VBAR; }

^[^ #@$\t\n\r]+\./[ \t\n\r]+        	{ BEGIN ag_line_save(agtext); return MTS; }


^[^ #@$\t\n\r]+:/[ \t\n\r]+        	{ BEGIN ag_line_save(agtext); return EXX; }


^:[^:#@$]+:[ \t]+	       	{ BEGIN ag_line_save(agtext); return VAR; }

^== 				{ BEGIN s_T; atfsave; return BIL; }

^#   				{ BEGIN s_T; return in_preamble ? PRO_COMMENT : COMMENT; }

<s_TRD>("labeled"|"parallel"|"unitary")	{ atfsave; tra_text=0; return TR_TYPE; }
<s_TRD>{LANG}				{ atfsave; s_next=s_TRD; BEGIN s_WORD; return TR_LANG; }
<s_TRD>[ \t]+				{ ; }
<s_TRD>"\n"				{ BEGIN s_TRA; }
<s_TRA>^@h[123]				{ atfsave; tra_text=0; s_next=s_TRA; BEGIN s_T; longinit; return TR_HDR; }
<s_TRA>^@label[^\n\r]*/[\n\r]	       	{ atfsave; tra_text=1; return TR_LABEL; }
<s_TRA>^@unit[^\n\r]*/[\n\r]	       	{ atfsave; tra_text=0; return TR_UNIT; }
<s_TRA>^@span[^\n\r]*/[\n\r]	       	{ atfsave; tra_text=1; return TR_SPAN; }
<s_TRA>^"$"  				{ tra_text=0; s_next=s_TRA; BEGIN s_T; longinit; return TR_DOLLAR; }
<s_TRA>^"#"[^ \t\n\r]*/[ \t\n\r]   	{ tra_text=0; s_next=s_TRA; BEGIN s_T; longinit; return TR_COMMENT; }
<s_TRA>{TRA_LAB}			{ atfsave; tra_text=1; return TR_LABEL; }
<s_TRA>{TRA_CHR}       			{ atfsave; return TR_TEXT; }
<s_TRA>{TRA_CMD}	       		{ atfsave; return TR_TEXT; }
<s_TRA>{TRA_PAR}			{ if (tra_text) { aglval.text=""; return TR_PAR; } }
<s_TRA>{TRA_WHT}			{ if (tra_text) { atfsave; return TR_TEXT; } }
<s_TRA>{TRA_WRD}	       		{ if (tra_text) { atfsave; return TR_TEXT; } }


[=^][aeiuAEIU]		{ if (gdl_unicode) { mesg_vwarning(currgdlfile,aglineno,
						"non-Unicode accent codes %s",agtext); }}

{G}({G}|{B})*{G}	{ GIN=1; return ag_gdl_grapheme_type(); }
{G}({G}|{B})*/{M}	{ GIN=1; return ag_gdl_grapheme_type(); }
{GRAPHEME} 		{ GIN=1; return ag_gdl_grapheme_type(); }

"|"/[ \t]	       	{ /*"*/ aglval.text=(char*)gdl_lexnames[(int)*yytext]; return gdl_lexfld[(int)*yytext]; }
"|"			{ BEGIN cvq_push(C); GIN=0; CIN=1; return C_O; }
<C>"|"/[ \t]   		{ BEGIN cvq_pop(); CIN=0; return C_C; }
<C>"|"			{ BEGIN cvq_pop(); CIN=0; return C_C; }
<C>"\xe2\x88\x98"	{ GIN=0; return C_CIRCLE; }
<C>{NNUM}/{M} 		{ GIN=1; gsave; return NNUM; }
<C>{NNUM} 		{ GIN=1; gsave; return NNUM; }
<C>{CG}			{ GIN=1; return ag_gdl_grapheme_type(); }
<C>{CG}({CG}|{B})*{CG}	{ GIN=1; return ag_gdl_grapheme_type(); }
<C>{CG}({CG}|{B})*/{M}	{ GIN=1; return ag_gdl_grapheme_type(); }
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
<C>[a-wyz]    		{ GIN=0; mesg_vwarning(currgdlfile,aglineno,"lowercase graphemes not allowed in |...|"); }
<C>"("			{ return GIN ? QLP : CLP; }
<C>")"			{ return CRP; }
<C>[ \t]+	       	{ CIN=GIN=0; mesg_vwarning(currgdlfile,aglineno,"missing '|' in compound");
			  cvq_reset(); }

<Q>")"/[}>]	       	{ return QRP; }
<Q>")"			{ return QRP; }
<Q>[ \t]+	       	{ QIN=GIN=0; mesg_vwarning(currgdlfile,aglineno,"missing ')' in qv");
			  cvq_reset(); }

":"/[ \t\n\(\]]        	{ /*"*/ GIN=1; gsave; return PUNCT; }
":."			{ /*"*/ GIN=1; gsave; return PUNCT; }
":'"			{ /*"*/ GIN=1; gsave; return PUNCT; }
":\""			{ /*"*/ GIN=1; gsave; return PUNCT; }
"::"      		{ /*"*/ GIN=1; gsave; return PUNCT; }
"/"/[ \t\n\(\]]		{ /*"*/ GIN=1; gsave; return PUNCT; }

{L}			{ gsave; return LISTNUM; }

[0-9nN]+(\/[0-9]+)?/"("	{ GIN=1; gsave; return NUMBER; }

{NNUM}/{M}	       	{ GIN=1; gsave; return NNUM; }
{NNUM} 			{ GIN=1; gsave; return NNUM; }

[0-9]+(\/[0-9]+)?	{ GIN=1; gsave; return !strncmp(aglval.text,"00",2) ? ZERO : BARENUM; }

{M}			{ GIN=1; gsave; return MOD; }

	/* Determinatives and glosses */

"{{"[0-9+]:	       	{ gconst; GIN=0; return L_dbl_cur; }
"{{"			{ gconst; GIN=0; return L_dbl_cur; }
"}}"			{ gconst; GIN=0; return R_dbl_cur; }
"{("			{ gconst; GIN=0; return L_cur_par; }
")}"			{ gconst; GIN=0; return R_cur_par; }
"(x("			{ gconst; GIN=0; return L_par_par; }
")x)"			{ gconst; GIN=0; return R_par_par; }
"{"			{ GIN=0; return '{'; }
"{+"			{ GIN=0; return DET_PHON; }
"{-"			{ GIN=0; return DET_SEME; }
"}"			{ GIN=0; return '}'; }

,			{ return ','; }
&[0-9]+  		{ gsave; return CELLSPAN; }
&			{ return '&'; }
![a-z]+        		{ gsave; return FTYPE; }

	/* Normalized text scanning */

<N>{G}			{ gsave; return NORMSEG; }

	/* Alphabetic text scanning */

<A>.			{ mesg_vwarning(currgdlfile,aglineno,"alphabetic input not yet implemented"); }

	/* Delimiters */

<INITIAL,N>"..."       	{ GIN=0; gsave; return ELLIPSIS; }
<INITIAL,N>"--"	       	{ GIN=0; return ENHYPHEN; }
<INITIAL,N>-		{ GIN=0; return agtext[0]; }
[+:.]     		{ GIN=0; return agtext[0]; }

"/"/("|"|{G})	       	{ return '/'; } /* alternation */

"//"			{ return LINEBREAK; }

<INITIAL,N>[ \t]+/[#"~=@^]    	{ GIN=0; SIN=1; gsave; return SPACE; }
<INITIAL,N>[ \t]+	       	{ GIN=0; gsave; return SPACE; }

	/* Inline comment/dollar */

"($"			{ BEGIN TD; return L_inl_dol; }
<TD>"$)"	       	{ BEGIN 0; return R_inl_dol; }

"(#"			{ BEGIN TC; return L_inl_cmt; }
<TC>"#)"       		{ BEGIN 0; return R_inl_cmt; }

<TD>([^$]|\$[^)])*     	{ gsave; return TEXT; }
<TC>([^#]|\#[^)])*   	{ gsave; return TEXT; }

	/* Anything in this section either does not return a token or
 	   only does so in some circumstances */

<INITIAL,N,Q>"("	{ if (GIN) { GIN=0; return QLP; } else { gdl_state_o(e_L_par,YY_START); } }
<INITIAL,N,Q>")"	{ GIN=0; if (QIN) { return QRP; } else { gdl_state_c(e_R_par); } }

<INITIAL,C,N>"<("	{ GIN=0; gdl_state_o(e_L_ang_par,YY_START); }
<INITIAL,C,N>")>"	{ GIN=0; BEGIN gdl_state_c(e_R_ang_par); }
<INITIAL,C,N>"<<"	{ GIN=0; gdl_state_o(e_L_dbl_ang,YY_START); }
<INITIAL,C,N>">>"	{ GIN=0; BEGIN gdl_state_c(e_R_dbl_ang); }
<INITIAL,C,N>"<"	{ GIN=0; gdl_state_o(e_L_ang,YY_START); }
<INITIAL,C,N>">"	{ GIN=0; BEGIN gdl_state_c(e_R_ang); }
<INITIAL,C,N>"["	{ GIN=0; gdl_break_o(e_L_squ); }
<INITIAL,C,N>"]"	{ GIN=0; gdl_break_c(e_R_squ); }
<INITIAL,C,N>{L_UHS}	{ GIN=0; gdl_break_o(e_L_uhs); }
<INITIAL,C,N>{R_UHS}	{ GIN=0; gdl_break_c(e_R_uhs); }
<INITIAL,C,N>{L_LHS}	{ GIN=0; gdl_break_o(e_L_lhs); }
<INITIAL,C,N>{R_LHS}	{ GIN=0; gdl_break_c(e_R_lhs); }

	/* The meta-delimiter ';' == indented line */
<INITIAL,C>";"	       	{ GIN=0; gdl_indent(); }

	/* Sign state force characters */
~~/{G}			{ g_logoforce_flag = 1; }
"~"/{G}			{ if (gdl_lexical) { gsave; mesg_vwarning(currgdlfile, aglineno,
			   "no space after ~ field; use ~~ for logogram force with atf: use lexical\n");
			     aglval.text=(char*)gdl_lexnames[(int)*yytext]; return gdl_lexfld[(int)*yytext];
			   } else { g_logoforce_flag = 1; } }
"$"/{G}			{ g_literal_flag = 1; }

	/* Grapheme meta flags, sometimes doubling as lex field shorthands */
<INITIAL,C,Q,N>[!?]     { gdl_lex_flag((uccp)yytext); }
<INITIAL,C,N>"*"       	{ /*"*/ if (GIN) { gdl_lex_flag((uccp)yytext); } else { GIN=1; gsave; return(PUNCT); } }
<INITIAL,C,N>^[#"~=@^]	{ aglval.text=(char*)gdl_lexnames[(int)*yytext]; return gdl_lexfld[(int)*yytext]; }
"#"			{ if (SIN) { SIN=0;
				     aglval.text=(char*)gdl_lexnames[(int)*yytext];
			  	     return gdl_lexfld[(int)*yytext]; } else { gdl_lex_flag((uccp)yytext); } }
<C,N>"#"	       	{ gdl_lex_flag((uccp)yytext); }
<INITIAL,C,N>[:"~=@^]   { if (SIN) { SIN=0;
				     aglval.text=(char*)gdl_lexnames[(int)*yytext];
				     return gdl_lexfld[(int)*yytext]; } else { gdl_lex_flag((uccp)yytext); } }
<INITIAL,N>[#"=@^]/{G} 	{ if (gdl_lexical) {
		       	    mesg_vwarning(currgdlfile, aglineno,
			  	       	  "no space after %c field\n", *yytext);
	       	     	    aglval.text=(char*)gdl_lexnames[(int)*yytext];
			    return gdl_lexfld[(int)*yytext]; }
			  else { gdl_lex_flag((uccp)yytext); } }

{UFLAG}			{ gdl_lex_flag((uccp)yytext); }

	/* Language shifts; ignore following whitespace after %-tag */
<INITIAL,N>%[-a-z0-9]+("/"[gn])?[ \t]*   { BEGIN gdl_lang_tag(yytext); }
_			                 { gdl_lang_flip(); }

	/* Note marks */
{NOTEMARK}		{ gdl_note_mark(agtext); }

	/* Inline variation note marks */
"("[0-9]+::[0-9]+")"   	{ gsave; GIN=0; gdl_var_mark(e_LR_var, aglval.text); }
"("[0-9]+:		{ gsave; GIN=0; gdl_var_mark(e_L_var, aglval.text); }
:[0-9+]")"		{ gsave; GIN=0; gdl_var_mark(e_R_var, aglval.text); }

	/* General administrative rules */

<INITIAL,N>\n  		{ return '\n'; }

"\xc3\x97"	       	{ mesg_vwarning(currgdlfile, aglineno, "gdl.l: misplaced ×\n"); }

<*>.			{ if (last_unmatched_line != aglineno)
			    {
				mesg_vwarning(currgdlfile, aglineno,
			  		      "gdl.l: unmatched character %c in state %d\n",
					       *agtext, YY_START);
			       	last_unmatched_line = aglineno;
			    }
			}

<<EOF>>			{ gdl_line_wrapup(aglloc); yyterminate(); }


%%
static void
ag_user_action(void)
{
  if (agflextrace)
      fprintf(stderr, "%s:%d: aglex: text=::%s:: start=%d; tra_text=%d\n", curragfile, aglineno, agtext, YY_START, tra_text);

  aglloc.file = curragfile;
  aglloc.line = aglineno;
}

static void
ag_l_lex_init(const char *file, int line)
{
  curragfile = agfile = file;
  aglineno = line;
}

static int
ag_line_save(const char *t)
{
  extern const char *curr_line_tok;
  curr_line_tok = (ccp)hpool_copy((uccp)t, ag_hpool);
  return 0;
}

#if 0
static int
ag_gdl_init(void)
{
  gdl_new_word(gdl_get_tree());
  return 0;
}
#endif
static Blocktok *last_bp = NULL;
static Blocktok *last_fragp = NULL;
void
ag_bp_reset(void)
{
  last_fragp = last_bp = NULL;
}

static int
ag_block(const char *scanned_at)
{
  const char *atcmd = scanned_at + 1;

  Blocktok *bp = blocktok(atcmd, strlen(atcmd));
  if (bp)
    {
      if (bp->bison == Y_TRANSLATION)
	BEGIN s_TRD;
      if (bp->bison == Y_FRAGMENT)
	{
          if (atfp->edoc == EDOC_COMPOSITE || atfp->edoc == EDOC_SCORE)
	     bp = blocktok("cfragment", 9); /* c[omposite]fragment */
          else if (last_bp)
	    {
	      if (last_bp->type == B_OBJECT)
	        bp = blocktok("objfrag", 7);
	      else if (last_bp->type == B_SURFACE)
	        bp = blocktok("srffrag", 7);
	      else if (last_bp->type == B_COLUMN)
	        bp = blocktok("colfrag", 7);
	      else
	        bp = blocktok("srffrag", 7);
	    }
	  else
            bp = blocktok("srffrag", 7);

	  if (last_fragp)
	    {
	      if (last_fragp->type != bp->type)
		  mesg_verr(&aglloc, "text mixes @fragment at different levels (last=%d!=curr=%d: missing @column?)", last_fragp->type, bp->type);
	      else
		{
		    /*mesg_verr(&aglloc, "@fragment level == %d", bp->type);*/
 	 	  bp = last_fragp;
		}
	    }
	  else
	      /*mesg_verr(&aglloc, "@fragment level == %d", bp->type)*/;
	  last_fragp = bp;
        }
      else
	last_bp = bp;
      aglval.b = bp;
      return bp->bison;
    }
  else
    {
      last_bp = NULL;
      fprintf(stderr, "atf: bad at-command @%s\n", atcmd);
      return Y_BAD;
    }
}

Tree *
ag_read_atf(char *atf, size_t len)
{
  YY_BUFFER_STATE atf_buffer;
  if (!(atf_buffer = ag_scan_buffer(atf, len)))
    {
      fprintf(stderr, "ag_read_atf: ag_scan_buffer failed\n");
      return NULL;
    }
  yy_start = s_next = 0;
  atf_init();
  Tree *tp = atfyacc();
  ag_wrapup_buffer();
  return tp;
}

void
ag_wrapup_buffer(void)
{
  ag_delete_buffer(YY_CURRENT_BUFFER);
}

ssize_t
ag_get_lineno(void)
{
    return last_aglineno;
}

void
ag_set_lineno(ssize_t n)
{
    agset_lineno(n);
}

void
ag_set_file(const char *f)
{
    curratffile = f;
    last_aglineno = aglineno = 1;
    agset_lineno(1);
}

char *
ag_set_file_line(char *a, ssize_t *n)
{
  char *s = a;
  while (!isspace(*a)) /* we know this line is `##file ' */
    ++a;
  while (isspace(*a))
    ++a;
  curratffile = a;
  while (*a && '\n' != *a)
    ++a;
  if (*a)
    {
      *a++ = '\0';
      if (*a)
        {
          while (*a && !isspace(*a))
	    ++a;
          while (isspace(*a))
            ++a;
          aglineno = (int)strtoul(a, NULL, 10);
          while (*a && '\n' != *a)
	    ++a;
          ++a;
          *n = (*n - (a - s));
          return a;
       }
    }
  return NULL;
}
int
ag_gdl_lex_state(void)
{
  return YY_START;
}

void
ag_gdl_lex_closers(void)
{
  if (CIN)
      mesg_vwarning(currgdlfile,aglineno,"missing '|' in compound (discovered at end of line)");
  else if (QIN)
    mesg_vwarning(currgdlfile,aglineno,"missing ')' in qv (discovered at end of line)");
  GIN=CIN=QIN=0;
  cvq_reset();
}

/* Public function to call with (0) arg at start of text and with (1)
   arg when #atf: use legacy is encountered */
void
ag_gdl_legacy_lexer(int legacy)
{
  if (legacy)
    {
      ag_gdl_grapheme_type = ag_gdl_grapheme_type_l;
      gdl_delim = gdl_delim_l;
      gdl_graph_node = gdl_graph_node_l;
    }
  else
    {
      ag_gdl_grapheme_type = ag_gdl_grapheme_type_s;
      gdl_delim = gdl_delim_s;
      gdl_graph_node = gdl_graph_node_s;
    }
}	

/* Version of ag_gdl_grapheme_type for use when atf mode is strict, i.e., non-legacy */
int
ag_gdl_grapheme_type_s(void)
{
  char *g = agtext;
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

/* Version of ag_gdl_grapheme_type for use when atf: use legacy is set */
int
ag_gdl_grapheme_type_l(void)
{
  char *l = agtext;
  char *g = (char*)gdl_unlegacy_str(&aglloc, (uccp)l);
  if (g)
    {
  gsaveg(g);

  if (strcmp(g,l))
    gdl_legacy_prop = (uccp)l;
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
  else
    {
       return IGNORE;
    }
}

void
gdl_lex_init(const char *file, int line)
{
  CIN = GIN = QIN = 0;
  cvq_top = -1;
  ag_l_lex_init(file, line);
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
  if (agflextrace)
    fprintf(stderr, "gdl_decr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

void
gdl_incr_qin(void)
{
  ++QIN;
  BEGIN cvq_push(Q);
  if (agflextrace)
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
    fprintf(stderr, "%s:%d: cvq_push: cvq stack overflow (CVQ_MAX=%d)\n", currgdlfile, aglineno, CVQ_MAX);
  return 0;
}

int
cvq_pop(void)
{
  if (cvq_top >= 0)
    return cvq_stack[cvq_top--];
  else
    fprintf(stderr, "%s:%d: cvq_pop: cvq stack underflow\n", currgdlfile, aglineno);
   return 0;
}

void
cvq_reset(void)
{
  cvq_top = -1;
  BEGIN 0;
}
