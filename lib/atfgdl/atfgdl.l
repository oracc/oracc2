%option prefix="ag"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include <atf.h>
#include <gdl.h>
#include <gdlstate.h>
#include "ag.h"

static int ag_gdl_grapheme_type_l(void);
static int ag_gdl_grapheme_type_s(void);

#define AGLTYPE Mloc
#include "ag.tab.h"
#define YY_USER_ACTION ag_user_action();
extern Mloc agloc;
const char *agfile = "<stdin>";
const char *curragfile = NULL;
static int last_unmatched_line = 0;

int agflextrace = 0, last_aglineno;
static int s_next;
static int linktext, tra_text;
static char *xs;

#define agpool(s) (char*)pool_copy((const unsigned char*)(s),atfmp->pool);
#define agnormpool(s) xs=(char*)normalize_ws((uccp)s);aglval.text=agpool(xs);free(xs);s=xs=NULL

static int ag_block(const char *atcmd);
static void ag_user_action(void);
static int ag_gdl_grapheme_type_l(void);
static int ag_gdl_grapheme_type_s(void);

#define agsave aglval.text=agpool(agtext)
#define agnorm aglval.text=agnormpool(agtext)

#define longinit longtext_init(atfmp->pool,agtext)
#define usesave curr_use_str=agpool(agtext)

int atf_term_flag = 0;
#define atf_yyterminate() return (atf_term_flag = !atf_term_flag) ? ATF_END : YY_NULL

#if 1
#define r(x) x
#else
#define r(x) fprintf(stderr, "returning %d\n", x),x
#endif

			/* GDLPROLOGUE */
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
LANG	[a-z][a-z][a-z]?(-[-a-z]+)?
use	"use"[ \t]+			
NOD 	[^ \t\n\r.:]
NON 	[^ \t\n\r]
WHT 	[ \t\n\r]
			/* */
TRA_CHR [@\"?\[\]\{\}]
TRA_CMD (@[a-z0-9]+("["[^\]]+"]")?)|@\?|\?@|@\"|\"@|\{|\}|\^[0-9]+\^
TRA_LAB ^@\([^)@]+\)
TRA_PAR [\n\r]+[ \t]*[\n\r]+
TRA_WHT [ \t\n\r]+
TRA_WRD [^ \n\r\t\@\"\?\[\]\{\}]+

			/* GDLPATDEF */

%x s_AMPl
%x s_AMPr
%x s_SCO
%x s_PRO
%x s_ATFPRO
%x s_LINK
%x s_LZR
%x s_T
%x s_WORD
%x s_TRD
%x s_TRA
%x s_IGNORE
%x A
%x C
%s Q
%s S
%x TC
%x TD
%x N

			/* GDLSTARTS */
%%

<INITIAL,A>^({BOM3}|{BOM2})?"&"  		{ BEGIN s_AMPl; s_next = A; }
<s_AMPl>{PQX}			{ agsave; return r(PQX); }
<s_AMPl>[ \t]+=	       		{ BEGIN s_AMPr; return r('='); }
<s_AMPl>[ \t]+/[^=]    		{ mesg_verr(&aglloc, "no '=' in &-line");
				  BEGIN s_AMPr; return r('='); }
<s_AMPr>[^\n\r]+      		{ BEGIN A; atfnorm; return r(TEXT); }

<s_T>^[\t ]*/[\n\r]	       	{ ; }
<s_T>^[^\t ][^\n\r]*           	{ BEGIN s_next; s_next = A; atfnorm; return r(TEXT); }
<s_T>.*/\r?\n[ \t]+[\r\n]    	{ BEGIN s_next; s_next = A; atfnorm; return TEXT; }
<s_T>.*/\r?\n[^ \t]+		{ BEGIN s_next; s_next = A; atfnorm; return TEXT; }
<s_T>.*/\r?\n	       		{ BEGIN s_next; s_next = A; atfnorm; return TEXT; }
<s_T>^[\t ]+.+/[\r\n]  		{ BEGIN s_next; s_next = A; atfnorm; return TAB; }
<s_T><<EOF>>			{ return TEXT; }

<s_WORD>[^ \t\n\r]+	     	{ BEGIN s_next;
				  int ret=(s_next==s_TRD?TR_PROJ:WORD); s_next = A; agsave; return ret; }
<s_WORD>[ \t]+	       		{ ; }

<INITIAL,A>^"@composite"			{ (void)ag_block(agtext); return COMPOSITE; }
<INITIAL,A>^"@score"		       	{ BEGIN s_SCO; (void)ag_block(agtext); return SCORE; }
<s_SCO>"matrix"			{ return MATRIX; }
<s_SCO>"synoptic"	       	{ return SYNOPTIC; }
<s_SCO>"parsed"			{ return PARSED; }
<s_SCO>"unparsed"	       	{ return UNPARSED; }
<s_SCO>"word"			{ return SWORD; }
<s_SCO>[ \t]			{ ; }
<s_SCO>[\n\r]	       		{ BEGIN A; }

<INITIAL,A>^"#project:"			{ BEGIN s_PRO; return HASH_PROJECT; }
<s_PRO>{PROJECT}/[ \t]*[\n\r]   { BEGIN A; agsave; return PROJECT; }
<s_PRO>[ \t]+			{ ; }

<INITIAL,A>^"#atf:"			{ BEGIN s_ATFPRO; }
<s_ATFPRO>lang.*/[\n\r]	       	{ agsave; return ATF_LANG; }
<s_ATFPRO>{use}mylines		{ usesave; return ATF_MYLINES; }
<s_ATFPRO>{use}line-labels    	{ usesave; return ATF_LINELABELS; }
<s_ATFPRO>{use}alignment-groups { usesave; return ATF_AGROUPS; }
<s_ATFPRO>{use}math	       	{ usesave; return ATF_MATH; }
<s_ATFPRO>{use}ascii		{ usesave; return ATF_ASCII; }
<s_ATFPRO>{use}unicode		{ usesave; return ATF_UNICODE; }
<s_ATFPRO>{use}legacy		{ usesave; return ATF_LEGACY; }
<s_ATFPRO>{use}lexical		{ usesave; return ATF_LEXICAL; }
<s_ATFPRO>{use}cuneiform      	{ ; } /* no longer useful; cuneify is default */
<s_ATFPRO>{use}lemconv		{ fprintf(stderr, "lemconv is not implemented\n"); }/* ever used?*/
<s_ATFPRO>[ \t]			{ ; }
<s_ATFPRO>[\n\r]       		{ BEGIN A; }

<INITIAL,A>^"#link:"	       		{ linktext = 0; BEGIN s_LINK; return HASH_LINK; }
<s_LINK>"def"			{ s_next=s_LINK; BEGIN s_WORD; return LINK_DEF; }
<s_LINK>"parallel"		{ return LINK_PARALLEL; }
<s_LINK>"source"		{ return LINK_SOURCE; }
<s_LINK>{QID}			{ linktext = 2; agsave; return QID; }
<s_LINK>=			{ if (linktext==2) BEGIN s_T; else if (linktext==1) BEGIN s_WORD;
				  linktext = 0; return '='; }
<s_LINK>[ \t]			{ ; }
<s_LINK>[\n\r]	       		{ BEGIN A; }

<INITIAL,A>^"#bib:"			{ BEGIN s_T; return in_preamble ? HASH_PROBIB : HASH_BIB; }
<INITIAL,A>^"#key:"			{ BEGIN s_T; return HASH_KEY; }
<INITIAL,A>^"#note:"       		{ BEGIN s_T; return HASH_NOTE; }
<INITIAL,A>^"#version:"			{ BEGIN s_T; return HASH_VERSION; }

<INITIAL,A>^"#etcsl:"/[ \t]+t     		{ BEGIN s_T; return in_preamble ? HASH_ETCSL_T : HASH_ETCSL_L; }
<INITIAL,A>^"#etcsl:"/[ \t]+l     		{ BEGIN s_T; return HASH_ETCSL_L; }

<INITIAL,A>^"#lemmatizer:"			{ BEGIN s_LZR; return HASH_LEMMATIZER; }

<s_LZR>"do"			{ ; }
<s_LZR>[ \t]+			{ ; }
<s_LZR>"sparse"			{ BEGIN s_T; return LZR_SPARSE; }
<s_LZR>"stop"			{ BEGIN s_WORD; return LZR_SPARSE; }
<s_LZR>[\n\r]	       		{ BEGIN A; }

<INITIAL,A>^@([a-z][a-z0-9]*) 		{ BEGIN s_T; return ag_block(agtext); }

<INITIAL,A>^"$"  			{ BEGIN s_T; agsave; return DOLLAR; }

<INITIAL,A>^"#lem:"  			{ BEGIN s_T; agsave; return LEM; }

<INITIAL,A>^#tr(.[a-z-]+)?:		{ BEGIN s_T; longinit; return TR_INTER; }

<INITIAL,A>^{LK_TOTO}  			{ BEGIN s_T; longinit; return LNK_TOTO; }

<INITIAL,A>^{LK_FROM}  			{ BEGIN s_T; longinit; return LNK_FROM; }

<INITIAL,A>^{LK_PLUS}  			{ BEGIN s_T; longinit; return LNK_PLUS; }

<INITIAL,A>^{LK_VBAR}  			{ BEGIN s_T; longinit; return LNK_VBAR; }

<INITIAL,A>^[^ #@$\t\n\r]+\./[ \t\n\r]+        	{ BEGIN 0; return MTS; }

<INITIAL,A>^[^ #@$\t\n\r]+:/[ \t\n\r]+        	{ BEGIN 0; return EXX; }

<INITIAL,A>^:[^:#@$]+:[ \t]+	       	{ BEGIN 0; return VAR; }

<INITIAL,A>^== 				{ BEGIN s_T; agsave; return BIL; }

<INITIAL,A>^#   				{ BEGIN s_T; return in_preamble ? PRO_COMMENT : COMMENT; }

<s_TRD>("labeled"|"parallel"|"unitary")	{ agsave; tra_text=0; return TR_TYPE; }
<s_TRD>{LANG}				{ agsave; s_next=s_TRD; BEGIN s_WORD; return TR_LANG; }
<s_TRD>[ \t]+				{ ; }
<s_TRD>"\n"				{ BEGIN s_TRA; }
<s_TRA>^@h[123]				{ agsave; tra_text=0; s_next=s_TRA; BEGIN s_T; longinit; return TR_HDR; }
<s_TRA>^@label[^\n\r]*/[\n\r]	       	{ agsave; tra_text=1; return TR_LABEL; }
<s_TRA>^@unit[^\n\r]*/[\n\r]	       	{ agsave; tra_text=0; return TR_UNIT; }
<s_TRA>^@span[^\n\r]*/[\n\r]	       	{ agsave; tra_text=1; return TR_SPAN; }
<s_TRA>^"$"  				{ tra_text=0; s_next=s_TRA; BEGIN s_T; longinit; return TR_DOLLAR; }
<s_TRA>^"#"[^ \t\n\r]*/[ \t\n\r]   	{ tra_text=0; s_next=s_TRA; BEGIN s_T; longinit; return TR_COMMENT; }
<s_TRA>{TRA_LAB}			{ agsave; tra_text=1; return TR_LABEL; }
<s_TRA>{TRA_CHR}       			{ agsave; return TR_TEXT; }
<s_TRA>{TRA_CMD}	       		{ agsave; return TR_TEXT; }
<s_TRA>{TRA_PAR}			{ if (tra_text) { aglval.text=""; return TR_PAR; } }
<s_TRA>{TRA_WHT}			{ if (tra_text) { agsave; return TR_TEXT; } }
<s_TRA>{TRA_WRD}	       		{ if (tra_text) { agsave; return TR_TEXT; } }

<A>[ \t\r\n]+     			{ ; }

<*>\r				{ }

<A><<EOF>>	       		{ atf_yyterminate(); }

				/* GDLLEXER */

%%
static void
ag_user_action(void)
{
  if (agflextrace)
      fprintf(stderr, "%s:%d: aglex: text=::%s:: start=%d; tra_text=%d\n", curragfile, aglineno, agtext, YY_START, tra_text);

  aglloc.file = curragfile;
  aglloc.line = aglineno;
}

static void
ag_l_lex_init(const char *file, int line)
{
  curragfile = agfile = file;
  aglineno = line;
}

#if 0
static int
ag_line_save(const char *t)
{
  extern const char *curr_line_tok;
  curr_line_tok = (ccp)hpool_copy((uccp)t, ag_hpool);
  return 0;
}
#endif

#if 0
static int
ag_gdl_init(void)
{
  gdl_new_word(gdl_get_tree());
  return 0;
}
#endif

Tree *
ag_read_atf(char *atf, size_t len)
{
  YY_BUFFER_STATE ag_buffer;
  if (!(ag_buffer = ag_scan_buffer(atf, len)))
    {
      fprintf(stderr, "ag_read_atf: ag_scan_buffer failed\n");
      return NULL;
    }
  yy_start = s_next = 0;
  atf_init(); /* this really is atf_init--initialize the ATF structure for the build */
  Tree *tp = agyacc();
  ag_wrapup_buffer();
  return tp;
}

static Blocktok *last_bp = NULL;
static Blocktok *last_fragp = NULL;
void
ag_bp_reset(void)
{
  last_fragp = last_bp = NULL;
}

static int
ag_block(const char *scanned_at)
{
  const char *atcmd = scanned_at + 1;

  Blocktok *bp = blocktok(atcmd, strlen(atcmd));
  if (bp)
    {
      if (bp->bison == Y_TRANSLATION)
	BEGIN s_TRD;
      if (bp->bison == Y_FRAGMENT)
	{
          if (atfp->edoc == EDOC_COMPOSITE || atfp->edoc == EDOC_SCORE)
	     bp = blocktok("cfragment", 9); /* c[omposite]fragment */
          else if (last_bp)
	    {
	      if (last_bp->type == B_OBJECT)
	        bp = blocktok("objfrag", 7);
	      else if (last_bp->type == B_SURFACE)
	        bp = blocktok("srffrag", 7);
	      else if (last_bp->type == B_COLUMN)
	        bp = blocktok("colfrag", 7);
	      else
	        bp = blocktok("srffrag", 7);
	    }
	  else
            bp = blocktok("srffrag", 7);

	  if (last_fragp)
	    {
	      if (last_fragp->type != bp->type)
		  mesg_verr(&aglloc, "text mixes @fragment at different levels (last=%d!=curr=%d: missing @column?)", last_fragp->type, bp->type);
	      else
		{
		    /*mesg_verr(&aglloc, "@fragment level == %d", bp->type);*/
 	 	  bp = last_fragp;
		}
	    }
	  else
	      /*mesg_verr(&aglloc, "@fragment level == %d", bp->type)*/;
	  last_fragp = bp;
        }
      else
	last_bp = bp;
      aglval.b = bp;
      return bp->bison;
    }
  else
    {
      last_bp = NULL;
      fprintf(stderr, "atf: bad at-command @%s\n", atcmd);
      return Y_BAD;
    }
}

void
ag_wrapup_buffer(void)
{
  ag_delete_buffer(YY_CURRENT_BUFFER);
}

ssize_t
ag_get_lineno(void)
{
    return last_aglineno;
}

void
ag_set_lineno(ssize_t n)
{
    agset_lineno(n);
}

void
ag_set_file(const char *f)
{
    curratffile = f;
    last_aglineno = aglineno = 1;
    agset_lineno(1);
}

char *
ag_set_file_line(char *a, ssize_t *n)
{
  char *s = a;
  while (!isspace(*a)) /* we know this line is `##file ' */
    ++a;
  while (isspace(*a))
    ++a;
  curratffile = a;
  while (*a && '\n' != *a)
    ++a;
  if (*a)
    {
      *a++ = '\0';
      if (*a)
        {
          while (*a && !isspace(*a))
	    ++a;
          while (isspace(*a))
            ++a;
          aglineno = (int)strtoul(a, NULL, 10);
          while (*a && '\n' != *a)
	    ++a;
          ++a;
          *n = (*n - (a - s));
          return a;
       }
    }
  return NULL;
}
int
ag_gdl_lex_state(void)
{
  return YY_START;
}

void
ag_gdl_lex_closers(void)
{
  if (CIN)
      mesg_vwarning(currgdlfile,aglineno,"missing '|' in compound (discovered at end of line)");
  else if (QIN)
    mesg_vwarning(currgdlfile,aglineno,"missing ')' in qv (discovered at end of line)");
  GIN=CIN=QIN=0;
  cvq_reset();
}

void
ag_gdl_lex_init(const char *file, int line)
{
  CIN = GIN = QIN = 0;
  cvq_top = -1;
  ag_l_lex_init(file, line);
}

void
ag_gdl_decr_qin(void)
{
  if (--QIN < 0)
    { /* underflow condition */
      QIN = 0;
      BEGIN 0;
    }
  else
    {
      BEGIN cvq_pop();
    }
  if (agflextrace)
    fprintf(stderr, "ag_gdl_decr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

void
ag_gdl_incr_qin(void)
{
  ++QIN;
  BEGIN cvq_push(Q);
  if (agflextrace)
    fprintf(stderr, "ag_gdl_incr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

int
cvq_push(int s)
{
  if (++cvq_top < CVQ_MAX)
    {
      cvq_stack[cvq_top] = YY_START;
      return s;
    }
  else
    fprintf(stderr, "%s:%d: cvq_push: cvq stack overflow (CVQ_MAX=%d)\n", currgdlfile, aglineno, CVQ_MAX);
  return 0;
}

int
cvq_pop(void)
{
  if (cvq_top >= 0)
    return cvq_stack[cvq_top--];
  else
    fprintf(stderr, "%s:%d: cvq_pop: cvq stack underflow\n", currgdlfile, aglineno);
   return 0;
}

void
cvq_reset(void)
{
  cvq_top = -1;
  BEGIN 0;
}

/* Public function to call with (0) arg at start of text and with (1)
   arg when #atf: use legacy is encountered */
void
ag_gdl_legacy_lexer(int legacy)
{
  if (legacy)
    {
      ag_gdl_grapheme_type = ag_gdl_grapheme_type_l;
      gdl_delim = gdl_delim_l;
      gdl_graph_node = gdl_graph_node_l;
    }
  else
    {
      ag_gdl_grapheme_type = ag_gdl_grapheme_type_s;
      gdl_delim = gdl_delim_s;
      gdl_graph_node = gdl_graph_node_s;
    }
}

/* Version of ag_gdl_grapheme_type for use when atf mode is strict, i.e., non-legacy */
static int
ag_gdl_grapheme_type_s(void)
{
  char *g = agtext;
  gsaveg(g);

  if (strlen(g) == 1
      && ('n' == *g || 'N' == *g))
      return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
  else if (isdigit(*g))
    {
      char *d = g+1;
      while (*d && isdigit(*d))
        ++d;
      if (*d == '(')
        return NUMBER;
      else if (!strncmp(g, "00", 2))
	return ZERO;
      else
	return BARENUM;
    }
  else if (sll_is_signlist(g))
    return LISTNUM;
  else
    return GRAPHEME;
}

/* Version of ag_gdl_grapheme_type for use when atf: use legacy is set */
static int
ag_gdl_grapheme_type_l(void)
{
  char *l = agtext;
  char *g = (char*)gdl_unlegacy_str(&aglloc, (uccp)l);
  if (g)
    {
  gsaveg(g);

  if (strcmp(g,l))
    gdl_legacy_prop = (uccp)l;
  if (strlen(g) == 1
      && ('n' == *g || 'N' == *g))
      return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
  else if (isdigit(*g))
    {
      char *d = g+1;
      while (*d && isdigit(*d))
        ++d;
      if (*d == '(')
        return NUMBER;
      else if (!strncmp(g, "00", 2))
	return ZERO;
      else
	return BARENUM;
    }
  else if (sll_is_signlist(g))
    return LISTNUM;
  else
    return GRAPHEME;
    }
  else
    {
       return IGNORE;
    }
}
