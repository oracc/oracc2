#include <oraccsys.h>
#include "bx.h"

/* module to create system and project key files used by the inline
 * bib handler to validate keys
 */

static FILE *keyfp;
const char *bib_file;
static int bib_lnum;

static void bx_keys_sub(Bx *bp, const char *f);
static void bx_keys_ids(Bx *bp, FILE *bibfp);
static int bx_keys_key(Bx *bp, FILE *bibfp);

void
bx_keys(Bx *bp, const char *project, const char **bibfiles)
{
  int i;
  if (bp->outfile)
    {
      if (!strcmp(bp->outfile, "-"))
	keyfp = stdout;
    }
  else if (project)
    {
      char buf[strlen(oracc())+strlen(project)+strlen("/pub/bib-key.txt0")];
      (void)sprintf(buf, "%s/pub/%s/bib-key.txt", oracc(), project);
      bp->outfile = (ccp)pool_copy((uccp)buf, bp->p);
    }
  else
    {
      char buf[strlen(oracc())+strlen("/lib/keys.tab0")];
      (void)sprintf(buf, "%s/lib/bib/keys.tab", oracc());
      bp->outfile = (ccp)pool_copy((uccp)buf, bp->p);
    }

  if (!keyfp)
    {
      if (!(keyfp = fopen(bp->outfile, "w")))
	{
	  fprintf(stderr, "can't write key output %s", bp->outfile);
	  exit(1);
	}
    }
  
  for (i = 0; bibfiles[i]; ++i)
    bx_keys_sub(bp, bibfiles[i]);

  const char **kk = hash_keys2(bp->keys, &i);
  qsort(kk, i, sizeof(const char *), cmpstringp);
  for (i = 0; kk[i]; ++i)
    fprintf(keyfp, "%s\n", kk[i]);

  fclose(keyfp);
}

