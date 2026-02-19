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

%token	<text>  PQX TEXT DOC PROJECT ATF LEMMATIZER LINK KEY
		MILESTONE OBJECT SURFACE COLUMN DIVISION
		MTS NTS LGS GUS LEM
		BIB VAR NOTE VERSION

%start atf

%%

atf: amp def preamble blocks

amp: pqx '=' name

pqx: PQX

name: TEXT

def: doc
     | project
     | doc project
     ;

doc: DOC

project: PROJECT

preamble: plk | preamble plk

plk:      protocol.start
	| link
	| key
	;

protocol.start: ATF | LEMMATIZER | VERSION

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
