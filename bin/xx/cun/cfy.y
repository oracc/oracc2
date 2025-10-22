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

%token <i> FILL GOESTO KW_CCF KW_KEY KW_FONT KW_FORMAT KW_WIDTH KW_COLWIDTHS
	   RETURN STRUCTMEM WORD ZWJ ZWNJ ZWS KW_GRID KW_UI
%token <i> ELTRb ELTJcp ELTJcs ELTRl ELTRc ELTJl ELTJc ELTJp ELTJr ELTJs
%token <s> CFYCCF CFYKEY ELTREF GRAPHEME JELTl JELTp JELTr JELTs GRID_T GRID_N
	   FONT_NM FONT_PC LITERAL MEMBER PQX VALUE UNIT COLOUR BORDERSTYLE KV_UI

%type <i> rulingelt
%type <s> r_unit r_colour r_style

%start configs

%%

configs:  config
	| configs config
	;

config:   cfyccf
	| cfyfont
	| cfyformat
	| cfygrid
	| cfykey
	| cfyui
	| cfywidth
	| cfycolwidths
	| sub
	;

cfyccf: KW_CCF CFYCCF pqxs {  } ;

cfyfont: KW_FONT FONT_PC FONT_NM	{ cfy_cfg_font(@$, &cfy, $2, $3); }

cfyformat: KW_FORMAT eltf

cfygrid: KW_GRID GRID_T GRID_N		{ cfy_cfg_grid(@$, &cfy, $2, $3); }

cfyui: KW_UI KV_UI 			{ cfy_cfg_ui(@$, &cfy, $2); }

cfywidth: KW_WIDTH UNIT			{ cfy_cfg_width(@$, &cfy, $2); }
	;

cfycolwidths: KW_COLWIDTHS unitlist '.' { cfy_cfg_col_wrap(@$, &cfy); }
	;

unitlist: UNIT			{ cfy_cfg_col_unit(@$, &cfy, $1); }
	| unitlist UNIT		{ cfy_cfg_col_unit(@$, &cfy, $2); }
	;

eltf:	  ELTJl { cfy_cfg_justify(@$, &cfy, ELT_Jl); }
	| ELTJp { cfy_cfg_justify(@$, &cfy, ELT_Jp); }
	| ELTJcp { cfy_cfg_justify(@$, &cfy, ELT_Jcp); }
	| ELTJr { cfy_cfg_justify(@$, &cfy, ELT_Jr); }
	| ELTJs { cfy_cfg_justify(@$, &cfy, ELT_Js); }
	| ELTJcs { cfy_cfg_justify(@$, &cfy, ELT_Jcs); }
	| ruling
	;

ruling:
	  rulingelt			       	{ cfy_r(@$, &cfy, $1, NULL, NULL, NULL); }
	| rulingelt r_unit	       		{ cfy_r(@$, &cfy, $1, $2, NULL, NULL); }
	| rulingelt r_unit r_colour		{ cfy_r(@$, &cfy, $1, $2, $3, NULL); }
	| rulingelt r_unit r_style		{ cfy_r(@$, &cfy, $1, $2, NULL, $3); }
	| rulingelt r_unit r_colour r_style	{ cfy_r(@$, &cfy, $1, $2, $3, $4); }
	;

rulingelt:
	  ELTRb { $$ = ELT_Rb; }
	| ELTRl { $$ = ELT_Rl; }
	| ELTRc { $$ = ELT_Rc; }
	;

r_unit: UNIT ;

r_colour: COLOUR ;

r_style: BORDERSTYLE ;

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
	| ELTJl		{ cfy_cfg_elt_Jl(@1, &cfy); }
	| ELTJp		{ cfy_cfg_elt_Jp(@1, &cfy); }
	| ELTJr		{ cfy_cfg_elt_Jr(@1, &cfy); }
	| ELTJs		{ cfy_cfg_elt_Js(@1, &cfy); }
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
