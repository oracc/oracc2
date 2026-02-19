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

#define ATFLTYPE_IS_DECLARED 1
#define ATFLTYPE Mloc
#define yylineno atflineno
ATFLTYPE atflloc;

%}

%union { char *text; int i; }

%token	<text>  PQX QID TEXT WORD DOC PROJECT ATFPRO LEMMATIZER LINK KEY
		MILESTONE OBJECT SURFACE COLUMN DIVISION
		MTS NTS LGS GUS LEM
		BIB VAR NOTE VERSION
		ATF_LANG

%token  <i>     HASH_PROJECT HASH_LINK COMPOSITE SCORE MATRIX SYNOPTIC PARSED UNPARSED SWORD
		ATF_MYLINES ATF_AGROUPS ATF_MATH ATF_UNICODE ATF_LEGACY ATF_LEXICAL
		LINK_DEF LINK_PARALLEL LINK_SOURCE

%nterm <text>   pqx name

%nterm <i> 	doc sparse stype atfuse link_type

%start atf

%%

atf: amp
     | amp preamble
     | amp preamble blocks
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

protocol.start: project | atfpro | LEMMATIZER | VERSION | BIB | NOTE

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

key: 		KEY
		
blocks: 	block
	| 	blocks block
		;

block:
		locator-or-line
	| 	locator-or-line protocols.inter
		;

/* This grammar requires secondary enforcement of sequencing
   constraints, e.g., NTS must follow MTS, COLUMN must not precede
   SURFACE */
locator-or-line:
		MILESTONE
	| 	DIVISION
	| 	OBJECT
	| 	SURFACE
	| 	COLUMN
	| 	line
		;

line: MTS | NTS | LGS | GUS | LEM

protocols.inter:
		protocol.inter
	| 	protocols.inter protocol.inter
		;

protocol.inter: BIB | NOTE | VAR

%%

void
atferror(const char *e)
{
  mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}
