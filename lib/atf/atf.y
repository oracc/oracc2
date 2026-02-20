%define api.prefix {atf}
%file-prefix"atf"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include <string.h>
#include "atf.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *atftext, *curratffile;
extern int atflineno, atftrace;
extern int gdl_unicode;

Block *curr_block;
Blocktok *curr_blocktok;
Group *curr_group;

#define ATFLTYPE_IS_DECLARED 1
#define ATFLTYPE Mloc
#define yylineno atflineno
ATFLTYPE atflloc;

%}

%union { char *text; int i; }

%token	<text>		PQX QID TEXT WORD DOC PROJECT ATFPRO LEMMATIZER LINK KEY
			MILESTONE OBJECT SURFACE COLUMN DIVISION GROUP
			MTS NTS LGS GUS LEM BIL EXX DOLLAR
			BIB VAR NOTE VERSION
			ATF_LANG L_LINK

%token  <i>		HASH_PROJECT HASH_LINK HASH_VERSION HASH_BIB HASH_NOTE HASH_KEY
			HASH_LEMMATIZER LZR_SPARSE LZR_STOP
			COMPOSITE SCORE MATRIX SYNOPTIC PARSED UNPARSED SWORD
			ATF_MYLINES ATF_AGROUPS ATF_MATH ATF_UNICODE ATF_LEGACY ATF_LEXICAL
			LINK_DEF LINK_PARALLEL LINK_SOURCE
			COMMENT
			L_BIB L_NOTE BLANK
			Y_BAD
			Y_BODY Y_BOTTOM Y_BULLA Y_CATCHLINE Y_CFRAGMENT Y_COLOPHON Y_COLUMN
			Y_COMPOSITE Y_DATE Y_DIV Y_DOCKET Y_EDGE Y_END Y_ENDVARIANTS
			Y_ENVELOPE Y_FACE Y_FRAGMENT Y_H1 Y_H1X Y_H2 Y_H2X Y_H3 Y_H3X
			Y_INCLUDE Y_LABEL Y_LEFT Y_LINECOUNT Y_M Y_NOTE Y_OBJECT Y_OBVERSE
			Y_PRISM Y_REFERTO Y_REVERSE Y_RIGHT Y_SCORE Y_SEAL Y_SEALINGS Y_SIDE
			Y_SIGNATURE Y_SPAN Y_SUMMARY Y_SURFACE Y_TABLET Y_TOP Y_TRANSLATION
			Y_TRANSLITERATION Y_TRANSTYPE Y_UNIT Y_VARIANT Y_VARIANTS Y_WITNESSES

%nterm <text>   pqx name

%nterm <i> 	doc sparse stype atfuse link_type column
		division heading milestone object surface

%start atf

%%

atf: 		amp
	| 	amp preamble
	| 	amp preamble { atf_wrapup(WH_PREAMBLE); } blocks /* xcl */
		;

amp: pqx '=' name { atf_bld_amp(@1, $1, (uccp)$3); }

pqx: PQX

name: TEXT

preamble: plks
	| doc plks
	;

doc:		COMPOSITE			{
		    atfp->edoc = EDOC_COMPOSITE;
		}
	| 	SCORE stype sparse		{
		    atfp->edoc = EDOC_SCORE;
		    atfp->stype = $2; atfp->sparse = $3;
		}
	| 	SCORE stype sparse SWORD	{
		    atfp->edoc = EDOC_SCORE;
		    atfp->stype = $2; atfp->sparse = $3;
		    atfp->sword = EDOC_WORD;
		}
	;

stype:
	MATRIX { $$=EDOC_MATRIX; } | SYNOPTIC { $$=EDOC_SYNOPTIC; };

sparse: PARSED { $$=EDOC_PARSED; } | UNPARSED { $$=EDOC_UNPARSED; };

plks: 		plk
	| 	plks plk
		;

plk:      	protocol.start
	| 	link
	| 	key
		;

protocol.start: project | atfpro | lemmatizer | version | bib | note

lemmatizer:	HASH_LEMMATIZER LZR_SPARSE TEXT	{ atf_bld_protocol(@1, PROT_LZR_SPARSE, $3); }
	|	HASH_LEMMATIZER LZR_STOP WORD	{ atf_bld_protocol(@1, PROT_LZR_STOP, $3); }
	;

version:	HASH_VERSION TEXT    { atf_bld_protocol(@1, PROT_VERSION, $2); }
	;

bib:		HASH_BIB TEXT 	     { atf_bld_protocol(@1, PROT_BIB, $2); }
	;

note:		HASH_NOTE TEXT 	     { atf_bld_protocol(@1, PROT_NOTE, $2); }
	;

project:	HASH_PROJECT PROJECT { atfp->project = (uccp)$2; }
		;

atfpro:	       	ATF_LANG 	{ atf_lang(atfp, $1); };
	|	atfuse		{ atfp->flags |= $1; }
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
		
blocks: 	{ atf_bld_implicit_block(); }   group 	{ atf_wrapup(WH_GROUP); }
	| 	block group 				{ atf_wrapup(WH_GROUP); }
	| 	blocks block group 			{ atf_wrapup(WH_GROUP); }
		;

block:
	 	column
	| 	division
	|	heading
	|	milestone
	| 	object
	| 	surface
		;

column:		Y_COLUMN
	;

division:
		Y_DIV
	| 	Y_END
	| 	Y_ENDVARIANTS
	| 	Y_VARIANT
	| 	Y_VARIANTS
	;

heading:
		Y_H1X
	|	Y_H2X
	|	Y_H3X
	;

milestone:
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

object:
		Y_BULLA
	| 	Y_ENVELOPE
	| 	Y_OBJECT
	| 	Y_PRISM
	| 	Y_TABLET
	;

surface:
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
group:
		line_mts
	|	line_mts line_etc
		;

line_mts:	MTS
	;

line_etc:
		line_xxx
	| 	line_etc line_xxx
		;

line_xxx:
		NTS
	| 	LGS
	| 	GUS
	| 	BIL
	| 	EXX
	| 	LEM
	|	L_LINK
	| 	L_BIB
	| 	L_NOTE
	| 	COMMENT
	| 	DOLLAR
	| 	BLANK
		;

%%

void
atferror(const char *e)
{
  mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}
