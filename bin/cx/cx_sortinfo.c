#include <oraccsys.h>
#include <pool.h>
#include "cx.h"
#include "keydata.h"

static FILE *sifp;
static Pool *sip;

static int cx_fccmp(Fsort *a, Fsort *b)
{
  return cmpstringp(ipool_str(sip,a->cp->index), ipool_str(sip,b->cp->index));
}

static void
cx_si_fields(Cx *c)
{
  fprintf(sifp, "#nfields %d\n", c->k->nfields);
  fprintf(sifp, "#nmapentries %d\n", c->k->nmapentries);
  int i;
  for (i = 0; c->k->fields[i]; ++i)
    {
      KD_map *mp = hash_find(c->k->sortable, (uccp)c->k->fields[i]);
      fprintf(sifp, "#field ");
      const char *f;
      for (f = list_first(mp->fields); f; f = list_next(mp->fields))
	fprintf(sifp, "%s ", f);
      fprintf(sifp, "= %s\n", mp->hr);
    }
}

/* Turn the roco for the catalogue data into a matrix of structures
 * storing index into a pool and sort code
 */
void
cx_si_pool(Cx *c)
{
  int i, fc_i = 0;
  c->si_rows = calloc((1+c->r->nlines), sizeof(Fcell **));

  for (i = 0; i < c->r->nlines; ++i)
    c->si_rows[i] = calloc((1+c->r->maxcols), sizeof(Fcell));

  /* hash-index-pool of all the strings in the roco */
  sip = c->si_pool = ihpool_init();

  for (i = 0; c->r->rows[0][i]; ++i)
    {
      /* Only index fields that are sortable */
      if (hash_find(c->k->sortable, c->r->rows[0][i]))
	{
	  /* This is the index into the field cell data matrix */
	  ++fc_i;

	  /* Each field has its own hash of field sort data; the sort
	   * data is a pointer to the field cell data and a list of
	   * cells that have the sort data in common
	   */
	  Hash *fh = hash_create(1024);
	  int j;
	  for (j = 1; c->r->rows[j]; ++j)
	    {
	      Fsort *fsp = hash_find(fh, c->r->rows[j][i]);
	      Fcell *fcp = &c->si_rows[j][fc_i];
	      if (fsp)
		{
		  *fcp = *(Fcell*)list_first(fsp->cells);
		  list_add(fsp->cells, fcp);
		}
	      else
		{
		  size_t ix = ipool_copy(c->r->rows[j][i], c->si_pool);
		  fcp->index = ix;
		  fsp = memo_new(c->msort);
		  fsp->cells = list_create(LIST_SINGLE);
		  list_add(fsp->cells, fcp);
		  hash_add(fh, c->r->rows[i][j], fsp);
		}
	    }

	  /* Now we have all the data for a field, get an array of the
	   * values which are field sort data; sort the array; use
	   * the list in the sort data to push the sort codes back
	   * into the cell data
	   */
	  int nvals;
	  const Fsort **vals = (const Fsort **)hash_vals2(fh, &nvals);
	  qsort(vals, nvals, sizeof(void*), (sort_cmp_func*)cx_fccmp);
	  int k;
	  for (k = 0; k < nvals; ++k)
	    {
	      Fcell *fcp;
	      for (fcp = list_first(vals[i]->cells); fcp; fcp = list_next(vals[i]->cells))
		fcp->sort = k;
	    }
	}
    }
}


void
cx_sortinfo(Cx *c)
{
  sifp = stdout;
  cx_si_fields(c);
  cx_si_pool(c);
}
