#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"

void
bx_icf_pre(Bx *bp)
{
}

void
bx_icf_out(Bx *bp)
{
}

void
bx_icf_run(Bx *bp)
{
}

static void
bx_icf_cite(Bx *bp, const char **atts)
{
}

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
void
printText(const char *s, FILE *frag_fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",frag_fp);
      else if (*s == '>')
	fputs("&gt;",frag_fp);
      else if (*s == '&')
	fputs("&amp;",frag_fp);
#if 0
      else if (*s == '\'')
	fputs("&apos;",frag_fp);
#endif
      else if (*s == '"')
	fputs("&quot;",frag_fp);
      else
	fputc(*s,frag_fp);
      ++s;
    }
}

void
printStart(FILE *fp, const char *name, const char **atts)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  fprintf(fp, " %s=\"",*ap++);
	  printText(*ap++, fp);
	  fputc('"', fp);
	}
    }
  fputc('>', fp);
}

void
printEnd(FILE *fp, const char *name)
{
  printText((const char *)charData_retrieve(), fp);
  fprintf(fp, "</%s>", name);
}

void
icf_sH(void *userData, const char *name, const char **atts)
{
  printStart(userData, name, atts);
  
  if (!strcmp(name, "b:cite"))
    bx_icf_cite(userData, atts);
}

void
icf_eH(void *userData, const char *name)
{
  printEnd(userData, name);
}

void
bx_run_icf(Bx *bp, const char *fname)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, icf_sH, icf_eH, NULL, bp);
}

