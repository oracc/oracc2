#include <oraccsys.h>
#include "nx.h"

void
nx_exec_lists(List *t, List *d)
{
  int tlen = 0, dlen = 0;
  nxp->toks = (const uchar **)list2array_c(t, &tlen);
  nxp->data = (const uchar **)list2array_c(d, &dlen);
  if (dlen && tlen != dlen)
    fprintf(stderr, "nx_exec_lists: tlen %d != dlen %d\n", tlen, dlen);
  else
    nx_exec(nxp->toks, nxp->data, tlen);
}

void
nx_exec(const uchar **toks, const uchar **data, int ntoks)
{
  nx_result *r = nx_parse(toks, (const void**)data, ntoks);
  nx_values(r);
  nxr_print(r, stdout, 0);
  mesg_print(stderr);
}