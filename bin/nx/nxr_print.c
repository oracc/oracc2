#include <oraccsys.h>
#include "nx.h"

static void nxr_print_na(nx_restok *rtp, FILE *fp);
static void nxr_print_no(nx_restok *rtp, FILE *fp);
static void nxr_print_nu(nx_restok *rtp, FILE *fp);
static void nxr_print_nu_sig(nx_number *np, FILE *fp);

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

static List *
nxr_get_data(nx_step *sp, List *lp)
{
  for ( ; sp; sp = sp->next)
    {
      if (sp->type == NX_STEP_TOK)
	{
	  if (sp->tok.data)
	    list_add(lp, (void*)sp->tok.data);
	}
      else
	(void)nxr_get_data(sp->num->steps, lp);
    }
  return lp;
}

static void
nxr_step_data_via_list(nx_step *sp, FILE *fp)
{
  if (sp)
    {
      List *nx_data = nxr_get_data(sp, list_create(LIST_SINGLE));
      if (list_len(nx_data))
	{
	  uchar *d;
	  int i = 0;
	  for (d = list_first(nx_data); d; d = list_next(nx_data))
	    {
	      if (i++)
		fputc('+', fp);
	      fputs((ccp)d, fp);
	    }
	}
      list_free(nx_data, NULL);
    }
}

static void
nxr_step_toks(nx_step *sp, FILE *fp)
{
  int i = 0;
  while (sp)
    {
      if (i++)
	fputc(' ', fp);
      fputs((ccp)sp->tok.tok, fp);
      sp = sp->next;
    }
}

static void
nxr_step_data(nx_step *sp, FILE *fp)
{
  if (!sp || !sp->tok.data)
    return;
  int i = 0;
  while (sp)
    {
      if (i++)
	fputc('+', fp);
      fputs(sp->tok.data, fp);
      sp = sp->next;
    }
}

static void
nxr_print_nu(nx_restok *rtp, FILE *fp)
{
  int i;
  for (i = 0; rtp->nu[i]; ++i)
    {
      /* Col 0: index in this cand set */
      fprintf(fp, "%d\t", i);
      /* Col 1,2,3: SYS\tSIG\tMEV */
      nxr_print_nu_sig(rtp->nu[i], fp);

      /* Col 4: WID if data_is_char; else empty */
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data_via_list(rtp->nu[i]->steps, fp);

      /* Col 5,6: DET\tWID */
      fputc('\t', fp);
      nxr_step_toks(rtp->nu[i]->det, fp);
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data(rtp->nu[i]->det, fp);

      /* Col 7,8: COM\tWID */
      fputc('\t', fp);
      nxr_step_toks(rtp->nu[i]->com, fp);
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data(rtp->nu[i]->com, fp);

      /* Col 9,10: ASS\tWID */
      fputc('\t', fp);
      nxr_step_toks(rtp->nu[i]->ass, fp);
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data(rtp->nu[i]->ass, fp);
    }
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

