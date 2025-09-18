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

%}

anchor_e	"$"
anchor_s	"^"
comment		^"#".*/[\n\r]
eltref		@[0-9]+
endrule		"."
equals		=
fill		"++"
goesto		"=>"
xgrapheme	([^&@*$ \t\n\r=][^ >\t\n\r]*)
grapheme	([a-zA-Z0-9\x80-\xff]+)
literal		'"'[^\"\n\r]+'"'
member		[a-zA-Z0-9][a-zA-Z0-9_]
return		"&r"
structmem      	"->"
value		"$"[a-zA-Z][a-zA-Z0-9_]*
word		"&w"
ws		[ \t\r\n]+
zwj		"&j"
zwnj		"&n"

%x M

%%

{anchor_e}	{ ++anchor_end; return *cfytext; }

{anchor_s}	{ ++anchor_start; return *cfytext; }

{comment}	{ ; }

{eltref}	{ cfylval.s = (char *)pool_copy((uccp)cfytext+1,cfy.p); return ELTREF; }

{endrule}	{ return *cfytext; }

{equals}	{ return *cfytext; }

{goesto}	{ return GOESTO; }

{grapheme} 	{ cfylval.s = (char *)pool_copy((uccp)cfytext,cfy.p); return GRAPHEME; }

{literal}      	{ return LITERAL; }

<M>{member}	{ cfylval.s = (char*)pool_copy((uccp)cfytext,cfy.p); return MEMBER; }

{structmem}	{ BEGIN M; return STRUCTMEM; }

{value}		{ cfylval.s = (char*)pool_copy((uccp)cfytext,cfy.p); return VALUE; }

{word}		{ return WORD; }

{ws}		{ ; }

{zwj}		{ return ZWJ; }

{zwnj}		{ return ZWNJ; }

.		{ fprintf(stderr, "cfy.l: unhandled input `%s'\n", yytext); }

%%

static void
cfy_user_action(void)
{
  cfylloc.file = curr_ccf;
  cfylloc.line = yylineno;
}
