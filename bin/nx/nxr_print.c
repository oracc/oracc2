#include <oraccsys.h>
#include "nx.h"

static void nxr_print_na(nx_restok *rtp, FILE *fp);
static void nxr_print_no(nx_restok *rtp, FILE *fp);
static void nxr_print_nu(nx_restok *rtp, FILE *fp);
static void nxr_print_nu_sig(nx_number *np, FILE *fp);
static List *nxr_nu_data(nx_number *np, List *lp);

#if 0
static void nxr_print_nu_sub(nx_number *np, FILE *fp);
#endif

void
nxr_print(nx_result *r, FILE *fp, int nonewline)
{
  int i;
  if (nxp->env && nxp->env->key_count)
    nxd_show_env(nxp->env);
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
#if 0
      nxr_print_nu_sub(rtp->nu[i], fp);
      fputs("==", fp);
#endif
      nxr_print_nu_sig(rtp->nu[i], fp);
    }
  if (nxp->data_is_char)
    {
      List *nu_data = nxr_nu_data(rtp->nu[0], list_create(LIST_SINGLE));
      uchar *d;
      fputc('\t', fp);
      int i = 0;
      for (d = list_first(nu_data); d; d = list_next(nu_data))
	{
	  if (i++)
	    fputc('+', fp);
	  fputs((ccp)d, fp);
	}
      list_free(nu_data, NULL);
    }
}

static List *
nxr_nu_data(nx_number *np, List *lp)
{
  if (np->sys)
    {
      nx_step *sp;
      for (sp = np->steps; sp; sp = sp->next)
	{
	  if (sp->type == NX_STEP_TOK)
	    {
	      if (sp->tok.data)
		list_add(lp, (void*)sp->tok.data);
	    }
	  else
	    (void)nxr_nu_data(sp->num, lp);
	}
    }
  return lp;
}

#if 0
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
      if (np->me_str)
	fprintf(fp, "==%s", np->me_str);
    }
}
#endif

static void
nxr_print_nu_sig(nx_number *np, FILE *fp)
{
  if (np->sys)
    {
      char o = '\t';
      char c = '\t';
#if 0
      int n = np->sys->name[1];
      if (strchr("SB", n))
	{
	  o = '<';
	  c = '>';
	}
#endif
      fprintf(fp, "%s%c", np->sys->name, o);
      nx_step *sp;
      int i = 0;
      for (sp = np->steps; sp; sp = sp->next)
	{
	  if (sp->type == NX_STEP_TOK)
	    {
	      if ('{' != *sp->tok.tok)
		{
		  if (i++)
		    fprintf(fp, "+");
		  fprintf(fp, "%s", sp->tok.tok);
		}
	    }
	  else
	    {
	      if (i++)
		fprintf(fp, "+");
	      if (!sp->num->me_str)
		nx_values_np(sp->num);
	      if (sp->num->me_str)
		fprintf(fp, "%s", sp->num->me_str);
	      else
		fprintf(fp, "N");
	      if (sp->num->unit)
		fprintf(fp, "*%s", sp->num->unit->tok.tok);
	      
	      /*nxr_print_nu_sig(sp->num, fp);*/
	    }
	}
      fprintf(fp, "%c", c);
      if (np->unit)
	fprintf(fp, "+%s", np->unit->tok.tok);
      if (np->me_str)
	fprintf(fp, "%s", np->me_str);
    }
}

