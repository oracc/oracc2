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
const char *currcfyfile = NULL;

#include "cfy.h"
int anchor_start, anchor_end;
static int endofinput = 0;


%}

anchor_e	"$"
anchor_s	"^"
comment		^"#".*/[\n\r]
eol		[\r\n]+
fill		"++"
goesto		"=>"
grapheme	([aeinuxAEINUX0-9]|[^& \t\n\r=>][^ \t\n\r]+)
literal		'"'[^\"\n\r]+'"'
return		"&r"
word		"&w"
ws		[ \t]+
zwj		"&j"
zwnj		"&n"

%%

{anchor_e}	{ ++anchor_end; return *cfytext; }

{anchor_s}	{ ++anchor_start; return *cfytext; }

{comment}	{ ; }

{eol}		{ return EOL; }

{goesto}	{ return GOESTO; }

{grapheme} 	{ cfylval.s = (char *)pool_copy((uccp)cfytext,cfy.p); return GRAPHEME; }

{literal}      	{ return LITERAL; }

{return}	{ return RETURN; }

{word}		{ return WORD; }

{ws}		{ ; }

{zwj}		{ return ZWJ; }

{zwnj}		{ return ZWNJ; }

.		{ fprintf(stderr, "cfy.l: unhandled input `%s'\n", yytext); }

<<EOF>>		{ if (!endofinput++) return EOL; else yyterminate(); }

%%

static void
cfy_user_action(void)
{
  cfylloc.file = currcfyfile;
  cfylloc.line = yylineno;
}
