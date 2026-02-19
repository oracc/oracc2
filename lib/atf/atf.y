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

%token	<text>  PQX TEXT DOC PROJECT ATFPRO LEMMATIZER LINK KEY
		MILESTONE OBJECT SURFACE COLUMN DIVISION
		MTS NTS LGS GUS LEM
		BIB VAR NOTE VERSION

%token  <i>     HASH_PROJECT COMPOSITE SCORE MATRIX SYNOPTIC PARSED UNPARSED SWORD

%nterm <text>   pqx name

%nterm <i> 	doc sparse stype

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

plks: plk
  | plks plk
;

plk:      protocol.start
	| link
	| key
	;

protocol.start: project | ATFPRO | LEMMATIZER | VERSION | BIB | NOTE

project:	HASH_PROJECT PROJECT { atfp->project = $2; }
	;

link: LINK

key: KEY

blocks: block
  | blocks block
  ;

block:
    locator-or-line
  | locator-or-line protocols.inter
  ;

/* This grammar requires secondary enforcement of sequencing
   constraints, e.g., NTS must follow MTS, COLUMN must not precede
   SURFACE */
locator-or-line:
    MILESTONE
  | DIVISION
  | OBJECT
  | SURFACE
  | COLUMN
  | line
  ;

line: MTS | NTS | LGS | GUS | LEM

protocols.inter:
   protocol.inter
   | protocols.inter protocol.inter
   ;

protocol.inter: BIB | NOTE | VAR

%%

void
atferror(const char *e)
{
  mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}
