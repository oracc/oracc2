%option prefix="cfy"
%option never-interactive yylineno nounput noyywrap

%{
#include <stdlib.h>
#include <string.h>
#include <mesg.h>

#define CFYLTYPE Mloc
#include "cfy.tab.h"
static void cfy_user_action(void);
#define YY_USER_ACTION cfy_user_action();
extern Mloc cfylloc;
const char *curr_ccf = NULL;

#include "cfy.h"
int anchor_start, anchor_end;
#define lval(v) cfylval.s = (char *)pool_copy((uccp)(v),cfy.p)
%}

anchor_e	"$"
anchor_s	"^"

bstyle		("dashed"|"dotted"|"double"|"groove"|"ridge"|"inset"|"outset"|"solid")

colour		"#"([0-9a-fA-F]{3}|[0-9a-fA-F]{6})
comment		^"#".*/[\n\r]
eltref		@[0-9]+
endrule		"."
eol		[\r\n]+
equals		=
fill		"++"
goesto		"=>"

jeltl		"&Jl"
jeltr		"&Jr"
jeltc		"&Jc"
jelts		"&Js"
jeltp		"&Jp"
jeltcs		"&Jcs"
jeltcp		"&Jcp"

jkwl		"left"
jkwr		"right"
jkwc		"centre"
jkws		"spread"
jkwp		"penult"
jkwcs		"char-spread"
jkwcp		"char-penult"

kv_ccf		[a-zA-Z0-9][-a-zA-Z0-9]*".ccf"
kv_font_nm	([a-z0-9]+|cfy-[a-z]+-.*)
kv_font_pc	"%"[0-9]{2,3}
kv_grid_t	"objects"|"surfaces"|"columns"
kv_key		cfy-[a-z]+-.*
kw_ccf		^cfy-ccf
kw_colwidths	^cfy-colwidths
kw_font		^cfy-font
kw_format     	^cfy-format
kw_grid		^cfy-grid
kw_key		^cfy-key
kw_width	^cfy-width

auto		"auto"
grapheme	([a-zA-Z0-9\x80-\xff]+)
integer		[0-9]+
literal		'"'[^\"\n\r]+'"'
member		[a-zA-Z0-9][a-zA-Z0-9_]
pqx		([a-z0-9/]+:)?[PQX][0-9]{6}

reltb		"&Rb"
reltl		"&Rl"
reltc		"&Rc"

rkwb		"boxed"
rkwl		"ruled"
rkwc		"crule"

return		"&r"

sp		[ \t]+
structmem      	"->"

unit		([0-9][.0-9]*("em"|"px"|"□"|"%"))

value		"$"[a-zA-Z][a-zA-Z0-9_]*

word		"&w"
ws		[ \t\r\n]+
zwj		"&j"
zwnj		"&n"
zws		"&s"

%x B
%x C
%x F
%x G
%x J
%x JJ
%x M

%%

{anchor_e}	{ ++anchor_end; return *cfytext; }

{anchor_s}	{ ++anchor_start; return *cfytext; }

{colour}	{ lval(cfytext); return COLOUR; }

{comment}	{ ; }

{eltref}	{ lval(cfytext+1); return ELTREF; }

{endrule}	{ return *cfytext; }

{equals}	{ return *cfytext; }

{goesto}	{ return GOESTO; }

{grapheme} 	{ lval(cfytext); return GRAPHEME; }

{literal}      	{ return LITERAL; }

{jeltl}		{ return ELTJl; }

{jeltr}		{ return ELTJr; }

{jeltc}		{ return ELTJc; }

{jelts}		{ return ELTJs; }

{jeltp}		{ return ELTJp; }

{jeltcs}       	{ return ELTJcs; }

{jeltcp}       	{ return ELTJcp; }

{kw_ccf}       	{ BEGIN C; return KW_CCF; }

<C>{kv_ccf}	{ lval(cfytext); return CFYCCF; }

<C>{endrule}   	{ BEGIN 0; }

<C>{pqx}       	{ return PQX; }

<C>{ws}		{ ; }

{kw_grid}	{ BEGIN G; return KW_GRID; }

<G>{kv_grid_t}	{ lval(cfytext); return GRID_T; }

<G>{integer}	{ BEGIN 0; lval(cfytext); return GRID_N; }

<G>{auto}	{ BEGIN 0; lval(cfytext); return GRID_N; }

<G>{ws}		{ ; }

{kw_key}       	{ return KW_KEY; }

{kv_key}       	{ lval(cfytext); return CFYKEY; }

{kw_font}	{ BEGIN F; return KW_FONT; }

<F>{kv_font_pc} { lval(cfytext+1); return FONT_PC; }

<F>{kv_font_nm} { BEGIN 0; lval(cfytext); return FONT_NM; }

<F>{ws}		{ ; }

{kw_format}	{ BEGIN J; return KW_FORMAT; }

{kw_width}	{ BEGIN J; return KW_WIDTH; }

<J>{jeltl}	{ BEGIN 0; return ELTJl; }

<J>{jeltr}	{ BEGIN 0; return ELTJr; }

<J>{jeltc}	{ BEGIN 0; return ELTJc; }

<J>{jelts}	{ BEGIN 0; return ELTJs; }

<J>{jeltp}	{ BEGIN 0; return ELTJp; }

<J>{jeltcs}     { BEGIN 0; return ELTJcs; }

<J>{jeltcp}     { BEGIN 0; return ELTJcp; }

<J>{jkwl}	{ BEGIN 0; return ELTJl; }

<J>{jkwr}	{ BEGIN 0; return ELTJr; }

<J>{jkwc}	{ BEGIN 0; return ELTJc; }

<J>{jkws}	{ BEGIN 0; return ELTJs; }

<J>{jkwp}	{ BEGIN 0; return ELTJp; }

<J>{jkwcs}	{ BEGIN 0; return ELTJcs; }

<J>{jkwcp}	{ BEGIN 0; return ELTJcp; }

<J>{rkwb}	{ BEGIN B; return ELTRb; }

<J>{rkwl}	{ BEGIN B; return ELTRl; }

<J>{rkwc}	{ BEGIN B; return ELTRc; }

<J>{reltb}	{ BEGIN B; return ELTRb; }

<J>{reltl}	{ BEGIN B; return ELTRl; }

<J>{reltc}	{ BEGIN B; return ELTRc; }

<J>{unit}      	{ BEGIN 0; lval(cfytext); return UNIT; }

<J>{ws}		{ ; }

{kw_colwidths}	{ BEGIN JJ; return KW_COLWIDTHS; }

<JJ>{unit}     	{ lval(cfytext); return UNIT; }

<JJ>{endrule}	{ return *cfytext; }

<JJ>{ws}	{ ; }

<B>{unit}	{ lval(cfytext); return UNIT; }

<B>{colour}	{ lval(cfytext); return COLOUR; }

<B>{bstyle}	{ lval(cfytext); return BORDERSTYLE; }

<B>{sp}		{ ; }

<B>{eol}	{ BEGIN 0; }

{reltb}		{ return ELTRb; }

{reltl}		{ return ELTRl; }

{reltc}		{ return ELTRc; }

{return}	{ return RETURN; }

{structmem}	{ BEGIN M; return STRUCTMEM; }

<M>{member}	{ BEGIN 0; lval(cfytext); return MEMBER; }

{value}		{ lval(cfytext); return VALUE; }

{word}		{ return WORD; }

{ws}		{ ; }

{zwj}		{ return ZWJ; }

{zwnj}		{ return ZWNJ; }

{zws}		{ return ZWS; }

.		{ fprintf(stderr, "cfy.l: unhandled input `%s'\n", yytext); }

%%

static void
cfy_user_action(void)
{
  cfylloc.file = curr_ccf;
  cfylloc.line = yylineno;
}
