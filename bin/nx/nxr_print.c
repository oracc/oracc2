#include <oraccsys.h>
#include "nx.h"

static void nxr_print_na(nx_restok *rtp, FILE *fp);
static void nxr_print_no(nx_restok *rtp, FILE *fp);
static void nxr_print_nu(nx_restok *rtp, FILE *fp);
static void nxr_print_nu_sig(nx_number *np, FILE *fp);
static void nxr_header(FILE *fp);
void nxr_print_num(ns_sys *ns, nx_num *nump, FILE *fp);

static const char *fields[] =
  {
   "res", "sys", "sig", "aev", "mev", "wid", "det", "dwid", "com", "cwid", "ass", "awid",
   NULL,
};

void
nxr_print(nx_result *r, FILE *fp, int nonewline)
{
  int i;
  if (nxp->env && nxp->env->key_count)
    nxd_show_env(nxp->env);

  if (!nxp->printed_header)
    nxr_header(fp);
  
  for (i = 0; i < r->nr; ++i)
    {
      if (i)
	fputc('\n', fp);
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
nxr_header(FILE *fp)
{
  ++nxp->printed_header;
  int i = 0;
  while (fields[i])
    {
      if (i)
	fputc('\t', fp);
      fputs(fields[i++], fp);
    }
  fputc('\n', fp);
}

static void
nxr_print_na(nx_restok *rtp, FILE *fp)
{
  fprintf(fp, "#:: ![%s=%s]", rtp->nb.tok, nxt_str[rtp->nb.type]);
}

static void
nxr_print_no(nx_restok *rtp, FILE *fp)
{
  fprintf(fp, "#:: !%s", rtp->no.tok);
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
      else
	fputc('-',fp);
      list_free(nx_data, NULL);
    }
  else
    fputc('-',fp);
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
  if (!i)
    fputc('-', fp);
}

static void
nxr_step_data(nx_step *sp, FILE *fp)
{
  if (!sp || !sp->tok.data)
    {
      fputc('-',fp);
      return;
    }
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
      /* newline between possible answers */
      if (i)
	fputc('\n', fp);

      /* Col 0: index in this cand set */
      fprintf(fp, "%d\t", i);
      
      /* Col 1..4: SYS\tSIG\tAEV\tMEV */
      nxr_print_nu_sig(rtp->nu[i], fp);

      /* Col 5: WID if data_is_char; else empty */
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data_via_list(rtp->nu[i]->steps, fp);

      /* Col 6,7: DET\tWID */
      fputc('\t', fp);
      nxr_step_toks(rtp->nu[i]->det, fp);
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data(rtp->nu[i]->det, fp);

      /* Col 8,9: COM\tWID */
      fputc('\t', fp);
      nxr_step_toks(rtp->nu[i]->com, fp);
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data(rtp->nu[i]->com, fp);

      /* Col 10,11: ASS\tWID */
      fputc('\t', fp);
      nxr_step_toks(rtp->nu[i]->ass, fp);
      fputc('\t', fp);
      if (nxp->data_is_char)
	nxr_step_data(rtp->nu[i]->ass, fp);
    }
}

static void
nxr_print_nu_sig(nx_number *np, FILE *fp)
{
  if (np->sys)
    {
      fprintf(fp, "%s\t", np->sys->name);
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

      fputc('\t', fp);
      if (np->ae_str)
	fputs((ccp)np->ae_str, fp);

      fputc('\t', fp);
      if (np->me_str)
	fputs((ccp)np->me_str, fp);
    }
}

uchar *
nxr_format_aev(ns_sys *ns, nx_num *nump)
{
  char *base = NULL;
  if (ns)
    {
      if (ns->conv[strlen((ccp)ns->conv)-1] != 'u')
	base = (char*)ns->base;
    }
  char unit[base ? (strlen(base)+2) : 1];
  if (base)
    sprintf(unit, " %s", base);
  else
    unit[0] = '\0';

  char buf[1024];
  int len = 0;
  
  if (nump->d == 1)
    len = snprintf(buf, 1024, "%llu%s", nump->n, unit);
  else
    len = snprintf(buf, 1024, "%llu/%d%s", nump->n, nump->d, unit);

  if (len == 1024)
    fprintf(stderr, "nxr_format_aev: buffer overflow in %s\n", buf);
  
  return pool_copy((ucp)buf, nxp->p);
}
