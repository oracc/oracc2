%define api.prefix {bbl}
%file-prefix"bbl"
%locations
%define parse.error verbose

%{

#include "bbl.h"

extern int yylex(void);
extern void yyerror(const char *);

int elts_rhs = 0;

#define BBLLTYPE_IS_DECLARED 1
#define BBLLTYPE Mloc
#define yylineno bbllineno
BBLLTYPE bbllloc;

%}

%union { char *s; int i; }

%token <i> BBL_DATALIST BBL_ENTRY
%token <s> BBL_TOKEN

%start bbl

%%

bbl:   datalist
     | bbl entry
     ;

datalist:
BBL_DATALIST '[' BBL_TOKEN ']' '{' BBL_TOKEN '}'	{ printf("found datalist %s\n", $3); }
        ;

entry:
	  BBL_ENTRY '{' BBL_TOKEN '}'		{ printf("found entry %s\n", $3); }
	  | '{' | '[' | ']' | '}'
	;
%%

void
yyerror(const char *e)
{
  fprintf(stderr, "yyerror: %s\n", e);
}
