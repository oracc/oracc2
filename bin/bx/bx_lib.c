#include <oraccsys.h>
#include <roco.h>
#include "bx.h"

static const char *xbib[2] = { 0,0 };

void
bxl_bib_files(Bx *bp, int use_argv)
{
  char *fmem = NULL;
  if (bp->flist_bib)
    {
      const char *dotbib = strstr(bp->flist_bib, ".bib");
      if (dotbib && !strcmp(dotbib, ".bib"))
	{
	  xbib[0] = bp->flist_bib;
	  bp->files_bib = xbib;
	}
      else
	bp->files_bib = bxl_flist_files(bp, bp->flist_bib, "*.bib", &fmem);
    }
  else if (use_argv && bp->argv[optind])
    bp->files_bib = (const char **)&bp->argv[optind];
  list_add(bp->mem, fmem);
}

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
      char buf[strlen(oracc())+strlen("/lib/bib/keys.tab0")];
      (void)sprintf(buf, "%s/lib/bib/keys.tab", oracc());
      if (BX_KEY == bp->mode || !access(buf, R_OK))
	bp->bibkey_file = (ccp)pool_copy((uccp)buf, bp->p);
    } 
}

const char **
bxl_key_array(Hash *h)
{
  int i;
  const char **kk = hash_keys2(h, &i);
  qsort(kk, i, sizeof(const char *), cmpstringp);
  return kk;
}

void
bxl_key_writer(Bx *bp, FILE *fp)
{
  int i;
  Hash *h = m_cit(bp) ? bp->keys_cit : bp->keys;
  const char **kk = bxl_key_array(h);
  for (i = 0; kk[i]; ++i)
    {
      Mloc *mp = hash_find(h, (uccp)kk[i]);
      if (mp)
	fprintf(fp, "%s\t%s\n", kk[i], mp->file ? mp->file : "#unknown_key");
      else
	fprintf(stderr, "bxl_key_writer: internal error: key %s has NULL Mloc*\n", kk[i]);
    }
}
