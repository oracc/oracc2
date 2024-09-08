%option prefix="atfd"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>

extern const char *catproj;
extern const char *project;

static void lem(void);
static void pqx(void);
static void tr(void);
static void pqx_term(void);
const char *curratffile = "<file>";

const char *curr_pqx;
int curr_lem = 0;
Hash *hdata = NULL;
Pool *p = NULL;

%}

BOM3	"\xef\xbb\xbf"
BOM2	"\xfe\xff"
PQX	[PQX][0-9]{6}
TRA	"@translation"[ \t]+[^ \t]+[ \t]+

%s AMP ATR DROP ITR

%%

<AMP>{PQX}		{ BEGIN(DROP); pqx(); }

<ATR>[^ \t]+		{ BEGIN(DROP); tr(); }

<DROP>.*/\n		{ BEGIN(0); }

<ITR>[-a-z0-9]+/:	{ BEGIN(DROP); tr(); }

^{BOM3}"&"		{ BEGIN(AMP); }

^{BOM2}"&"      	{ BEGIN(AMP); }

^"&"	      		{ BEGIN(AMP); }

^{TRA}			{ BEGIN(ATR); }

^"#tr."  		{ BEGIN(ITR); }

^"#tr:"  		{ atfdtext="en"; tr(); }

^"#lem:"  		{ BEGIN(DROP); lem(); }

\n			{ }

.			{ }

<<EOF>>			{ pqx_term(); yyterminate(); }

%%

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
  curr_lem = 0;
}

static void
pqx_term(void)
{
  const char *trs;
  printf("%s:%s@%s\t%s\t%s\n", project, curr_pqx, catproj, curr_lem ? "#lem" : "", (trs = pqx_trs()));
  hash_free(hdata, NULL);
  pool_term(p);
  if (*trs)
    free((void*)trs);
}

static void
lem(void)
{
  ++curr_lem;
}

static void
pqx(void)
{
  if (curr_pqx)
    pqx_term();
  pqx_init(atfdtext);
}

static void
tr(void)
{
  if (!hash_find(hdata, (uccp)atfdtext))
    hash_add(hdata, (uccp)pool_copy((uccp)atfdtext,p), "");
}
