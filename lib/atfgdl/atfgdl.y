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

%token	<i>
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
		'<' '>' '[' ']' '(' ')' CLP CRP QLP QRP
		'/' '+' ':'
		L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur L_par_par R_par_par
		L_dbl_par R_dbl_par L_dbl_par_c R_dbl_par_c eras_canc_pivot
		L_ang_par R_ang_par L_ang_par_s R_ang_par_s L_cur_par R_cur_par
		L_uhs R_uhs L_lhs R_lhs LANG_FLIP
		'{' DET_SEME DET_PHON '}'
		PLUS_FLAG SPACE END

%token	<node>
		NOTEMARK

%token	<text>
		PQX QID TEXT WORD DOC PROJECT LINK KEY
		MILESTONE OBJECT SURFACE COLUMN DIVISION GROUP
		TAB EOL
		VERSION
		ATF_LANG
		TR_TYPE TR_LANG TR_PROJ TR_HEAD TR_LABEL TR_UNIT TR_SPAN
		TR_TEXT TR_PAR TR_HDR TR_COMMENT TR_DOLLAR
		FTYPE LANG ENHYPHEN ELLIPSIS CELLSPAN IGNORE NORMSEG
		GRAPHEME NNUM NUMBER BARENUM ZERO LISTNUM PUNCT MOD LINEBREAK
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES CMOD C_CIRCLE
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		',' LF_AT LF_CARET LF_EQUALS LF_HASH LF_QUOTE LF_TILDE LF_VBAR

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

gdl.line: lineparts
	;

lineparts:
		linepart
	|	lineparts linepart
		;

linepart:	cell
	| 	field
	|	segment
		;

cell:
		cellspec
		;

cellspec:
		'&'			       	    	{ gdl_cell(ytp,"1"); }
	| 	CELLSPAN			      	{ gdl_cell(ytp,$1); }
		;

field:
		',' 		       	    		{ ynp = gdl_field(ytp,"default"); }
	| 	FTYPE		       		    	{ ynp = gdl_field(ytp,$1); }
        | 	lexfld				    	{ ynp = gdl_field(ytp,$1); }
		;

lexfld:
		LF_AT		{ $$ = $1; }
	| 	LF_CARET	{ $$ = $1; }
	| 	LF_EQUALS	{ $$ = $1; }
	| 	LF_HASH		{ $$ = $1; }
	| 	LF_QUOTE	{ $$ = $1; }
	| 	LF_TILDE	{ $$ = $1; }
	| 	LF_VBAR		{ $$ = $1; }
		;

segment:
		comment
	| 	space
	| 	linebreak
	| 	transliteration
		;

comment:
		L_inl_dol R_inl_dol	        { gdl_nongraph(&@1, ytp, $2, "dollar"); }
	| 	L_inl_cmt R_inl_cmt	        { gdl_nongraph(&@1, ytp, $2, "comment"); }
		;

space:
	  SPACE						{ gdl_hc(0); ynp = gdl_new_word(ytp);
	    						  /*gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO,
	      						    "literal", aglval.text); */ }
	| '\n'						{ gdl_line_wrapup(@1); }
	| END						{ gdl_balance_flush(@1); }
	;

linebreak:	LINEBREAK	      			{ gdl_nongraph(&@1, ytp, "//", "linebreak"); }
	;

transliteration:
		delim
	| 	grapheme
	| 	normseg
	;

delim:
		'.' 				       	{ ynp = gdl_delim(ytp, "."); }
        | 	'-' 			       		{ ynp = gdl_delim(ytp, "-"); }
	| 	ENHYPHEN 	       	       		{ ynp = gdl_delim(ytp, "--"); }
	|	'/'					{ gdl_group(@1, '/'); }
	|	':'					{ gdl_group(@1, ':'); }
	|	'+'					{ gdl_group(@1, '+'); }
	;

normseg:	NORMSEG					{ ynp = gdl_normseg(&@1, ytp, aglval.text); }
	;

detgloss:
		det
	|	gloss
	;

det:	  '{'	      					{ gdl_balance_state(@1,'{',gdl_lex_state());
	      						  $$ = ynp = gdl_push_l(&@1,ytp,"g:d");
	  						  ps_on(gs_det_o);
	  						  rs_on(gs_det|gs_g_semd_i); }
	| DET_SEME    					{ gdl_balance_state(@1,'{',gdl_lex_state());
	    						  $$ = ynp = gdl_push_l(&@1,ytp,"g:d");
	  						  ps_on(gs_det_o);
	  						  rs_on(gs_det|gs_g_semd_e); }
	| DET_PHON      	      			{ gdl_balance_state(@1,'{',gdl_lex_state());
	    						  $$ = ynp = gdl_push_l(&@1,ytp,"g:d"); 
	  						  ps_on(gs_det_o);
	  						  rs_on(gs_det|gs_g_phond); }
	| '}' 	 		  			{ if (-1 != gdl_balance_state(@1,'}',gdl_lex_state()))
		  					    gdl_det_props(ytp->curr);
		    					  if (gdl_post_det_gp_attach)
							    {
								tree_curr(gdl_post_det_gp_attach);
								gdl_post_det_gp_attach = NULL;
							    }
							  $$ = lgp = ytp->curr;
	      						  ynp = gdl_pop(ytp,"g:d");
	     						  /* set pst->det = SB_CL; lgp is last
							     node with g content or equivalent, i.e.,
						             where the closer state must be set */
	    						  /*gdl_update_state(lgp, gs_det_c);*/
	      						  bit_set(*lst, gs_det_c);
						          rs_no(gs_det|gs_g_semd_e|gs_g_semd_i|gs_g_phond);
	  				       	        }
	;

gloss:		glosso
	|	glossc
	;

graphemes:	grapheme
	|	graphemes grapheme
	;

grapheme:
		scgrapheme
	| 	valuqual
	|	detgloss
		;

scgrapheme:
		simplexg
	| 	compound
		;

maybegflags:
		mods
	| 	/*empty */
		;

simplexg:	s maybegflags	       	       	 { /*if (gdl_legacy) gdl_unlegacy(ynp);*/
		    		                   if (ynp->kids) gdl_mod_wrap(ynp, 1);
						   gvl_simplexg(ynp);
						   $$ = ynp; }
		;

s:
		GRAPHEME			{ ynp = gdl_graph(&@1, ytp, aglval.text); }
	| 	LISTNUM				{ ynp = gdl_listnum(&@1, ytp, aglval.text); }
	| 	NNUM				{ ynp = gdl_nnum(&@1, ytp, aglval.text); }
	| 	NUMBER				{ ynp = gdl_number(&@1, ytp, aglval.text); }
	| 	BARENUM				{ ynp = gdl_barenum(&@1, ytp, aglval.text); }
	| 	ZERO				{ ynp = gdl_graph(&@1, ytp, aglval.text); } /* 00 or 00~a etc */
	| 	PUNCT				{ ynp = gdl_punct(&@1, ytp, aglval.text); }
	| 	ELLIPSIS			{ ynp = gdl_nongraph(&@1, ytp, aglval.text, "ellipsis"); }
	|	IGNORE				{ ; }
	;

compound:
	  c cmaybemodflags         			{ ycp->mloc = ynp->mloc = mloc_mloc(&@1);
	    						  gdl_modq_flush();
							  gvl_compound(ycp);
							  $$ = ycp;
	    						  ynp = gdl_pop(ytp,"g:c");
							  ynp = gdl_c_term(ytp); }
	;

c:
	C_O 						{ ycp = gdl_push_l(&@1,ytp,"g:c");
	    						  gdl_c_init(ycp); }
	cbits
	C_C 						{ lgp = ynp = ycp; lst = prop_state(ycp,0L); c_processing = 0; }
        ;

cbits:
	  cbit
        | cbits cbit
	;

cbit:
	s	       			       		{ /*ynp->mloc = mloc_mloc(&@1);*/
	  						  /*if (gdl_legacy) gdl_unlegacy(ynp);*/
							  gvl_simplexg(ynp); }
	| cdelim					{ }
	| CLP			       			{ gdl_balance_state(@1,CLP,gdl_lex_state());
	    					  	  gdl_push_l(&@1,ytp,"g:gp"); }
	| CRP	     			    		{ if (-1 != gdl_balance_state(@1,CRP,gdl_lex_state()))
	      					    	   gdl_pop(ytp,"g:gp"); }
	| QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push_l(&@1,ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_remove_q_error(@1, yrem);
  							  gdl_incr_qin();
							}
	| QRP				      		{ gdl_decr_qin();
	  						  ynp->mloc = mloc_mloc(&@1);
	  						  gvl_valuqual(ytp->curr);
							  lgp = ytp->curr;
							  ynp = gdl_pop(ytp,"g:q");
							}
	| mod						{ gdl_modq_add(mnp); mnp = NULL; };
	;

cdelim:
		C_PERIOD	       			{ ynp = gdl_delim(ytp, "."); }
	| 	C_ABOVE					{ ynp = gdl_delim(ytp, "&"); }
	| 	C_COLON					{ ynp = gdl_delim(ytp, ":"); }
	| 	C_CROSSING     				{ ynp = gdl_delim(ytp, "%"); }
	| 	C_OPPOSING     				{ ynp = gdl_delim(ytp, "@"); }
	| 	C_PLUS					{ ynp = gdl_delim(ytp, "+"); }
	| 	C_TIMES					{ ynp = gdl_delim(ytp, "×"); }
	| 	C_3TIMES			       	{ gdl_prefix(ytp, (uccp)"3"); }
	| 	C_4TIMES		       		{ gdl_prefix(ytp, (uccp)"4"); }
	| 	C_CIRCLE	       			{ ynp = gdl_delim(ytp, "∘"); } /* for PC */
	;

cmaybemodflags:
		cmods
	| /* empty */
	;

cmods:
	  MOD						{ mnp = gdl_mod_cmod(&@1, ynp, NULL, $1); }
	| cmods MOD					{ (void)gdl_mod_cmod(&@1, ynp, mnp, $1); }
	;

valuqual:
	q	    				       	 { ynp->mloc = mloc_mloc(&@1);
	  						   gvl_valuqual(ytp->curr);
	  						   lgp = ynp = ytp->curr; }
	qmaybemodflags					 { gdl_mod_wrap_q(ynp);
	    						   $$ = lgp;
							   ynp = gdl_pop(ytp,"g:q"); }
        ;

q:
	grapheme
	QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push_l(&@1,ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_incr_qin(); }
	graphemes 	 	       			{ gdl_remove_q_error(@1, yrem); }		
	QRP 						{ gdl_decr_qin(); }
	;

qmaybemodflags:
		mods
	| /* empty */
	;

mods:
	  mod
	| mods mod
        ;

mod:
	  MOD						{ ynp->mloc = mloc_mloc(&@1);
	  						  mnp = gdl_mod(ytp, aglval.text); }
	;

glosso:
		L_cur_par	{ ynp = gdl_gloss_o(&@1, ytp, aglval.text, e_L_cur_par); }
	| 	L_dbl_cur	{ ynp = gdl_gloss_o(&@1, ytp, aglval.text, e_L_dbl_cur); }
	| 	L_ang_par_s	{ ynp = gdl_gloss_o(&@1, ytp, aglval.text, e_L_ang_par); }
	;

glossc:
		R_cur_par      	{ ynp = gdl_gloss_c(&@1, ytp, aglval.text, e_R_cur_par); }
	| 	R_dbl_cur      	{ ynp = gdl_gloss_c(&@1, ytp, aglval.text, e_R_dbl_cur); }
	| 	R_ang_par_s    	{ ynp = gdl_gloss_c(&@1, ytp, aglval.text, e_R_ang_par); }
	;

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
