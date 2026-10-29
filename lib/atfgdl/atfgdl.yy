%define api.prefix {ag}
%file-prefix"ag"
%locations
%define parse.error verbose

%{
#include <oraccsys.h>
#include <mesg.h>
#include <gdl.h>
#include <gdlstate.h>
#include <atf.h>
#include "ag.h"
extern const char *agtext, *curragfile;
extern int aglineno, agtrace;
extern int gdl_unicode, gdl_lexical;

static Tree *ytp;
static Node *ynp, *yrem, *ycp, *mnp;
extern Node *lgp;

#define AGLTYPE_IS_DECLARED 1
#define AGLTYPE Mloc
#define yylineno aglineno
AGLTYPE aglloc;

extern int aglex(void);
extern void yyerror(const char *);

extern Node *gdl_post_det_gp_attach;

#define alo(xxx) ag_line_o_##xxx
#define alc(xxx) ag_line_c_##xxx
%}

%union { char *text; int i; struct node *node; Blocktok *b; }

%token	<text>
		FTYPE LANG ENHYPHEN ELLIPSIS CELLSPAN IGNORE NORMSEG
		GRAPHEME NNUM NUMBER BARENUM ZERO LISTNUM PUNCT MOD LINEBREAK
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES CMOD C_CIRCLE
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		',' LF_AT LF_CARET LF_EQUALS LF_HASH LF_QUOTE LF_TILDE LF_VBAR
		PQX QID TEXT WORD DOC PROJECT LINK KEY
		MILESTONE OBJECT SURFACE COLUMN DIVISION GROUP
		TAB EOL
		VERSION
		ATF_LANG
		TR_TYPE TR_LANG TR_PROJ TR_HEAD TR_LABEL TR_UNIT TR_SPAN
		TR_TEXT TR_PAR TR_HDR TR_COMMENT TR_DOLLAR

%token	<i>
		'<' '>' '[' ']' '(' ')' CLP CRP QLP QRP
		'/' '+' ':'
		L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur L_par_par R_par_par
		L_dbl_par R_dbl_par L_dbl_par_c R_dbl_par_c eras_canc_pivot
		L_ang_par R_ang_par L_ang_par_s R_ang_par_s L_cur_par R_cur_par
		L_uhs R_uhs L_lhs R_lhs LANG_FLIP
		'{' DET_SEME DET_PHON '}'
		PLUS_FLAG SPACE END
		HASH_PROJECT HASH_LINK HASH_VERSION HASH_NOTE HASH_KEY
		HASH_ETCSL_T HASH_ETCSL_L
		HASH_LEMMATIZER LZR_SPARSE LZR_STOP HASH_BIB HASH_PROBIB
		COMPOSITE SCORE MATRIX SYNOPTIC PARSED UNPARSED SWORD
		ATF_MYLINES ATF_LINELABELS ATF_AGROUPS ATF_MATH
		ATF_ASCII ATF_UNICODE
		ATF_LEGACY ATF_LEXICAL
		LINK_DEF LINK_PARALLEL LINK_SOURCE
		PRO_COMMENT COMMENT
		MTS NTS LGS GUS LEM BIL EXX VAR DOLLAR TR_INTER
		LNK_TOTO LNK_FROM LNK_PLUS LNK_VBAR
		Y_BAD
		Y_BODY Y_BOTTOM Y_BULLA Y_CATCHLINE Y_CFRAGMENT Y_COLOPHON Y_COLUMN
		Y_COMPOSITE Y_DATE Y_DIV Y_DOCKET Y_EDGE Y_END Y_ENDVARIANTS
		Y_ENVELOPE Y_FACE Y_FRAGMENT Y_H1 Y_H2 Y_H3 
		Y_INCLUDE Y_LABEL Y_LEFT Y_LINECOUNT Y_M Y_NOTE Y_OBJECT Y_OBVERSE
		Y_PRISM Y_REFERTO Y_REVERSE Y_RIGHT Y_SCORE Y_SEAL Y_SEALINGS Y_SIDE
		Y_SIGNATURE Y_SPAN Y_SUMMARY Y_SURFACE Y_TABLET Y_TOP Y_TRANSLATION
		Y_TRANSLITERATION Y_TRANSTYPE Y_UNIT Y_VARIANT Y_VARIANTS Y_WITNESSES
		Y_OBJFRAG Y_SRFFRAG Y_COLFRAG ATF_END

%token	<node>
		NOTEMARK

%nterm	<text>		pqx name longtext
		field lexfld cmods

%nterm	<node>		detgloss det gloss
		grapheme scgrapheme compound simplexg valuqual
		glosso glossc

%nterm	<i>		sparse stype atfuse link_type
		atf.line l_link note bib etcsl.line proto.comment
		heading heading_tok tr.inter
		milestone atf.comment dollar

%nterm	<b>		object object_tok
		surface surface_tok
		column column_tok
		milestone_tok
		division
		bdivision edivision vdivision
		div_tok end_tok var_tok

%start atfgdl

%%

atfgdl: atfs | gdl.line

atfs:		atf
	|	atfs atf
	;

atf:		atf.transliteration { atf_tlit_wrapup(); }
	|	composite 	{ atf_tlit_wrapup(); }
	|	score 		{ atf_tlit_wrapup(); }
	|	translation	{ atf_tlat_wrapup(); }
	;

atf.transliteration:
		amp preamble { atf_wrapup(WH_PREAMBLE); }
	| 	amp preamble { atf_wrapup(WH_PREAMBLE); } tblocks
		;

composite:	amp atcomposite preamble { atf_wrapup(WH_PREAMBLE); }
	| 	amp atcomposite preamble { atf_wrapup(WH_PREAMBLE); } cblocks
		;

score:		amp atscore cblocks
		;

amp: 		pqx '=' name { atf_bld_amp(@1, $1, (uccp)$3); }
		;

pqx: 		PQX
		;

name: 		TEXT
		;

preamble:
		plks
	|	ATF_END
	;

atcomposite:	COMPOSITE			{ atfp->edoc = EDOC_COMPOSITE;
		    				  atf_bld_doc(@1); }
	;

atscore:
		SCORE stype sparse		{
		    atfp->edoc = EDOC_SCORE;
		    atfp->stype = $2; atfp->sparse = $3;
		    atf_bld_doc(@1);
		}
	| 	SCORE stype sparse SWORD	{
		    atfp->edoc = EDOC_SCORE;
		    atfp->stype = $2; atfp->sparse = $3;
		    atfp->sword = EDOC_WORD;
		    atf_bld_doc(@1);
		}
	;

stype:
		/* I don't like "SYNOPSIS" for "SYNOPTIC" but its in the
		 * original XTF schema so it can't change now
		 */
	MATRIX { $$=EDOC_MATRIX; } | SYNOPTIC { $$=EDOC_SYNOPSIS; };

sparse: PARSED { $$=EDOC_PARSED; } | UNPARSED { $$=EDOC_UNPARSED; };

plks: 		plk
	| 	plks plk
		;

plk:      	protocol.start
	| 	link
	| 	key
		;

protocol.start: project | atfpro | proto.comment | lemmatizer | version | probib | etcsl.text

lemmatizer:	HASH_LEMMATIZER LZR_SPARSE TEXT
		{ atf_bld_protocol(@1, PROT_LZR_SPARSE, $3); }
	|	HASH_LEMMATIZER LZR_STOP WORD
		{ atf_bld_protocol(@1, PROT_LZR_STOP, $3); }
	;

version:	HASH_VERSION TEXT    { atf_bld_protocol(@1, PROT_VERSION, $2); }
	;

etcsl.text:	HASH_ETCSL_T TEXT      	{ atf_bld_protocol(@1, PROT_ETCSL, $2); }
	;

etcsl.line:	HASH_ETCSL_L TEXT      	{ atf_bld_protocol(@1, PROT_ETCSL, $2); }
	;

probib: 	HASH_PROBIB longtext 	{ atf_bld_bib(@1, longtext_get()); }
		;

bib:		HASH_BIB longtext    	{ atf_bld_bib(@1, longtext_get()); }
	;

proto.comment:
		PRO_COMMENT longtext	{ $$=$1; atf_bld_protocol(@1, PROT_COMMENT, longtext_get()); }
		;

atf.comment:
		COMMENT longtext	{ atf_comment(@1, longtext_get()); }
		;

dollar:
		DOLLAR longtext		{ atf_dollar(@1, longtext_get()); }
		;

note:		HASH_NOTE longtext   	{ line_note(@1, longtext_get()); }
		;

project:	HASH_PROJECT PROJECT 	{ atfp->project = (uccp)$2;
					  atf_xprop(ytp->root->kids,"project",$2);
				          atf_bld_protocol(@1, PROT_PROJECT, $2);
		}
		;

atfpro:	       	ATF_LANG 	{ atf_lang(@1, atfp, $1); };
	|	atfuse		{ atfp->flags |= $1; atf_bld_atf_protocol(@1, curr_use_str); }
		;

atfuse:		ATF_MYLINES    { $$=ATFF_MYLINES; }
	|	ATF_LINELABELS { $$=ATFF_LINELABELS; }
	|	ATF_AGROUPS    { $$=ATFF_AGROUPS; }
	|	ATF_MATH       { $$=ATFF_MATH; }
	|	ATF_ASCII      { gdl_ascii = 1; $$=ATFF_ASCII; }
	|	ATF_UNICODE    { $$=ATFF_UNICODE; }
	|	ATF_LEGACY     { $$=ATFF_LEGACY; gdl_legacy = 1; ag_gdl_legacy_lexer(1); }
	|	ATF_LEXICAL    { $$=ATFF_LEXICAL; gdl_lexical = 1; }
		;

link:
		HASH_LINK link_body
		;

link_body:
		LINK_DEF WORD '=' QID '=' TEXT 	{ atf_bld_link(@1, ELINK_DEF,(uccp)$2, $4, (uccp)$6); }
	|	link_type QID '=' TEXT	        { atf_bld_link(@1, $1, NULL, $2, (uccp)$4); }
		;

link_type:	LINK_PARALLEL {$$=ELINK_PARALLEL;} | LINK_SOURCE {$$=ELINK_SOURCE;}
		;

key: 		HASH_KEY TEXT 	     { atf_bld_key(@1, $2); }
	;

tblocks:	tblock
	|	tblocks tblock
	;

tblock: 	object
	|	surface
	|	column
	|	milestone
	|	heading
	|	atf.line
	|	ATF_END
		;

cblocks:	cblock
	|	cblocks cblock
		;

cblock: 	division
	|	vdivision
	|	milestone
	|	heading
	|	atf.line
	|	error atf.line { mesg_print(stderr); mesg_init();
		    /* fprintf(stderr, "skipping to next line\n"); */
		    yyclearin; yyerrok; }
	;

division:	bdivision cblocks edivision
		;

bdivision:	div_tok    TEXT { atf_bld_block(@1, $1, $2); }
	;

edivision:	end_tok    TEXT { atf_bld_block(@1, $1, $2); }
	;

vdivision:	var_tok    TEXT { atf_bld_block(@1, $1, $2); }
	;

milestone:	milestone_tok 	TEXT { atf_bld_block(@1, $1, $2); }
	;

object: 	object_tok TEXT { atf_bld_block(@1, $1, $2); }
	;

surface:	surface_tok TEXT { atf_bld_block(@1, $1, $2); }
		;

column:		column_tok TEXT { atf_bld_block(@1, $1, $2); }
	;

heading:	heading_tok longtext { atf_bld_heading(@1, $1, longtext_get()); }
	;

atf.line:
		MTS {alo(mts)(@1,agtext);} gdl.line { $$=$1; alc(mts)(@1); }
	| 	NTS {alo(nts)(@1,agtext);} gdl.line { $$=$1; alc(nts)(@1); } /* MTS prereq; singleton */
	| 	LGS {alo(lgs)(@1,agtext);} gdl.line { $$=$1; alc(lgs)(@1); } /* MTS prereq; singleton */
	| 	GUS {alo(gus)(@1,agtext);} gdl.line { $$=$1; alc(gus)(@1); } /* MTS prereq; singleton */
	| 	BIL {alo(bil)(@1,agtext);} gdl.line { $$=$1; alc(bil)(@1); } /* MTS prereq */
	| 	EXX {alo(var)(@1,agtext);} gdl.line { $$=$1; alc(var)(@1); } /* MTS prereq */
	| 	VAR {alo(var)(@1,agtext);} gdl.line { $$=$1; alc(var)(@1); } /* MTS prereq */
	| 	LEM longtext		{ $$=$1; line_lem(@1, (ucp)longtext_get()); } /* MTS|NTS|BIL prereq */
		/* l_link, etcsl.line, and tr.inter all require a line group */
	|	l_link longtext		{ $$=$1; atf_bld_protocol(@1, PROT_LINK, longtext_get()); }
	| 	etcsl.line
	|	tr.inter
		/* bib, comment, and note are attached to line group
		   if there is one or at the current block level */
	| 	bib
	|	atf.comment
	| 	note
		/* dollar is attached to line group unless it parses with a
		   column/surface/object scope in which case it ends any
		   current group and is attached at the appropriate level */
	|	dollar
	;

l_link:		LNK_TOTO
	|	LNK_FROM
	|	LNK_PLUS
	|	LNK_VBAR
	;

div_tok:	Y_DIV 		{ $$=yylval.b; }
	| 	Y_VARIANTS 	{ $$=yylval.b; }
	;

end_tok: 	Y_END 		{ $$=yylval.b; }
	| 	Y_ENDVARIANTS 	{ $$=yylval.b; }
	;

var_tok:	Y_VARIANT 	{ $$=yylval.b; }
	;

milestone_tok:
		Y_BODY		{ $$=yylval.b; }
	| 	Y_CATCHLINE	{ $$=yylval.b; }
	| 	Y_CFRAGMENT	{ $$=yylval.b; } /* @fragment in @composite or @score */
	| 	Y_COLOPHON      { $$=yylval.b; }
	| 	Y_DATE	        { $$=yylval.b; }
/* 	| 	Y_FRAGMENT      { $$=yylval.b; } */ /* never returned by lexer */
	| 	Y_INCLUDE       { $$=yylval.b; }
	| 	Y_LINECOUNT	{ $$=yylval.b; }
	| 	Y_M	        { $$=yylval.b; }
	| 	Y_REFERTO       { $$=yylval.b; }
	| 	Y_SEALINGS	{ $$=yylval.b; }
	| 	Y_SIGNATURE	{ $$=yylval.b; }
	| 	Y_SUMMARY	{ $$=yylval.b; }
	| 	Y_WITNESSES	{ $$=yylval.b; }
	;

object_tok:
		Y_BULLA  	{ $$=yylval.b; }
	| 	Y_ENVELOPE  	{ $$=yylval.b; }
	| 	Y_OBJECT  	{ $$=yylval.b; }
	| 	Y_PRISM  	{ $$=yylval.b; }
	| 	Y_TABLET  	{ $$=yylval.b; }
	|	Y_OBJFRAG 	{ $$=yylval.b; }
	;

surface_tok:
		Y_BOTTOM       	{ $$=yylval.b; }
	| 	Y_DOCKET       	{ $$=yylval.b; }
	| 	Y_EDGE	        { $$=yylval.b; }
	| 	Y_FACE	        { $$=yylval.b; }
	| 	Y_LEFT	        { $$=yylval.b; }
	| 	Y_OBVERSE       { $$=yylval.b; }
	| 	Y_REVERSE      	{ $$=yylval.b; }
	| 	Y_RIGHT	        { $$=yylval.b; }
	| 	Y_SEAL	        { $$=yylval.b; }
	| 	Y_SIDE	     	{ $$=yylval.b; }
	| 	Y_SURFACE       { $$=yylval.b; }
	| 	Y_TOP	     	{ $$=yylval.b; }
	|	Y_SRFFRAG 	{ $$=yylval.b; }
	;

column_tok:
		Y_COLUMN     	{ $$=yylval.b; }
	|	Y_COLFRAG 	{ $$=yylval.b; }
	;

heading_tok:		
		Y_H1
	|	Y_H2
	|	Y_H3
	;

translation:
		tr.header tr.chunks
		;

tr.header:	Y_TRANSLATION TR_TYPE TR_LANG TR_PROJ { atf_bld_tlat(@1,$2,$3,$4); }
		;

tr.chunks:
		tr.chunk
	|	tr.chunks tr.chunk
		;

tr.chunk:
		tr.align tr.trans
	|	tr.meta
		;

tr.meta:
		tr.hdr
	|	tr.comment
	|	tr.dollar
		;

tr.hdr: 	TR_HDR longtext       	{ atr_hdr(@1, $1+1, (ucp)longtext_get()); }
		;

tr.comment:	TR_COMMENT longtext    	{ atr_comment(@1, (ucp)longtext_get()); }
		;

tr.dollar:	TR_DOLLAR longtext     	{ atr_dollar(@1, (ucp)longtext_get()); }
		;

tr.align:
		tr.label
	|	tr.unit tr.span
		;

tr.label:	TR_LABEL		{ atr_label(@1, (ucp)$1); }
		;

tr.unit:	TR_UNIT 		{ atr_unit(@1, (ucp)$1); }
		;

tr.span:	TR_SPAN			{ atr_span(@1, (ucp)$1); }
		;

tr.trans:
		tr.tran
	|	tr.trans tr.tran
		;

tr.tran:
		TR_TEXT			{ atr_text(@1, $1); }
	|	TR_PAR			{ atr_para(); /*atr_pop();*/ }
		;

tr.inter:
		TR_INTER longtext      	{ $$=$1; atr_inter(@1, (ucp)longtext_get()); }
		;

longtext:
		TEXT			{ $$ = longtext(atfmp->pool, $1, NULL); }
        |      	TAB			{ $$ = longtext(atfmp->pool, $1, NULL); }
        | 	longtext TAB		{ $$ = longtext(atfmp->pool, $1, $2); }
		;

	/* GDL.Y */

%%
void
ag_init(void)
{

}

void
ag_set_tree(Tree *tp)
{
  ytp = tp;
  atf_bld_tree(tp);
}

void
agerror(const char *e)
{
  extern Stck *div_stck;
  Node *np = NULL;

  if (strstr(e, "unexpected end of file")
      && div_stck && ((intptr_t)(np = (Node*)stck_peek(div_stck)) != -1))
    mesg_verr(np->mloc, "atf: unclosed @division\n");
  else
    mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}
