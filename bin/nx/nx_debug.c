#include <oraccsys.h>
#include "nx.h"

int debug = 0;

const char *nxd_arg = "";

void nxd_show_aevs(nx_number *np);
void nxd_show_step(nx_step *ns);

int
nxd_arg_ok(const char *arg)
{
  const char *nxd_arg_ok = "LYTPVM";
  while (*arg)
    if (!strchr(nxd_arg_ok, *arg))
      return 0;
    else
      ++arg;
  return 1;
}

void
nxd_show_num(nx_num *nump)
{
  if (nump->d == 1)
    fprintf(stderr,"%llu", nump->n);
  else
    fprintf(stderr,"%llu/%d",nump->n, nump->d);
}

void
nxd_show_sum(nx_num *sump)
{
  fprintf(stderr,"==>> ");
  nxd_show_num(sump);
  fprintf(stderr,"\n");
}

void
nxd_show_nxnu(nx_number *np)
{
  nx_step *nxs;
  fprintf(stderr,"%s => ", np->sys->name);
  for (nxs = np->steps; nxs; nxs = nxs->next)
    {
      nxd_show_step(nxs);
      fprintf(stderr," ");
    }
  fprintf(stderr,"\n");
}

void
nxd_show_aevs(nx_number *np)
{
  nx_step *nxs;
  fprintf(stderr,"%s => ", np->sys->name);
  for (nxs = np->steps; nxs; nxs = nxs->next)
    {
      nxd_show_step(nxs);
      if (nxs->aev)
	{
	  fprintf(stderr,"=");
	  nxd_show_num(nxs->aev);
	}
      fprintf(stderr," ");
    }
  fprintf(stderr,"\n");
}

void
nxd_show_Snum(nx_number *np)
{
  fprintf(stderr,"<%s:", np->sys->name);
  nx_step *nxs;
  int i = 0;
  for (nxs = np->steps; nxs; nxs = nxs->next)
    {
      if (i++)
	printf(".");
      fprintf(stderr,"%s", nxs->tok.tok);
    }
  fprintf(stderr,">");
}

void
nxd_show_step(nx_step *nxs)
{
  if (nxs->type == NX_STEP_TOK)
    {
      if (nxs->tok.inst)
	{
	  fprintf(stderr,"[");
	  nxd_show_num(&nxs->tok.inst->count);
	  fprintf(stderr,"*%s]",nxs->tok.inst->step->unit);
	}
      else
	printf("%s", nxs->tok.tok);
    }
  else
    {
      fprintf(stderr,"[");
      nxd_show_Snum(nxs->num);
      fprintf(stderr,"=");
      nxd_show_num(&nxs->num->aev);
      fprintf(stderr,"*%s]", nxs->num->unit->tok.inst->step->unit);
    }
}

void
nxd_show_inst(const uchar *tok, ns_inst *ip)
{
  if (tok)
    fprintf(stderr,"tok %s from hash gives inst: ", tok);
  fprintf(stderr,"%s => %llu/%d %s ", ip->text, ip->count.n, ip->count.d, ip->unit);
  if (ip->a_or_d)
    fprintf(stderr,"%c", ip->a_or_d);
  if (ip->step)
    fprintf(stderr,"; step: %s = %llu/%d %s", ip->step->sys->name,
	   ip->step->mult.n, ip->step->mult.d, ip->step->unit);
  fprintf(stderr,"\n");
}

void
nxd_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to)
{
  int f = from;
  fprintf(stderr,"nxp:\ntoks:");
  while (from <= to)
    fprintf(stderr,"\t%s", toks[from++]);
  fprintf(stderr,"\ntypes:");
  from = f;
  while (from <= to)
    fprintf(stderr,"\t%s", nxt_str[nptoks[from++]]);
  fprintf(stderr,"\n\n");
}

static int
ne_cmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  struct nxt_tab *ne_a = ne(cc1, strlen(cc1));
  struct nxt_tab *ne_b = ne(cc2, strlen(cc2));
  if (ne_a->tok < ne_b->tok)
    return -1;
  else if (ne_a->tok > ne_b->tok)
    return 1;
  else
    return 0;    
}

void
nxd_show_env(Hash *e)
{
  int nkey;
  const char **k = hash_keys2(e, &nkey);
  qsort(k, nkey, sizeof(const char *), ne_cmp);
  int i;
  for (i = 0; k[i]; ++i)
    {
      const char *v = hash_find(e, (ucp)k[i]);
      if (strpbrk(v, " \t\n"))
	printf("$%s=\"%s\"\n", k[i], v);
      else
	printf("$%s=%s\n", k[i], v);
    }
}
