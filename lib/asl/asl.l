%option prefix="asl"
%option never-interactive noyywrap yylineno nounput

%{
#include <ctype.h>
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include "asl.h"

#define ASLLTYPE Mloc
#include "asl.tab.h"
#define YY_USER_ACTION asl_user_action();
extern Mloc asllloc;
const char *aslfile = "<stdin>";
const char *curraslfile = NULL;

int endofinput = 0;
int aslflextrace = 0;
int fake_flag = 0;
int Ynext = 0;

extern Pool *aslpool;
#define xaslpool(s) (char*)pool_copy((const unsigned char*)(s),aslpool);
static void asl_user_action(void);
static int asl_grapheme(const char *gp);
static void check_minus(char *t);
static void strip_literal(char *t);
static char *skip_initial_ws(char *t);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

#define aslsave   asllval.text=xaslpool(asltext)

#define aslsave_w asllval.text=xaslpool(skip_initial_ws(asltext))

#define atcmd(at,st) aslsave; check_minus(asllval.text); pmatch("###ATCMD\n"); BEGIN((st)); return(at)

#define Tcmd(at,st) pmatch("###TCMD\n"); BEGIN((st)); return(at)

#define no_comments(where) mesg_vwarning( \
					  curraslfile, asllineno, \
					  "[asl] #comments no longer allowed; use @inote, @v- or the like")

%}

S		\xe2\xb8[^\xa2-\xa5]
G		([a-zA-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|{S}|\xc3[^\x97]|\xe2[^\xb8])+
M		([@~]([a-z]+[0-9]*|90|180|270))*
N		(N[0-9]+[ABCDE]*)
Q		\??
L		=?
COMPOUND	"|"([a-zA-Z\x80-\xff]|[@~().%&+()0-9])+"|"{M}{Q}{L}
GRAPHEME 	((({G}|{N}){M})|"15"){Q}{L}
ZERO		"00"(~[a-d])?
PHONGRAPH	"/"{G}"/"
LISTNAME	(U\+|[A-Z]+)
LISTNUM		(U\+[A-F0-9]+|[A-Z]+[0-9]+[()A-Z0-9a-z_'"+*/^]*{M}{Q}{L})
NUMGRAPH	[0-9XnN]+(\/[0-9])?\(({GRAPHEME}|{COMPOUND})(′|″)?\){M}{Q}{L}
FRACTION	[0-9]\/[0-9]
PUNCT		("*"|:['".:]?|"/"){Q}{L}
SYSNAME		[a-zA-Z][a-zA-Z0-9_:-]+
OTF		("."|ss|cv)[0-9]+
OTFEAT		{OTF}([ \t\n]+{OTF})*

LIGATURE       	({GRAPHEME}|{COMPOUND})([-\+]({GRAPHEME}|{COMPOUND}))+
LIGAFONT	(u[0-9A-F]+)(_u[0-9A-F]+)+
UTF8SEQ		([\xc2-\xdf][\x80-\xbf]|\xe0[\xa0-\xbf][\x80-\xbf]|[\xe1-\xec][\x80-\xbf][\x80-\xbf]|\xed[\x80-\x9f][\x80-\xbf]|[\xee\xef][\x80-\xbf][\x80-\xbf]|\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]|\xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf])
ZW		(\xE2\x80[\x8C\x8D])
UCUNSEQ		(\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]|\xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf]|{ZW})
UTF8UCUN	{UCUNSEQ}([ \t\n]*{UCUNSEQ})*

%s E
%x F
%x G
%s L
%x LES
%s S
%s T
%x Y
%x Yt
%x X

%%

<E>"sign"			{ fake_flag = 0; atcmd(ESIGN,0); }

<E>"pcun"			{ fake_flag = 0; atcmd(EPCUN,0); }

<E>"xsux"			{ fake_flag = 0; atcmd(EXSUX,0); }

<E>"form"			{ fake_flag = 0; atcmd(EFORM,0); }

<E>[^ \t]			{ fake_flag = 0; aslsave; fprintf(stderr, "###EBAD %s\n", asltext); return EBAD; }

<F>"*"?[0-9]+			{ aslsave; return GLYF_TAG; }
<F>{UCUNSEQ}			{ aslsave; return GLYF_UNI; }
<F>FA[0-9A-F]{3}		{ aslsave; return GLYF_HEX; }
<F>{OTFEAT}			{ aslsave; return GLYF_OTF; }
<F>[ \t]			{ }
<F>\n		       		{ BEGIN(0); return EOL; }

<G>{LIGATURE}/[ \t]+         	{ aslsave; return LIGATURE; }
<G>{LIGAFONT}/[ \t]+		{ aslsave; return LIGAFONT; }
<G>{UTF8UCUN}/[ \t\n]	       	{ aslsave; BEGIN 0 ;return LIGAUCUN; }
<G>[ \t]			{ }
<G>\n		       		{ BEGIN(0); return EOL; }

<L>{LISTNAME}/[ \t]+         	{ aslsave; BEGIN(S); return LISTNAME; }

<LES>"<"[^ \t\n]+      		{ aslsave; return LISTIMAGE; }
<LES>{LISTNUM}   	 	{ aslsave; return LISTNUM; }
<LES>{UTF8UCUN}			{ aslsave; return UTOKEN; }
<LES>{GRAPHEME}|{COMPOUND}|{NUMGRAPH} { aslsave; return asl_grapheme(asllval.text); }
<LES>{OTFEAT}			{ aslsave; return OTFEAT; }
<LES>[^ \t\n]+			{ aslsave; return ATF; } /* Not really ATF but satisfies the atftoken grammar */
<LES>[ \t]+			{ }
<LES>\n/@			{ BEGIN 0; pmatch("###<LES>eol\n"); return EOL; }
<LES>\n/[ \t]*\n	       	{ BEGIN 0; pmatch("###<LES>eol\n"); return EOL; }
<LES>\n				{ }

<S>[ \t]+			{ BEGIN T; }

<T>^"#".*/\n   			{ BEGIN 0; no_comments("text"); }
<T>^"#".*/\n@   	       	{ BEGIN 0; no_comments("text"); }
<T>^[ \t]+.*/\n@          	{ BEGIN 0; aslsave; return TAB; }
<T>^[ \t]+.*/\n			{ aslsave; return TAB; }
<T>.+/\n@			{ BEGIN 0; aslsave; return TEXT; }
<T>.+/\n			{ aslsave; return TEXT; }
<T>\n				{ }
<T><<EOF>>			{ if (!endofinput++) return EOL; else yyterminate(); }

<X>([^"\n]|\\\")*\"?[ \t]*/\n   { BEGIN 0; aslsave; strip_literal(asllval.text); return LITERAL; }
\"				{ BEGIN X; }

<Y>{SYSNAME}/\n         	{ aslsave; BEGIN Yt; return SYSNAME; }
<Y>{SYSNAME}/[ \t]+         	{ aslsave; BEGIN Yt; return SYSNAME; }
<Yt>([^= \n]|=[^>])+/"=>"     	{ aslsave; return SYSGOESTO; }
<Yt>[^\n]+/\n			{ aslsave_w; BEGIN(0); return SYSTEXT; }
<Y,Yt>[ \t]+   			{ }

^@@				{ return EFORM; }
^@end/[ \t].*\n        	       	{ atcmd(END,E); }
^@signlist/[ \t]+      		{ atcmd(SIGNLIST,S); }
^@project/[ \t]+      		{ atcmd(PROJECT,S); }
^@domain/[ \t]+      		{ atcmd(DOMAIN,S); }
^@linkdef/[ \t]+       		{ Ynext=L; atcmd(LINKDEF,Y); }
^@listdef/[ \t]+       		{ atcmd(LISTDEF,L); }
^@sysdef/[ \t]+       		{ Ynext=Yt; atcmd(SYSDEF,Y); }
^@scriptdef/[ \t]+             	{ Tcmd(SCRIPTDEF,S); }
^@images/[ \t]+			{ atcmd(IMAGES,S); }
^@sign-?/[ \t]       		{ atcmd(SIGN,0); }
^@pcun-?/[ \t]       		{ atcmd(PCUN,0); }
^@xsux-?/[ \t]       		{ atcmd(XSUX,0); }
^@aka/[ \t]       		{ atcmd(AKA,0); }
^@smap/[ \t]+			{ atcmd(SMAP,0); }
^@compoundonly/[ \t]           	{ atcmd(COMP,0); }
^@pname/[ \t]       		{ atcmd(PNAME,0); }
^@nosign/[ \t]     		{ mesg_vwarning(curraslfile, asllineno, "[asl] '@nosign' has been replaced by '@sign-'");
				  /*atcmd(SIGN,-1);*/ }
^@glyf/[ \t]       		{ atcmd(GLYF,F); }
^@liga-?/[ \t]       		{ atcmd(LIGA,G); }
^@list-?/[ \t]       		{ atcmd(LIST,LES); }
^@lref/[ \t]+			{ atcmd(LIST,LES); }
^@form[-+]?/[ \t]              	{ atcmd(FORM,0); }
^@noform/[ \t]         		{ mesg_vwarning(curraslfile, asllineno, "[asl] '@noform' has been replaced by '@form-'");
				  /* atcmd(FORM,-1);*/ }
^@v-?/[ \t]+	       		{ atcmd(V,0); }
^@link/[ \t]+			{ Ynext=0; atcmd(LINK,Y); }
^@sys/[ \t]+			{ Ynext=0; atcmd(SYS,Y); }
^@inote/[ \t]+       		{ Tcmd(INOTE,S); }
^@lit/[ \t]+	       		{ Tcmd(LIT,S); }
^@script/[ \t]+             	{ Tcmd(SCRIPT,S); }
^@merge/[ \t]+       		{ Tcmd(MERGE,S); }
^@note/[ \t]+       		{ Tcmd(NOTE,S); }
^@oid/[ \t]+       		{ Tcmd(OID,S); }
^@ref/[ \t]+	       		{ Tcmd(REF,S); }
^@ucun/[ \t]+			{ Tcmd(UTF8,S); }
^@umap/[ \t]+			{ Tcmd(UMAP,S); }
^@uhex/[ \t]+			{ mesg_vwarning(curraslfile, asllineno, "[asl] '@uhex' should not appear in a signlist; say @list U+HHHHH"); }
^@useq/[ \t]+     		{ Tcmd(USEQ,S); }
^@uname/[ \t]+			{ Tcmd(UNAME,S); }
^@unote/[ \t]+   		{ Tcmd(UNOTE,S); }
^@uage/[ \t]+			{ Tcmd(UAGE,S); }
^@upua/[ \t]+			{ Tcmd(UPUA,S); }
^@fake[ \t]+1[ \t]*/\n		{ return FAKE; }
^@sref/[ \t]+			{ atcmd(SREF,0); }

^@[a-z]+         		{ BEGIN 0; mesg_vwarning(curraslfile, asllineno,
							"[asl] malformed or unknown @-line: %s", asltext); }

{ZERO}      			{ aslsave; return GNAME; }

"1/3(|NINDA₂×(ŠE.AŠ)|)"		{ aslsave; return GVALUE; }
"1/3(|NINDA₂×(ŠE.1(AŠ))|)"  	{ aslsave; return GVALUE; }
"2/3(|NINDA₂×(ŠE.AŠ.AŠ)|)"    	{ aslsave; return GVALUE; }
"2/3(|NINDA₂×(ŠE.2(AŠ))|)"  	{ aslsave; return GVALUE; }

{NUMGRAPH}			{ aslsave; return asl_grapheme(asllval.text); }

{FRACTION}			{ aslsave; mesg_vwarning(curraslfile, asllineno,
							 "[asl] unqualified FRACTION not allowed: %s", asltext);
				  return GVALUE; }

{GRAPHEME}|{COMPOUND}          	{ aslsave; return asl_grapheme(asllval.text); }

{LISTNUM}   	 		{ aslsave; return LISTNUM; }

{PHONGRAPH}	       		{ aslsave; return GVALUE; }

{PUNCT}				{ aslsave; return GVALUE; }

[^-_ \t]+[-_\{\}][^ \t\n]+	{ aslsave; return ATF; }
[^{} \t]*[{}][^ \t\n]*/\n      	{ aslsave; return ATF; }

"%"[-a-z0-9]+("/n")?   		{ aslsave; return LANG; }

"~"[^ \t\n]+			{ mesg_vwarning(curraslfile, asllineno, "[asl] form variant tags like ~a are no longer used"); }

^"#".*/\n	       		{ no_comments("@v"); }

^[^ \t\n@].*/\n			{ pmatch("###bad\n"); return BAD; }

\n([ \t]*)+/\n			{ BEGIN 0; pmatch("###par\n"); return PAR; }

"=>"				{ return GOESTO; }

[ \t]+				{ }

\n				{ pmatch("###eol\n"); return EOL; }

\?				{ mesg_vwarning(curraslfile, asllineno,
						"[asl] misplaced '?'; say, e.g., @v ba?", *asltext); }

.				{ if (mesg_last_line()!=asllineno)
				    mesg_vwarning(curraslfile, asllineno,
						  "[asl] unmatched character %c", *asltext); }

<<EOF>>				{ if (!endofinput++) return EOL; else yyterminate(); }

%%

static int
asl_grapheme(const char *gp)
{
  const char *gp_orig = gp;
  while (*gp)
    if (*gp <= 127 && islower(*gp))
      return GVALUE;
    else if (*gp <= 127 && isupper(*gp))
      return GNAME;
    else
      ++gp;
  if (!strcmp(gp_orig, "15") || ('0' == gp[0] && '0' == gp[1]))
    return GNAME;
  else
    return GVALUE;
}

static void
asl_user_action(void)
{
#if 1
  if (asl_flex_debug)
    fprintf(stderr, "%s:%d: asllex: text=::%s::; start=%d\n", curraslfile, asllineno, asltext,yy_start);
#endif

  asllloc.file = curraslfile;
  asllloc.line = yylineno;
}

static void
check_minus(char *t)
{
  char *m = strrchr(t,'-');
  extern int minus_flag;
  if (m && (!m[1] || !strcmp(m, "-+")))
    {
      if ('+' == m[1])
	mesg_vwarning(curraslfile, asllineno, "[asl] '-' not allowed with '+')", *asltext);
      else if (strcmp(t, "@sign-") && strcmp(t, "@form-") && strcmp(t, "@list-") && strcmp(t, "@v-"))
	mesg_vwarning(curraslfile, asllineno, "[asl] '-' not allowed on %s (only on @sign/@form/@list/@v)", *asltext);
      else
        minus_flag = 1;
    }
  else
    minus_flag = 0;
  m = strchr(t,'+');
  if (m && (!m[1] || !strcmp(m, "+-")))
    {
      if ('-' == m[1])
	mesg_vwarning(curraslfile, asllineno, "[asl] '-' not allowed with '+')", *asltext);
      else if (strcmp(t, "@form+"))
	mesg_vwarning(curraslfile, asllineno, "[asl] '+' not allowed on %s (only on @form)", *asltext);
      else
        minus_flag = -1;
    }
}

static char *
skip_initial_ws(char *t)
{
  while (' ' == *t || '\t' == *t)
    ++t;
  return t;
}

static void
strip_literal(char *t)
{
  char *e = t + strlen(t);
  while (e > t && isspace(e[-1]))
    --e;
  if (e > t && e[-1] == '"')
    e[-1] = '\0';
  else
    {
      *e = '\0';
      mesg_vwarning(curraslfile, asllineno, "[asl] missing closing \"-quote on LITERAL");
    }
}
