%define api.prefix {atf}
%file-prefix"atf"
%locations
%define parse.error verbose

%{
#include <oraccsys.h>
#include "atf.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *atftext, *curratffile;
extern int atflineno, atftrace;
extern int gdl_unicode;

static Tree *ytp;

Block *curr_block;
Blocktok *curr_bt;
Group *curr_group;
const char *curr_use_str;

#define ATFLTYPE_IS_DECLARED 1
#define ATFLTYPE Mloc
#define yylineno atflineno
ATFLTYPE atflloc;

%}

%union { char *text; int i; }

%token	<text>		PQX QID TEXT WORD DOC PROJECT ATFPRO LEMMATIZER LINK KEY
			MILESTONE OBJECT SURFACE COLUMN DIVISION GROUP
			TAB EOL
			VAR VERSION
			ATF_LANG

%token  <i>		HASH_PROJECT HASH_LINK HASH_VERSION HASH_NOTE HASH_KEY
			HASH_LEMMATIZER LZR_SPARSE LZR_STOP HASH_BIB HASH_PROBIB
			COMPOSITE SCORE MATRIX SYNOPTIC PARSED UNPARSED SWORD
			ATF_MYLINES ATF_AGROUPS ATF_MATH ATF_UNICODE ATF_LEGACY ATF_LEXICAL
			LINK_DEF LINK_PARALLEL LINK_SOURCE
			LNK_TOTO LNK_FROM LNK_PLUS LNK_VBAR
			COMMENT
			MTS NTS LGS GUS LEM BIL EXX DOLLAR
			Y_BAD
			Y_BODY Y_BOTTOM Y_BULLA Y_CATCHLINE Y_CFRAGMENT Y_COLOPHON Y_COLUMN
			Y_COMPOSITE Y_DATE Y_DIV Y_DOCKET Y_EDGE Y_END Y_ENDVARIANTS
			Y_ENVELOPE Y_FACE Y_FRAGMENT Y_H1 Y_H1X Y_H2 Y_H2X Y_H3 Y_H3X
			Y_INCLUDE Y_LABEL Y_LEFT Y_LINECOUNT Y_M Y_NOTE Y_OBJECT Y_OBVERSE
			Y_PRISM Y_REFERTO Y_REVERSE Y_RIGHT Y_SCORE Y_SEAL Y_SEALINGS Y_SIDE
			Y_SIGNATURE Y_SPAN Y_SUMMARY Y_SURFACE Y_TABLET Y_TOP Y_TRANSLATION
			Y_TRANSLITERATION Y_TRANSTYPE Y_UNIT Y_VARIANT Y_VARIANTS Y_WITNESSES

%nterm <text>   	pqx name longtext

%nterm <i> 		doc sparse stype atfuse link_type
			division milestone
			lines line l_link note bib
			division_tok milestone_tok
			object object_tok surface surface_tok column column_tok
			heading

%start atf

%%

atf: 		amp
	|	amp blocks
	| 	amp preamble { atf_wrapup(WH_PREAMBLE); }
	| 	amp preamble { atf_wrapup(WH_PREAMBLE); } blocks /* xcl */
		;

longtext:
		TEXT		{ $$ = longtext(atfmp->pool, $1, NULL); }
        | 	longtext TAB	{ $$ = longtext(atfmp->pool, $1, $2); }
		;

amp: 		pqx '=' name { atf_bld_amp(@1, $1, (uccp)$3); }
		;

pqx: 		PQX
		;

name: 		TEXT
		;

/* This can be empty because of atf definition above */
preamble:
		plks
	| 	doc plks
	;

doc:		COMPOSITE			{
		    atfp->edoc = EDOC_COMPOSITE;
		    atf_bld_doc(@1);
		}
	| 	SCORE stype sparse		{
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

protocol.start: project | atfpro | lemmatizer | version | probib

lemmatizer:	HASH_LEMMATIZER LZR_SPARSE TEXT
		{ atf_bld_protocol(@1, PROT_LZR_SPARSE, $3); }
	|	HASH_LEMMATIZER LZR_STOP WORD
		{ atf_bld_protocol(@1, PROT_LZR_STOP, $3); }
	;

version:	HASH_VERSION TEXT    { atf_bld_protocol(@1, PROT_VERSION, $2); }
	;

probib: 	HASH_PROBIB longtext { atf_bld_bib(@1, (ccp)longtext(NULL,NULL,NULL)); }
		;

bib:		HASH_BIB longtext    { atf_bld_bib(@1, (ccp)longtext(NULL,NULL,NULL)); }
	;


note:		HASH_NOTE longtext   { atf_bld_note(@1, (ccp)longtext(NULL,NULL,NULL)); }
	;

project:	HASH_PROJECT PROJECT { atfp->project = (uccp)$2;
		    		       atf_xprop(ytp->root->kids,"project",$2);
				       atf_bld_protocol(@1, PROT_PROJECT, $2);
		}
		;

atfpro:	       	ATF_LANG 	{ atf_lang(@1, atfp, $1); };
	|	atfuse		{ atfp->flags |= $1; atf_bld_atf_protocol(@1, curr_use_str); }
		;

atfuse:		ATF_MYLINES { $$=ATFF_MYLINES; }
	|	ATF_AGROUPS { $$=ATFF_AGROUPS; }
	|	ATF_MATH    { $$=ATFF_MATH; }
	|	ATF_UNICODE { $$=ATFF_UNICODE; }
	|	ATF_LEGACY  { $$=ATFF_LEGACY; }
	|	ATF_LEXICAL { $$=ATFF_LEXICAL; }
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
		
/* composite or transliteration */
blocks:		lines
	|	cblocks
	|	tblocks
	;

cblocks:
		cblock
	|	cblock lines
	|	cblocks cblock lines
	;

cblock:		division
	|	milestone
	;

division:	division_tok TEXT { atf_bld_block(@1, $1, $2); }
	;

milestone:	milestone_tok TEXT { atf_bld_block(@1, $1, $2); }
	;

tblocks:
		object
	|	tblocks object
		;


object: 	object_tok TEXT { atf_bld_block(@1, $1, $2); }
	|	object_tok TEXT { atf_bld_block(@1, $1, $2); } lines
	|	object_tok TEXT { atf_bld_block(@1, $1, $2); } surfaces
	|	object_tok TEXT { atf_bld_block(@1, $1, $2); } columns
	;

surfaces:	surface
	|	surfaces surface
	;

surface:
		surface_tok TEXT { atf_bld_block(@1, $1, $2); }
	|	surface_tok TEXT { atf_bld_block(@1, $1, $2); } lines
	|	surface_tok TEXT { atf_bld_block(@1, $1, $2); } columns
		;

columns:	column
	|	columns column
	;

column:		column_tok TEXT { atf_bld_block(@1, $1, $2); }
	| 	column_tok TEXT { atf_bld_block(@1, $1, $2); } lines
	;

lines:		line
	|	lines line
	;

/* The grammar doesn't impose constraints on prerequisites, sequence,
 * or repetition of line types--that has to be handled by validating
 * as we build
 */
line:
		MTS longtext		{ $$=$1; }
	| 	NTS longtext		{ $$=$1; } /* MTS prereq; singleton */
	| 	LGS longtext		{ $$=$1; } /* MTS prereq; singleton */
	| 	GUS longtext		{ $$=$1; } /* MTS prereq; singleton */
	| 	BIL longtext		{ $$=$1; } /* MTS prereq */
	| 	EXX longtext		{ $$=$1; } /* MTS prereq */
	| 	LEM longtext		{ $$=$1; } /* MTS|NTS|BIL prereq */
	|	l_link longtext		{ $$=$1; } /* MTS prereq */
	|	COMMENT longtext	{ $$=$1; }
	| 	DOLLAR longtext		{ $$=$1; }
	| 	bib
	| 	note
	|	heading
	;

l_link:		LNK_TOTO
	|	LNK_FROM
	|	LNK_PLUS
	|	LNK_VBAR
	;

division_tok:
		Y_DIV
	| 	Y_END
	| 	Y_ENDVARIANTS
	| 	Y_VARIANT
	| 	Y_VARIANTS
	;

milestone_tok:
		Y_BODY
	| 	Y_CATCHLINE
	| 	Y_CFRAGMENT
	| 	Y_COLOPHON
	| 	Y_DATE
	| 	Y_FRAGMENT
	| 	Y_INCLUDE
	| 	Y_LINECOUNT
	| 	Y_M
	| 	Y_REFERTO
	| 	Y_SEALINGS
	| 	Y_SIGNATURE
	| 	Y_SUMMARY
	| 	Y_WITNESSES
	;

object_tok:
		Y_BULLA
	| 	Y_ENVELOPE
	| 	Y_OBJECT
	| 	Y_PRISM
	| 	Y_TABLET
	;

surface_tok:
		Y_BOTTOM
	| 	Y_DOCKET
	| 	Y_EDGE
	| 	Y_FACE
	| 	Y_LEFT
	| 	Y_OBVERSE
	| 	Y_REVERSE
	| 	Y_RIGHT
	| 	Y_SEAL
	| 	Y_SIDE
	| 	Y_SURFACE	
	;

column_tok:
		Y_COLUMN
	;

heading:
		Y_H1X
	;

%%

void
atf_set_tree(Tree *tp)
{
  ytp = tp;
  atf_bld_tree(tp);
}

void
atferror(const char *e)
{
  mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}
