#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"

static void bx_cit_run_one(Bx *bp, const char *fname);

/* Pre for CIT mode means:
 *
 *   - load the bib-key.txt file if any
 *   - make an array of the inputs
 */
void
bx_cit_pre(Bx *bp)
{
  char *fmem;
  bxl_bibkeys(bp);
  if (bp->flist_cit)
    bp->files_cit = bxl_flist_files(bp, bp->flist_cit, "*.xml", &fmem);
  else if (bp->argv[optind])
    bp->files_cit = (const char **)&bp->argv[optind];
  bp->keys_cit = hash_create(1024);
}

/* Run for CIT mode means iterate over the inputs extracting the cite
 * keys; we check the cite keys are known if there is a bib-key.txt.
 */
void
bx_cit_run(Bx *bp)
{
  int i;
  for (i = 0; bp->files_cit[i]; ++i)
    bx_cit_run_one(bp, bp->files_cit[i]);
}

/* Output for CIT mode means list the found keys if output is requested */
void
bx_cit_out(Bx *bp)
{
  if (bp->outfile)
    {
      FILE *outfp = !strcmp(bp->outfile,"-") ? stdout : fopen(bp->outfile, "w");
      if (outfp)
	{
	  bxl_key_writer(bp, outfp);
	  if (strcmp(bp->outfile,"-"))
	    fclose(outfp);
	}
      else
	fprintf(stderr, "bx: unable to write cite key output %s\n", bp->outfile);
    }
}

static void
bx_cite(Bx *bp, const char **atts)
{
  const char *k = findAttr(atts, "key");
  if (!hash_find(bp->keys_cit, (uccp)k))
    {
      hash_add(bp->keys_cit, pool_copy((uccp)k, bp->p), "");
      if (!hash_find(bp->keys, (uccp)k) && !bp->quiet)
	fprintf(stderr, "%s:%ld: unknown cite key %s\n", runexpat_file(), runexpat_lnum(), k);
    }
}

static void
cit_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "b:cite"))
    bx_cite(userData, atts);
}

static void
cit_eH(void *userData, const char *name)
{
}

static void
bx_cit_run_one(Bx *bp, const char *fname)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, cit_sH, cit_eH, NULL, bp);
}
