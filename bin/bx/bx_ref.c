#include <oraccsys.h>
#include "bx.h"

static void bx_ref_bib_from_cit(Bx *bp);

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
 * - load the bib-names.xml db
 * - load the bib-jrnser.xml db
 *
 */

void
bx_ref_pre(Bx *bp)
{
  if (bp->bibonly)
    {
      bxl_bib_files(bp);
    }
  else
    {
      /* this will load bp->keys_cit with the cite keys from the .xml inputs */
      bx_cit_pre(bp);
      bx_cit_run(bp);
      bx_ref_bib_from_cit(bp);
    }
}

void
bx_ref_run(Bx *bp)
{
  /* load the necessary .bib files */

  /* validate the entries */

  /* sort the entries */
  
  /* augment the entries */
  
}

void
bx_ref_out(Bx *bp)
{
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
