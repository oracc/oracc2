#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static void bx_ref_bib_from_cit(Bx *bp);
static void bx_ref_run_one(const char *bibfile, Hash *cites);

/* REF mode:
 *
 * with -R arg validate all .bib files; optionally sort, augment, and
 *    output them as XML/HTML
 *
 * with -r arg get the set of cite keys to process by running CIT mode
 *    and then process references to create a sorted, disambiguated
 *    bibliography which can be printed and also used for ICF mode
 */

/* Pre for REF mode means:
 *
 * - determine the .bib inputs
 *
 *   a) use CIT mode to get citations and set bp->keys_cit to a hash
 *      with the .bib files then invert bp->keys_cit to create array
 *      of .bib inputs
 *
 *   b) make an array of the .bib inputs according to the args
 *
 * For full function:
 *
 * - load the bm-names.xml db
 * - load the bm-jrnser.xml db
 *
 */

void
bx_ref_pre(Bx *bp)
{
  if (bp->bibonly)
    {
      bxl_bib_files(bp);
      bp->keys_cit = NULL; /* transparency for bib_load arg */
    }
  else
    {
      /* this will load bp->keys_cit with the cite keys from the .xml inputs */
      bx_cit_pre(bp);
      bx_cit_run(bp);
      bx_ref_bib_from_cit(bp);
    }
  bp->entries = list_create(LIST_SINGLE);
  bp->bibs = list_create(LIST_SINGLE);
  bnm_init(bp);
  bib_init(bp);
}

void
bx_ref_run(Bx *bp)
{
  int i;
  bibset_debug(0);
  /* load the entries from the .bib files */
  for (i = 0; bp->files_bib[i]; ++i)
    {
      Bib *bibp = memo_new(bp->m_bib);
      bibp->file = bp->files_bib[i];
      list_add(bp->bibs, bibp);
      curr_bibp = bibp;
      /* per-file entry lists are done by resetting top-level entries
       * pointer; during load always add to top-level bp->entries
       */
      if (bp->bibonly && !bp->sort)
	bp->entries = bibp->entries = list_create(LIST_SINGLE);
      bx_ref_run_one(bibp->file, bp->keys_cit);
    }

  /* validate the entries */

  /* sort the entries */

  if (bp->sort)
    bib_sort(bp);
  
  /* augment the entries */
  
}

void
bx_ref_out(Bx *bp)
{
  if (bp->xmloutput)
    {
      FILE *xfp = NULL;
      if (!strcmp(bp->outfile, "-"))
	xfp = stdout;
      else
	xfp = fopen(bp->outfile, "w");
      bib_xml(bp->bibs, xfp);
    }
}

static void
bx_ref_bib_from_cit(Bx *bp)
{
  /* use the values of the keys_cit to make a hash of .bib files
     needed and then coerce that hash into the Bx files_bib
     array */
  const char **bibf = (const char **)hash_vals(bp->keys_cit);
  Hash *hbib = hash_create(1024);
  int i;
  for (i = 0; bibf[i]; ++i)
    {
      if (strcmp(bibf[i], "#unknown_key"))
	if (!hash_find(hbib, (uccp)bibf[i]))
	  hash_add(hbib, (uccp)bibf[i], "");
    }
  bp->files_bib = hash_keys(hbib);
  free(hbib);
}

static void
bx_ref_run_one(const char *bibfile, Hash *cites)
{
  extern const char *curr_bib;
  extern FILE *bibin;
  curr_bib = bibfile;
  FILE *fp = fopen(bibfile, "r");
  if (!fp)
    {
      fprintf(stderr, "can't read .bib %s\n", bibfile);
      exit(1);
    }
  bibin = fp;
  field = NULL;
  bib_cites = cites;
  bib_field = bib_nesting = 0;
  biblineno = 1;
  bibparse();
  bibreset();
 fclose(fp);
}
