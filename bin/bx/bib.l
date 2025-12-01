%option prefix="bib"
%option never-interactive yylineno nounput noyywrap

%{
#include <stdlib.h>
#include <string.h>
#include "bx.h"
#define BIBLTYPE Mloc
#include "bib.tab.h"
static void bib_user_action(void);
extern Mloc biblloc;
#define YY_USER_ACTION bib_user_action();
extern Bx *curr_bp;
extern const char *curr_bib;
extern int bib_entry, bib_field, bib_nesting;
int bib_key_next;

#include "bib.h"
#define lval(v) biblval.s = (char *)pool_copy((uccp)(v),curr_bp->p)
%}

/* Regular expressions for .bib elements */
TOKEN	    [a-zA-Z]+
CHUNK  	    [a-zA-Z0-9\x80-\xff'\-\_\\;:./()<>\[\]`'"&?#|+*~@!%]+
STRING      \"([^"\n]|\\.)*\"
WS  	    [ \t\n\r]+

%%

^[ \t]*@{TOKEN}[ \t]* { field = lval(bibtext); return BIB_ENTRY; }
{TOKEN}         { field = lval(bibtext);
		  return bib_field ? BIB_CHUNK : (bib_key_next ? BIB_KEY : BIB_FIELD); }
{CHUNK}	        { lval(bibtext); return bib_field ? BIB_CHUNK : BIB_KEY; }
{STRING}        { lval(bibtext); return bib_field ? BIB_CHUNK : BIB_STRING; }
"="             { return bib_field ? (lval(bibtext),BIB_CHUNK) : '='; }
"{"             { ++bib_nesting; return bib_field ? (lval(bibtext),BIB_CHUNK) : '{'; }
"}"             { --bib_nesting; return (bib_nesting>1) ? (lval(bibtext),BIB_CHUNK) : '}'; }
","             { return bib_field ? (lval(bibtext),BIB_CHUNK) : ','; }
^"%"[^\n]*/\n   { lval(bibtext); return COMMENT; }
{WS}   	        { lval(bibtext);
		  if (bib_field) { return BIB_CHUNK; }
		  else if (!bib_entry) { return COMMENT; }
		}
.               { fprintf(stderr, "%s:%d: unhandled character '%c'\n", curr_bib, yylineno, *bibtext);
		  return BIB_ERROR; }

<<EOF>>		{ bib_eof(); yyterminate();  }
%%

void bibreset(void) {
  bib_flush_buffer(YY_CURRENT_BUFFER);
}

const char * l_biblloc_file(void) { return biblloc.file; }
const int l_biblloc_line(void) { return biblloc.line; }

static void
bib_user_action(void)
{
  /*fprintf(stderr, "bib_user_action: token %s in %s:%d\n", bibtext, curr_bib, yylineno);*/
  biblloc.file = curr_bib;
  biblloc.line = yylineno;
}
