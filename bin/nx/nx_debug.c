#include <oraccsys.h>
#include "nx.h"

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

