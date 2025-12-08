#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"
#include "bib.h"

static void bx_cite(Bx *bp, struct bib_cite_tab *tp, const char *k);
static void bx_cit_run_one(Bx *bp, const char *fname);
Mloc no_known_loc = { .file = NULL, .line = -1 };

/* Pre for CIT mode means:
 *
 *   - load the bib-key.txt file if any
 *   - make an array of the inputs
 */
void
bx_cit_pre(Bx *bp)
{
  char *fmem;
  if (bp->mode != BX_CIT && bp->mode != BX_ICF)
    bxl_bibkeys(bp); /* argv may have .bib files */
  else
    bxl_bibkeys(bp); /* argv has .xml files */
  if (bp->citations_file)
    {
      bp->citations = (const char **)loadfile_lines3((uccp)bp->citations_file, NULL, NULL);
    }
  else
    {
      if (bp->flist_cit)
	bp->files_cit = bxl_flist_files(bp, bp->flist_cit, "*.xml", &fmem);
      else if (bp->argv[optind])
	bp->files_cit = (const char **)&bp->argv[optind];
      bp->keys_cit = hash_create(1024);
      bp->keys_full = hash_create(1024);
    }
}

/* Run for CIT mode means iterate over the inputs extracting the cite
 * keys; we check the cite keys are known if there is a bib-key.txt.
 */
void
bx_cit_run(Bx *bp)
{
  if (bp->citations)
    {
      int i;
      for (i = 0; bp->citations[i]; ++i)
	bx_cite(bp, bib_cite("cite", 4), bp->citations[i]);
    }
  else if (bp->files_cit)
    {
      int i;
      for (i = 0; bp->files_cit[i]; ++i)
	bx_cit_run_one(bp, bp->files_cit[i]);
    }
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
bx_cite(Bx *bp, struct bib_cite_tab *tp, const char *k)
{
  if (!hash_find(bp->keys_cit, (uccp)k))
    {
      Mloc *mp = hash_find(bp->keys, (uccp)k);
      if (!mp)
	{
	  const char *keyf = hash_find(bp->hbibkey, (uccp)k);
	  if (keyf)
	    {
	      uccp kk;
	      hash_add(bp->keys_cit, kk = pool_copy((uccp)k, bp->p), (void*)keyf);
	      if (C_FULL == tp->ctype)
		hash_add(bp->keys_full, kk, "");
	    }
	  else
	    {
	      if (!bp->quiet)
		fprintf(stderr, "%s:%ld: unknown cite key %s\n", runexpat_file(), runexpat_lnum(), k);
	      hash_add(bp->keys_cit, pool_copy((uccp)k, bp->p), &no_known_loc);
	    }
	}
      else
	hash_add(bp->keys_cit, pool_copy((uccp)k, bp->p), (void*)mp->file);
    }
}

static void
cit_sH(void *userData, const char *name, const char **atts)
{
  struct bib_cite_tab *tp = bib_cite(name+2, strlen(name+2));
  if (tp)
    {
      const char *k = findAttr(atts, "key");
      bx_cite(userData, tp, k);
    }
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
