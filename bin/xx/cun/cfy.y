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

%token <i> FILL GOESTO KW_CCF KW_KEY KW_JUSTIFY RETURN STRUCTMEM WORD ZWJ ZWNJ ZWS
%token <s> CFYCCF CFYJUSTIFY CFYKEY ELTREF GRAPHEME JELTl JELTp JELTr JELTs
	   LITERAL MEMBER PQX VALUE

%start configs

%%

configs:  config
	| configs config
	;

config:   cfyccf
	| cfyjustify
	| cfykey
	| sub
	;

cfyccf: KW_CCF CFYCCF pqxs {  } ;

cfyjustify: KW_JUSTIFY CFYJUSTIFY { cfy_cfg_justify(@$, &cfy, $2); } ;

cfykey: KW_KEY CFYKEY  	{ cfy_cfg_key(@$, &cfy, $2); } ;

pqxs:     PQX
	| pqxs PQX
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

relts:    relt
	| relts relt
	;

relt:	  elt
	| assignment
	;

elts:     elt
	| elts elt
	;

assignment:
	  ELTREF				{ cfy_cfg_asgn(@1, &cfy, atoi($1), NULL, NULL); }
	| ELTREF '=' VALUE			{ cfy_cfg_asgn(@1, &cfy, atoi($1), NULL, $3); }
	| ELTREF STRUCTMEM MEMBER '=' VALUE	{ cfy_cfg_asgn(@1, &cfy, atoi($1), $3, $5); }
	;

elt:	  GRAPHEME 	{ cfy_cfg_elt_g(@1, &cfy, (uccp)$1); }
	| FILL		{ cfy_cfg_elt_f(@1, &cfy); }
	| JELTl		{ cfy_cfg_elt_Jl(@1, &cfy); }
	| JELTp		{ cfy_cfg_elt_Jp(@1, &cfy); }
	| JELTr		{ cfy_cfg_elt_Jr(@1, &cfy); }
	| JELTs		{ cfy_cfg_elt_Js(@1, &cfy); }
	| LITERAL      	{ cfy_cfg_elt_q(@1, &cfy, (uccp)$1); }
	| RETURN      	{ cfy_cfg_elt_r(@1, &cfy); }
	| WORD		{ cfy_cfg_elt_w(@1, &cfy); }
	| ZWJ		{ cfy_cfg_elt_j(@1, &cfy); }
	| ZWNJ		{ cfy_cfg_elt_n(@1, &cfy); }
	| ZWS		{ cfy_cfg_elt_s(@1, &cfy); }
	;

%%

void
yyerror(const char *e)
{
  extern int yylineno, cfy_cfg_status;
  extern const char *curr_ccf;
  mesg_vwarning(curr_ccf, yylineno, "%s\n", e);
  ++cfy_cfg_status;
}
