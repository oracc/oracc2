%option prefix="atf"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include "atf.h"

#define ATFLTYPE Mloc
#include "atf.tab.h"
#define YY_USER_ACTION atf_user_action();
extern Mloc atflloc;
const char *atffile = "<stdin>";
const char *curratffile = NULL;

int atfflextrace = 0;

#define atfpool(s) (char*)pool_copy((const unsigned char*)(s),atfmp->pool);
static void atf_user_action(void);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

#define atfsave atflval.text=atfpool(atftext)

%}
PQX	([PQX][0-9]{6})
BOM3	("\xef\xbb\xbf")
BOM2	("\xfe\xff")
LINK	("<<"|">>"|"++"|"||")
PROJECT [a-z][a-z0-9/]+

%s T
%x AMPl
%x AMPr
%x SCO
%x PRO

%%

^({BOM3}|{BOM2})?"&"         	{ BEGIN AMPl; }
<AMPl>{PQX}			{ atfsave; return PQX; }
<AMPl>=				{ BEGIN AMPr; return '='; }
<AMPl,AMPr>[ \t]+	       	{ ; }
<AMPr>.+/\n	       		{ BEGIN 0; atfsave; return TEXT; }

.				{ mesg_vwarning(curratffile, atflineno,
						"atf.l: unmatched character %c\n", *atftext); }

^"@composite"			{ return COMPOSITE; }
^"@score"		       	{ BEGIN SCO; return SCORE; }
<SCO>"matrix"			{ return MATRIX; }
<SCO>"synoptic"			{ return SYNOPTIC; }
<SCO>"parsed"			{ return PARSED; }
<SCO>"unparsed"			{ return UNPARSED; }
<SCO>"word"			{ return SWORD; }
<SCO>[ \t]			{ ; }
<SCO>\n				{ BEGIN 0; }

"#project:"			{ BEGIN PRO; return HASH_PROJECT; }
<PRO>{PROJECT}/[ \t]*\n		{ BEGIN 0; atfsave; return PROJECT; }

[ \t]+ 				{ ; }

.				{ mesg_vwarning(curratffile, atflineno,
						"atf.l: unmatched character %c\n", *atftext); }

%%

void
atf_lex_init(FILE *fp, const char *file)
{
#if 1
  static ssize_t fsize;
  char *buf = (char*)slurp("atf", file, &fsize);
#else
  YY_FLUSH_BUFFER;
  yyrestart(fp);
#endif
  atffile = curratffile = file;
  atflineno = 1;
#if 1
  /* slurp adds \n\0\0 but only counts \n in the returned fsize */
  atf_scan_buffer(buf,fsize+2);
#endif
}

static void
atf_user_action(void)
{
  if (atfflextrace)
    fprintf(stderr, "%s:%d: atflex: text=::%s::\n", curratffile, atflineno, atftext);

  atflloc.file = curratffile;
  atflloc.line = yylineno;
}
