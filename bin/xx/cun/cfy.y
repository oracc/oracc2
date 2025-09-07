%define api.prefix {cfy}
%file-prefix"cfy"
%locations
%define parse.error verbose

%{

#include "cfy.h"

extern int yylex(void);
extern void yyerror(const char *);

int elts_rhs = 0;

#define CFYLTYPE_IS_DECLARED 1
#define CFYLTYPE Mloc
#define yylineno cfylineno
CFYLTYPE cfylloc;

%}

%union { char *s; int i; }

%token <i> FILL GOESTO RETURN STRUCTMEM WORD ZWJ ZWNJ
%token <s> ELTREF GRAPHEME LITERAL MEMBER VALUE

%start sublist

%%

sublist:
	subs
	;

subs:
	  sub
	| subs sub
	;

sub:
	lelts GOESTO { elts_rhs = 1; } relts { cfy_cfg_stash(@$, &cfy); } '.'
	;

lelts:
	  elts
	| '^' elts
	| elts '$'
	| '^' elts '$'
	;

relts:    elts
	| assignment
	;

elts:     elt
	| elts elt
	;

assignment:
	  ELTREF '=' VALUE			{ cfy_cfg_asgn(@1, &cfy, atoi($1), NULL, $3); }
	| ELTREF STRUCTMEM MEMBER '=' VALUE	{ cfy_cfg_asgn(@1, &cfy, atoi($1), $3, $5); }
	;

elt:	  GRAPHEME 	{ cfy_cfg_elt_g(@1, &cfy, (uccp)$1); }
	| FILL		{ cfy_cfg_elt_f(@1, &cfy); }
	| LITERAL      	{ cfy_cfg_elt_q(@1, &cfy, (uccp)$1); }
	| RETURN      	{ cfy_cfg_elt_r(@1, &cfy); }
	| WORD		{ cfy_cfg_elt_w(@1, &cfy); }
	| ZWJ		{ cfy_cfg_elt_j(@1, &cfy); }
	| ZWNJ		{ cfy_cfg_elt_n(@1, &cfy); }
	;

%%

void
yyerror(const char *e)
{
  extern int yylineno, cfy_cfg_status;
  mesg_vwarning(cfy.config, yylineno, "%s\n", e);
  ++cfy_cfg_status;
}
