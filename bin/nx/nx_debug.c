#include <oraccsys.h>
#include "nx.h"

void nxd_show_aevs(nx_number *np);
void nxd_show_step(nx_step *ns);

void
nxd_show_num(nx_num *nump)
{
  if (nump->d == 1)
    printf("%llu", nump->n);
  else
    printf("%llu/%d",nump->n, nump->d);
}

void
nxd_show_sum(nx_num *sump)
{
  printf("==>> ");
  nxd_show_num(sump);
  printf("\n");
}

void
nxd_show_aevs(nx_number *np)
{
  nx_step *nxs;
  printf("%s => ", np->sys->name);
  for (nxs = np->steps; nxs; nxs = nxs->next)
    {
      nxd_show_step(nxs);
      if (nxs->aev)
	{
	  printf("=");
	  nxd_show_num(nxs->aev);
	}
      printf(" ");
    }
  printf("\n");
}

void
nxd_show_Snum(nx_number *np)
{
  printf("<%s:", np->sys->name);
  nx_step *nxs;
  int i = 0;
  for (nxs = np->steps; nxs; nxs = nxs->next)
    {
      if (i++)
	printf(".");
      printf("%s", nxs->tok.tok);
    }
  printf(">");
}

void
nxd_show_step(nx_step *nxs)
{
  if (nxs->type == NX_STEP_TOK)
    {
      if (nxs->tok.inst)
	{
	  printf("[");
	  nxd_show_num(&nxs->tok.inst->count);
	  printf("*%s]",nxs->tok.inst->step->unit);
	}
      else
	printf("%s", nxs->tok.tok);
    }
  else
    {
      printf("[");
      nxd_show_Snum(nxs->num);
      printf("=");
      nxd_show_num(&nxs->num->aev);
      printf("*%s]", nxs->num->unit->tok.inst->step->unit);
    }
}

void
nxd_show_inst(const uchar *tok, ns_inst *ip)
{
  if (tok)
    printf("tok %s from hash gives inst: ", tok);
  printf("%s => %llu/%d %s ", ip->text, ip->count.n, ip->count.d, ip->unit);
  if (ip->a_or_d)
    printf("%c", ip->a_or_d);
  if (ip->step)
    printf("; step: %s = %llu/%d %s", ip->step->sys->name,
	   ip->step->mult.n, ip->step->mult.d, ip->step->unit);
  printf("\n");
}

void
nxd_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to)
{
  int f = from;
  printf("nxp:\ntoks:");
  while (from <= to)
    printf("\t%s", toks[from++]);
  printf("\ntypes:");
  from = f;
  while (from <= to)
    printf("\t%s", nxt_str[nptoks[from++]]);
  printf("\n\n");
}

