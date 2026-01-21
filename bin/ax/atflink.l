%option prefix="atfl"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>

extern const char *project;

static void stash(void);
const char *curratffile = "<file>";

const char *curr_pqx = NULL;
int curr_lem = 0;
int x_mode = 0;
Hash *hdata = NULL;
Pool *p = NULL;
int type;
#define LINK 1
#define RELN 2

%}

BOM3	"\xef\xbb\xbf"
BOM2	"\xfe\xff"
PQX	[PQX][0-9]{6}
LINK	"#link:"
RELN	"<<"|">>"|"||"|"++"

%s AMP

%%

<AMP>{PQX}		{ BEGIN(DROP); if (curr_pqx) free(currpqx); curr_pqx = strdup(atfltext); }

<DROP>.*/\n		{ BEGIN(0); }

<GRAB>.*/\n		{ BEGIN(0); stash(atfltext); }

^{BOM3}"&"		{ BEGIN(AMP); }

^{BOM2}"&"      	{ BEGIN(AMP); }

^"&"	      		{ BEGIN(AMP); }

^{LINK}			{ BEGIN(GRAB); type=LINK; }

^{RELN}			{ BEGIN(GRAB); type=RELN; }

\n			{ }

.			{ }

<<EOF>>			{ yyterminate(); }

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
pqx(void)
{
  if (curr_pqx && !x_mode)
    pqx_term();
  pqx_init(atfltext);
}

static void
tr(void)
{
  if (!hash_find(hdata, (uccp)atfltext))
    hash_add(hdata, (uccp)pool_copy((uccp)atfltext,p), "");
}

static void
x_term(void)
{
  char *p = atfltext; while (isspace(*p)) ++p;
  char *e = p + strlen(p); while (e > p && isspace(e[-1])) --e;
  if (e == p)
    p = "Unnamed";
  else if ('=' == *p)
    {
      ++p; while (isspace(*p)) ++p;
      *e = '\0';
    }
  printf("%s\t%s\tunknown\tunknown\n", curr_pqx, p);
}
