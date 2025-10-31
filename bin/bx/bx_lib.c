#include <oraccsys.h>
#include <roco.h>
#include "bx.h"

/* Future iterations of this routine will allow a comma-separated list
 * of files in flist and also check for directories in the flist or
 * its contents which will then be globbed using the ext arg
 */
const char **
bxl_flist_files(Bx *bp, const char *flist, const char *ext, char **fmem)
{
  const char **ret = NULL;
  if (!strcmp(flist, "=") || !access(flist, R_OK))
    ret = (const char **)loadfile_lines3((uccp)flist, NULL, (unsigned char **)fmem);
  else
    fprintf(stderr, "bx: failed to open file list %s\n", flist);
  return ret;
}

void
bxl_bibkeys(Bx *bp)
{
  bxl_bibkey_file(bp);
  if (bp->bibkey_file)
    bp->hbibkey = roco_hash(roco_load1(bp->bibkey_file));
  if (verbose)
    {
      if (bp->hbibkey)
	fprintf(stderr, "bx: using bib keys file %s\n", bp->bibkey_file);
      else
	fprintf(stderr, "bx: proceeding with no bib keys file\n");
    }
}

void
bxl_bibkey_file(Bx *bp)
{
  if (bp->project)
    {
      char buf[strlen(oracc())+strlen(bp->project)+strlen("/pub/bib-key.txt0")];
      (void)sprintf(buf, "%s/pub/%s/bib-key.txt", oracc(), bp->project);
      if (!access(buf, R_OK))
	bp->bibkey_file = (ccp)pool_copy((uccp)buf, bp->p);
    }

  if (!bp->bibkey_file)
    {
      char buf[strlen(oracc())+strlen("/lib/data/bib-key.txt0")];
      (void)sprintf(buf, "%s/lib/data/bib-key.txt", oracc());
      if (!access(buf, R_OK))
	bp->bibkey_file = (ccp)pool_copy((uccp)buf, bp->p);
    } 
}

void
bxl_key_writer(Bx *bp, FILE *fp)
{
  int i;
  const char **kk = hash_keys2(m_cit(bp) ? bp->keys_cit : bp->keys, &i);
  qsort(kk, i, sizeof(const char *), cmpstringp);
  if (m_cit(bp))
    for (i = 0; kk[i]; ++i)
      fprintf(fp, "%s\n", kk[i]);
  else
    for (i = 0; kk[i]; ++i)
      fprintf(fp, "%s\t%s\n", kk[i], (char*)hash_find(bp->keys, (uccp)kk[i]));
}
