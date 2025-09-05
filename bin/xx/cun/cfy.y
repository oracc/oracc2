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

%token <i> EOL FILL GOESTO RETURN WORD ZWJ ZWNJ
%token <s> GRAPHEME LITERAL

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
	EOL
	| lelts GOESTO { elts_rhs = 1; } elts { cfy_cfg_stash(@1, &cfy); } EOL
	;

lelts:
	  elts
	| '^' elts
	| elts '$'
	| '^' elts '$'
	;

elts:
	elt
	| elts elt
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
  extern int yylineno;
  fprintf(stderr, "cfy-config: %s at line %d\n", e, yylineno);
}
