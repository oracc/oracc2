%option prefix="atfl"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>

extern const char *project;

static void output(void);
extern const char *curratffile;

char *curr_pqx = NULL;
int type;
#define LINK 1
#define RELN 2

%}

BOM3	"\xef\xbb\xbf"
BOM2	"\xfe\xff"
PQX	[PQX][0-9]{6}
LINK	"#link:"
RELN	"<<"|">>"|"||"|"++"

%s AMP DROP GRAB

%%

<AMP>{PQX}		{ BEGIN(DROP); if (curr_pqx) free(curr_pqx); curr_pqx = strdup(atfltext); }

<DROP>.*/\n		{ BEGIN(0); }

<GRAB>.*/\n		{ BEGIN(0); output(); }

^{BOM3}"&"		{ BEGIN(AMP); }

^{BOM2}"&"      	{ BEGIN(AMP); }

^"&"	      		{ BEGIN(AMP); }

^{LINK}			{ BEGIN(GRAB); type=LINK; }

^{RELN}			{ BEGIN(GRAB); type=RELN; }

\n			{ }

.			{ }

<<EOF>>			{ yyterminate(); }

%%

static void
output(void)
{
  char *p = atfltext; while (isspace(*p)) ++p;
  if (type==RELN)
    {
      p += 2;
      char *q = p;
      while (isspace(*q))
        ++q;
      if (q > p)
        {
          while (*q)
            *p++ = *q++;
          *p = '\0';
        } 
    }
  printf("%s:%s\t%s\n", project, curr_pqx, p);
}
