#include <oraccsys.h>
#include "nx.h"

static void nxr_print_na(nx_restok *rtp, FILE *fp);
static void nxr_print_no(nx_restok *rtp, FILE *fp);
static void nxr_print_nu(nx_restok *rtp, FILE *fp);
static void nxr_print_nu_sub(nx_number *np, FILE *fp);

void
nxr_print(nx_result *r, FILE *fp, int nonewline)
{
  int i;
  for (i = 0; i < r->nr; ++i)
    {
      if (i)
	fprintf(fp, " :: ");
      if (r->r[i].type == NX_NU)
	nxr_print_nu(&r->r[i], fp);
      else if (r->r[i].type == NX_NO)
	nxr_print_no(&r->r[i], fp);
      else
	nxr_print_na(&r->r[i], fp);
    }
  if (!nonewline)
    fprintf(fp, "\n");
}

static void
nxr_print_na(nx_restok *rtp, FILE *fp)
{
  fprintf(fp, " ![%s=%s]", rtp->nb.tok, nxt_str[rtp->nb.type]);
}

static void
nxr_print_no(nx_restok *rtp, FILE *fp)
{
  fprintf(fp, " !%s", rtp->no.tok);
}

static void
nxr_print_nu(nx_restok *rtp, FILE *fp)
{
  int i;
  for (i = 0; rtp->nu[i]; ++i)
    {
      if (i)
	fputc(';', fp);
      nxr_print_nu_sub(rtp->nu[i], fp);
    }
}

static void
nxr_print_nu_sub(nx_number *np, FILE *fp)
{
  if (np->sys)
    {
      int n = np->sys->name[1];
      char o = '[';
      char c = ']';
      if (strchr("SB", n))
	{
	  o = '<';
	  c = '>';
	}
      fprintf(fp, "%s%c", np->sys->name, o);
      nx_step *sp;
      int i = 0;
      for (sp = np->steps; sp; sp = sp->next)
	{
	  if (i++)
	    fprintf(fp, ".");
	  if (sp->type == NX_STEP_TOK)
	    fprintf(fp, "%s", sp->tok.tok);
	  else
	    nxr_print_nu_sub(sp->num, fp);
	}
      fprintf(fp, "%c", c);
      if (np->unit)
	fprintf(fp, "+%s", np->unit->tok.tok);
    }
}

