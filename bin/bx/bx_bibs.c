#include <oraccsys.h>
#include "bx.h"

const char **
bx_bibs_file(const char *bibf)
{
  const char **bfs = NULL;
  static unsigned char *fmem;
  if (bibf)
    {
      bfs = (const char **)loadfile_lines3((uccp)bibf, NULL, &fmem);
    }
  else
    {
      free(fmem);
      fmem = NULL;
    }
  return bfs;
}

void
bx_bibs_res(const char *proj, const char **bibs)
{
  const char *resfile = NULL;
  if (project)
    {
      char buf[strlen(oracc())+strlen(project)+strlen("/pub/bib-res.ltx0")];
      (void)sprintf(buf, "%s/pub/%s/bib-addresources.ltx", oracc(), project);
      resfile = (ccp)pool_copy((uccp)buf, p);
    }
  else
    {
      char buf[strlen(oracc())+strlen("/lib/data/bib-res.ltx0")];
      (void)sprintf(buf, "%s/lib/data/bib-res.ltx", oracc());
      resfile = (ccp)pool_copy((uccp)buf, p);
    }

  FILE *resfp = fopen(resfile, "w");
  if (!resfp)
    {
      fprintf(stderr, "can't open resfile %s\n", resfile);
      exit(1);
    }
  
  int i;
  for (i = 0; bibs[i]; ++i)
    {
      if (verbose)
	fprintf(stderr, "using .bib file %s\n", bibs[i]);
      fprintf(resfp, "\\addbibresource{%s}\n", bibs[i]);
    }

  fclose(resfp);
}
