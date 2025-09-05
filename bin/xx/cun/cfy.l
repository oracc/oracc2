%option prefix="cfy"
%option never-interactive yylineno nounput noyywrap

%{
#include <stdlib.h>
#include <string.h>
#include "cfy.tab.h"
#include "cfy.h"
int anchor_start, anchor_end;
%}

anchor_e	"$"
anchor_s	"^"
comment		^"#".*/[\n\r]
eol		"[\r\n]+"
fill		"++"
goesto		"=>"
grapheme	([aeinuxAEINUX0-9]|[^& \t\n\r=>].+)
literal		'"'[^\"\n\r]+'"'
return		"&r"
word		"&w"
ws		[ \t]+
zwj		"&j"
zwnj		"&n"

%%

{anchor_e}	{ ++anchor_end; return *cfytext; }

{anchor_s}	{ ++anchor_start; return *cfytext; }

{comment}	{ }

{eol}		{ return EOL; }

{goesto}	{ return GOESTO; }

{grapheme} 	{ cfylval.s = (char *)pool_copy((uccp)cfytext,cfy.p); return GRAPHEME; }

{literal}      	{ return LITERAL; }

{return}	{ return RETURN; }

{word}		{ return WORD; }

{ws}		{ }

%%
