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
comment		^"#".*/[\n\r]
eltref		@[0-9]+
endrule		"."
equals		=
fill		"++"
goesto		"=>"
jeltl		"&Jl"
jeltr		"&Jr"
jelts		"&Js"
jeltp		"&Jp"
jeltcs		"&Jcs"
jeltcp		"&Jcp"
kv_ccf		[a-zA-Z0-9][-a-zA-Z0-9]*".ccf"
kv_key		cfy-[a-z]+-.*
kv_format     	(none|left|right|char-spread|char-penult|spread|penult|boxed|ruled|colruled)
kw_ccf		^cfy-ccf
kw_format     	^cfy-format
kw_key		^cfy-key
grapheme	([a-zA-Z0-9\x80-\xff]+)
literal		'"'[^\"\n\r]+'"'
member		[a-zA-Z0-9][a-zA-Z0-9_]
pqx		([a-z0-9/]+:)?[PQX][0-9]{6}
reltb		"&Rb";
reltl		"&Rl";
reltc		"&Rc";
return		"&r"
structmem      	"->"
value		"$"[a-zA-Z][a-zA-Z0-9_]*
word		"&w"
ws		[ \t\r\n]+
zwj		"&j"
zwnj		"&n"
zws		"&s"

%x C
%x J
%x M

%%

{anchor_e}	{ ++anchor_end; return *cfytext; }

{anchor_s}	{ ++anchor_start; return *cfytext; }

{comment}	{ ; }

{eltref}	{ lval(cfytext+1); return ELTREF; }

{endrule}	{ return *cfytext; }

{equals}	{ return *cfytext; }

{goesto}	{ return GOESTO; }

{grapheme} 	{ lval(cfytext); return GRAPHEME; }

{literal}      	{ return LITERAL; }

{jeltl}		{ return ELTJl; }

{jeltr}		{ return ELTJr; }

{jelts}		{ return ELTJs; }

{jeltp}		{ return ELTJp; }

{kw_ccf}       	{ BEGIN C; return KW_CCF; }

<C>{kv_ccf}	{ lval(cfytext); return CFYCCF; }

<C>{endrule}   	{ BEGIN 0; }

<C>{pqx}       	{ return PQX; }

<C>{ws}		{ ; }

{kw_key}       	{ return KW_KEY; }

{kv_key}       	{ lval(cfytext); return CFYKEY; }

{kw_format}	{ BEGIN J; return KW_FORMAT; }

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
