#include <oraccsys.h>
#include "nx.h"

void
nxr_testdata(FILE *fp, nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to)
{
  int i;
  for (i = from; i < to; ++i)
    {
      if (i)
	fputc(' ', fp);
      fputs((ccp)toks[i], fp);
    }
  fputc('\t', fp);
  for (i = from; i < to; ++i)
    {
      if (i)
	fputc(' ', fp);
      fputs(nxt_str[nptoks[i]], fp);
    }
  fputc('\t', fp);
  nxr_print(r, fp, 1);
  fputc('\n', fp);  
}
