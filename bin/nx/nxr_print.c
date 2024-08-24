#include <oraccsys.h>
#include "nx.h"

static void nxr_print_na(nx_restok *rtp, FILE *fp);
static void nxr_print_no(nx_restok *rtp, FILE *fp);
static void nxr_print_nu(nx_restok *rtp, FILE *fp);

void
nxr_print(nx_result *r, FILE *fp)
{
  int i;
  for (i = 0; i < r->nr; ++i)
    {
      if (r->r[i].type == NX_NU)
	nxr_print_nu(&r->r[i], fp);
      else if (r->r[i].type == NX_NO)
	nxr_print_no(&r->r[i], fp);
      else
	nxr_print_na(&r->r[i], fp);
    }
  fprintf(fp, "\n");
}

static void
nxr_print_na(nx_restok *rtp, FILE *fp)
{
  fprintf(fp, " ![%s=%s]", rtp->nb.tok, nxt_str[rtp->type]);
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
      if (rtp->nu[i]->sys)
	{
	  fprintf(fp, " %s[", rtp->nu[i]->sys->name);
	  nx_step *sp;
	  int i = 0;
	  for (sp = rtp->nu[i]->steps; sp; sp = sp->next)
	    {
	      if (i++)
		fprintf(fp, ".");
	      if (sp->type == NX_STEP_TOK)
		fprintf(fp, "%s", sp->tok.tok);
	    }
	  fprintf(fp, "]");
	}
    }
}
