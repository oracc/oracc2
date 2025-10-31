%option prefix="bbl"
%option never-interactive yylineno nounput noyywrap

%{
#include <stdlib.h>
#include <string.h>
#include <mesg.h>
#include <pool.h>

#define BBLLTYPE Mloc
#include "bbl.tab.h"
static void bbl_user_action(void);
extern Mloc bbllloc;
#define YY_USER_ACTION bbl_user_action();
const char *curr_bbl;
int need_token;
extern Pool *p;

#include "bbl.h"
#define lval(v) bbllval.s = (char *)pool_copy((uccp)(v),p)
%}

bracket		[{}\[\]]
comment		^[ \t]*%.*
datalist	"\\datalist"
entry		"\\entry"
token		[a-zA-Z0-9\x80-\xff/][-_a-zA-Z0-9\x80-\xff/]+
ws		[ \t\n\r]+

%%

{comment}	{ }
{bracket}	{ if (need_token) { if ('}' == *bbltext) need_token = 0; return *bbltext; } }
{datalist}	{ need_token = 1; return BBL_DATALIST; }
{entry}		{ need_token = 1; return BBL_ENTRY; }
{token}		{ if (need_token) { lval(bbltext); return BBL_TOKEN; } }
{ws}		{ }
[^ \t\n]  	{ } /* ignore any other input */

%%

static void
bbl_user_action(void)
{
  bbllloc.file = curr_bbl;
  bbllloc.line = yylineno;
}
