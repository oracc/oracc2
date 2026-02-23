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
static int endofinput;
static int s_next;
static int linktext;
static char *xs;
const char *initial_longtext;

#define atfpool(s) (char*)pool_copy((const unsigned char*)(s),atfmp->pool);
#define atfnormpool(s) xs=(char*)normalize_ws((uccp)s);atflval.text=atfpool(xs);free(xs);

static int atf_block(const char *atcmd);
static void atf_user_action(void);

#define atfsave atflval.text=atfpool(atftext)
#define atfnorm atflval.text=atfnormpool(atftext)

#define longsave initial_longtext=atfpool(atftext)
#define usesave curr_use_str=atfpool(atftext)

#if 1
#define r(x) x
#else
#define r(x) fprintf(stderr, "returning %d\n", x),x
#endif

%}
PQX	([PQX][0-9]{6})
BOM3	("\xef\xbb\xbf")
BOM2	("\xfe\xff")
LK_TOTO ">>"
LK_FROM "<<"
LK_PLUS "++"
LK_VBAR "||"
PROJECT [a-z][a-z0-9/]+
QID	{PROJECT}:{PQX}
	
%x s_AMPl
%x s_AMPr
%x s_SCO
%x s_PRO
%x s_ATFPRO
%x s_LINK
%x s_LZR
%s s_T
%x s_WORD

%%

^({BOM3}|{BOM2})?"&"         	{ BEGIN s_AMPl; }
<s_AMPl>{PQX}			{ atfsave; return r(PQX); }
<s_AMPl>=	       		{ BEGIN s_AMPr; return r('='); }
<s_AMPr>[^\n]+      		{ BEGIN 0; atfnorm; return r(TEXT); }
<s_AMPl>[ \t]+	       		{ ; }

<s_T>^\t[^\n]+			{ atfnorm; return TAB; }
<s_T>[^\t][^\n]*           	{ BEGIN s_next; s_next=0; atfnorm; return r(TEXT); }

<s_T>^[ \t]+.*/\n[ \t]       	{ atfnorm; return TAB; }
<s_T>^[ \t]+.*/\n      		{ BEGIN 0; atfnorm; return TAB; }
<s_T>.+/\n[ \t]      		{ atfnorm; return TAB; }
<s_T>.+/\n			{ BEGIN 0; atfnorm; return TEXT; }
<s_T><<EOF>>			{ yyterminate(); }

<s_WORD>[^ \t\n]+	     	{ BEGIN s_next; s_next=0; atfsave; return WORD; }

^"@composite"			{ (void)atf_block(atftext); return COMPOSITE; }
^"@score"		       	{ BEGIN s_SCO; (void)atf_block(atftext); return SCORE; }
<s_SCO>"matrix"			{ return MATRIX; }
<s_SCO>"synoptic"	       	{ return SYNOPTIC; }
<s_SCO>"parsed"			{ return PARSED; }
<s_SCO>"unparsed"	       	{ return UNPARSED; }
<s_SCO>"word"			{ return SWORD; }
<s_SCO>[ \t]			{ ; }
<s_SCO>\n	       		{ BEGIN 0; }

^"#project:"			{ BEGIN s_PRO; return HASH_PROJECT; }
<s_PRO>{PROJECT}/\n		{ BEGIN 0; atfsave; return PROJECT; }

^"#atf:"			{ BEGIN s_ATFPRO; }
<s_ATFPRO>lang.*/\n	       	{ atfsave; return ATF_LANG; }
<s_ATFPRO>"use mylines"		{ usesave; return ATF_MYLINES; }
<s_ATFPRO>"use alignment-groups" { usesave; return ATF_AGROUPS; }
<s_ATFPRO>"use math"	       	{ usesave; return ATF_MATH; }
<s_ATFPRO>"use unicode"		{ usesave; return ATF_UNICODE; }
<s_ATFPRO>"use legacy"		{ usesave; return ATF_LEGACY; }
<s_ATFPRO>"use cuneiform"      	{ ; } /* no longer useful; cuneify is default */
<s_ATFPRO>"use lemconv"		{ fprintf(stderr, "lemconv is not implemented\n"); }/* ever used?*/
<s_ATFPRO>[ \t]			{ ; }
<s_ATFPRO>\n	       		{ BEGIN 0; }

^"#link"	       		{ linktext = 0; BEGIN s_LINK; return HASH_LINK; }
<s_LINK>"def"			{ s_next=s_LINK; BEGIN s_WORD; return LINK_DEF; }
<s_LINK>"parallel"		{ return LINK_PARALLEL; }
<s_LINK>"source"		{ return LINK_SOURCE; }
<s_LINK>{QID}			{ linktext = 2; atfsave; return QID; }
<s_LINK>=			{ if (linktext==2) BEGIN s_T; else if (linktext==1) BEGIN s_WORD;
				  linktext = 0; return '='; }
<s_LINK>[ \t]			{ ; }
<s_LINK>\n	       		{ BEGIN 0; }

^"#bib:"			{ BEGIN s_T; return HASH_BIB; }
^"#key:"			{ BEGIN s_T; return HASH_KEY; }
^"#note:"       		{ BEGIN s_T; return HASH_NOTE; }
^"#version:"			{ BEGIN s_T; return HASH_VERSION; }

^"#lemmatizer:"			{ BEGIN s_LZR; return HASH_LEMMATIZER; }

<s_LZR>"do"			{ ; }
<s_LZR>[ \t]+			{ ; }
<s_LZR>"sparse"			{ BEGIN s_T; return LZR_SPARSE; }
<s_LZR>"stop"			{ BEGIN s_WORD; return LZR_SPARSE; }
<s_LZR>\n	       		{ BEGIN 0; }

^@([a-z][a-z0-9]*) 		{ BEGIN s_T; return atf_block(atftext); }

^"$"  				{ BEGIN s_T; atfsave; return DOLLAR; }

^"#lem:"  			{ BEGIN s_T; atfsave; return LEM; }

^{LK_TOTO}  			{ BEGIN s_T; atfsave; return LNK_TOTO; }

^{LK_FROM}  			{ BEGIN s_T; atfsave; return LNK_FROM; }

^{LK_PLUS}  			{ BEGIN s_T; atfsave; return LNK_PLUS; }

^{LK_VBAR}  			{ BEGIN s_T; atfsave; return LNK_VBAR; }

^[^ \t\n]+\.[ \t]+ 		{ BEGIN s_T; longsave; return MTS; }

^[^ \t\n#]+:[ \t]+ 		{ BEGIN s_T; longsave; return EXX; }

^== 				{ BEGIN s_T; atfsave; return BIL; }

^\#   				{ BEGIN s_T; return COMMENT; }

[ \t]+ 				{ ; }

<<EOF>>				{ yyterminate(); }

.				{ mesg_vwarning(curratffile, atflineno,
						"atf.l: unmatched character %c\n", *atftext); }

%%

static int
atf_block(const char *atcmd)
{
  ++atcmd;

  Blocktok *bp = blocktok(atcmd, strlen(atcmd));
  if (bp)
    {
      curr_bt = bp;
      return atflval.i = bp->bison;
    }
  else
    {
      fprintf(stderr, "atf: bad at-command @%s\n", atcmd);
      return Y_BAD;
    }
}

void
atf_wrapup_buffer(void)
{
  atf_delete_buffer(YY_CURRENT_BUFFER);
}

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
