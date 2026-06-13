%option prefix="atfi"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include <hash.h>
#include "atfdata.h"

int split_dups = 0;
int splitting = 0;
static void pqx(void);
extern const char *curratffile;

extern Hash *hseen;
extern Pool *pseen;

%}

BOM3	"\xef\xbb\xbf"
BOM2	"\xfe\xff"
PQX	"&"[PQX][0-9]{6}

%%

^({BOM2}|{BOM3})?{PQX}.*\n { pqx(); }

^[^\n&\xef\xfe].*\n  	   { if (outfp) fputs(yytext, outfp); }

^\n			   { if (outfp) fputc('\n', outfp); }

<<EOF>>			   { yyterminate(); }

%%

static void
pqx(void)
{
  static char pqx[8];
  char *s = strchr(yytext, '&') + 1;
  strncpy(pqx, s, 7);
  pqx[7] = '\0';
  /*fprintf(stderr, "pqx: %s\n", pqx);*/
  char *t = s;
  while (*t && !isspace(*t))
    ++t;
  if (hash_find(hseen, (uccp)pqx))
    {
      fprintf(stderr, "dropping PQX %s\n", pqx);
      if (split_dups && !splitting)
        {
          splitting = 1;
	  outfp = splitfp;
	}
    }
  else
    {
      hash_add(hseen, pool_copy((uccp)pqx, pseen), (void*)(uintptr_t)1);
      if (split_dups && splitting)
	  {
	    splitting = 0;
	    outfp = keepfp;
	  }
    }
  /* Now the correct output is set if one is needed */
  if (fix_dups_mode)
    atf_fix_dup(pqx);
  if (outfp)
    fprintf(outfp, "&%s%s", pqx, t);
}

FILE *
atfi_input(const char *fn)
{
  if (fn && strcmp(fn, "-"))
    {
      FILE *fp = xfopen(fn, "r");
      if (fp)
        {
          curr_file = fn;
          yyin = fp;
          yylineno = 1;
        }
      return fp;
    }
  return stdin;
}

FILE *
atfi_output(const char *fn, const char *odir)
{
  if (fn && strcmp(fn, "-"))
    {
      char *outfn = malloc(strlen(fn)+strlen(odir)+2);
      char *s = strrchr(fn, '/');
      if (s)
	  ++s;
      else
	  s = (char*)fn;
      sprintf(outfn, "%s/%s", odir, s);
      FILE *fp;
      if (!(fp = xfopen(outfn, "w")))
	  free(outfn);
      return outfp = fp;
    }
  else
    return outfp = stdout;
}

FILE *
atfi_split(const char *fn, const char *odir)
{
  char *outfn = malloc(strlen(fn)+strlen(odir)+2);
  char *s = strrchr(fn, '/');
  if (s)
    ++s;
  else
    s = (char*)fn;
  sprintf(outfn, "%s/%s", odir, s);
  FILE *fp;
  if (!(fp = xfopen(outfn, "w")))
    free(outfn);
  return fp;
}
