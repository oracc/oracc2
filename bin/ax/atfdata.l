%option prefix="atfd"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include <hash.h>
#include "atfdata.h"
#include "atf_cdli.h"

extern const char *catproj;
extern const char *project;

static void lem(void);
static void pqx(void);
static void tr(void);
static void pqx_term(void);
static void x_term(void);
int curr_lem = 0;
int dups = 0;
int x_mode = 0;
Hash *hseen = NULL;
Pool *pseen = NULL;
Hash *hdata = NULL;
Pool *p = NULL;

%}

BOM3	"\xef\xbb\xbf"
BOM2	"\xfe\xff"
PQX	[PQX][0-9]{6}
TRA	"@translation"[ \t]+[^ \t]+[ \t]+

%s AMP ATR DROP ITR X

%%

<AMP>{PQX}		{ if (x_mode&&'X'==*atfdtext) { BEGIN(X); } else { BEGIN(DROP); } pqx(); }

<ATR>[^ \t]+		{ BEGIN(DROP); if (!x_mode) tr(); }

<X>.*/\n		{ BEGIN(0); x_term(); }

<DROP>.*/\n		{ BEGIN(0); }

<ITR>[-a-z0-9]+/:	{ BEGIN(DROP); if (!x_mode) tr(); }

^{BOM3}"&"		{ BEGIN(AMP); }

^{BOM2}"&"      	{ BEGIN(AMP); }

^"&"	      		{ BEGIN(AMP); }

^{TRA}			{ BEGIN(ATR); }

^"#tr."  		{ BEGIN(ITR); }

^"#tr:"  		{ atfdtext="en"; if (!x_mode) tr(); }

^"#lem:"  		{ BEGIN(DROP); if (!x_mode) lem(); }

\n			{ }

.			{ }

<<EOF>>			{ if (!x_mode) pqx_term(); yyterminate(); }

%%

FILE *
atfd_input(const char *fn)
{
  if (fn)
    {
      if (!strcmp(fn, "-"))
	return stdin;
      else
        {
          FILE *fp = xfopen(fn, "r");
          if (fp)
            {
              curr_file = fn;
	      yyin = fp;
	      yylineno = 1;
	      yyrestart(fp);
	      BEGIN(0);
	    }
          return fp;
	}
    }
  return NULL;
}

static const char *
pqx_trs(void)
{
  if (hdata->key_count)
    {
      int nptrs;
      const char **trptrs = hash_keys2(hdata, &nptrs);
      return vec_to_str((char**)trptrs, nptrs, " ");
    }
  else
    return "";
}

static void
pqx_init(const char *pqx)
{
  hdata = hash_create(5);
  p = pool_init();
  curr_pqx = (ccp)pool_copy((uccp)pqx, p);
  curr_line = yylineno;
#if 0
  if (!x_mode)
    fprintf(stderr, "pqx_init: %s\n", curr_pqx);
#endif
  curr_lem = 0;
}

static void
pqx_term(void)
{
    /*fprintf(stderr, "pqx_term: %s\n", curr_pqx);*/
  const char *trs = NULL;
  if (hash_find(hseen, (uccp)curr_pqx))
    fprintf(stderr, "%s:%d: skipping duplicate PQX %s\n", curr_file, curr_line, curr_pqx);
  else
    {
      if (!dups && !dry)
        printf("%s:%s@%s\t%s\t%s\n",
	       project, curr_pqx, catproj, curr_lem ? "#lem" : "", (trs = pqx_trs()));
      if ('P' == *curr_pqx)
        atf_cdli_check(curr_pqx);
      hash_add(hseen, pool_copy((uccp)curr_pqx,pseen), (void*)(uintptr_t)1);
    }
  curr_pqx = NULL;
  if (hdata)
    {
      hash_free(hdata, NULL);
      hdata = NULL;
      pool_term(p);
      p = NULL;
    }
  if (trs && *trs)
    {
      free((void*)trs);
      trs = NULL;
    }
}

static void
lem(void)
{
  ++curr_lem;
}

static void
pqx(void)
{
  if (curr_pqx && !x_mode)
    pqx_term();
  pqx_init(atfdtext);
}

static void
tr(void)
{
  if (!hash_find(hdata, (uccp)atfdtext))
    hash_add(hdata, (uccp)pool_copy((uccp)atfdtext,p), "");
}

static void
x_term(void)
{
  char *p = atfdtext; while (isspace(*p)) ++p;
  char *e = p + strlen(p); while (e > p && isspace(e[-1])) --e;
  if (e == p)
    p = "Unnamed";
  else if ('=' == *p)
    {
      ++p; while (isspace(*p)) ++p;
      *e = '\0';
    }
  if (hash_find(hseen, (uccp)curr_pqx))
    fprintf(stderr, "atfdata: xcat: skipping duplicate %s\n", curr_pqx);
  else
    {
      if (!dry)
        printf("%s\t%s\tunknown\tunknown\n", curr_pqx, p);
      hash_add(hseen, (uccp)curr_pqx, "");
    }
}
