#include <oraccsys.h>
#include <pool.h>
#include "cx.h"
#include "keydata.h"

extern Hash *cx_sortcodes(Cx *c, KD_key*kp, const char *ktype, const Fsort **vals);

static FILE *sifp;
static Pool *sip;
static Hash *curr_vh;

/* compare as strings */
static int cx_fccmp(void *a, void*b)
{
  Fsort *fs_a = *(Fsort**)a;
  Fsort *fs_b = *(Fsort**)b;
  return strcmp((ccp)ipool_str(sip,fs_a->cp->u.index), (ccp)ipool_str(sip,fs_b->cp->u.index));
}

/* compare by value hash */
static int cx_vhcmp(void *a, void*b)
{
  Fsort *fs_a = *(Fsort**)a;
  Fsort *fs_b = *(Fsort**)b;
  long c_a = (uintptr_t)hash_find(curr_vh, ipool_str(sip,fs_a->cp->u.index));
  long c_b = (uintptr_t)hash_find(curr_vh, ipool_str(sip,fs_b->cp->u.index));
#if 0
  fprintf(stderr, "s_a=%s/c_a=%ld; s_b=%s/c_b=%ld\n",
	  ipool_str(sip,fs_a->cp->u.index), c_a,
	  ipool_str(sip,fs_b->cp->u.index), c_b);
#endif
  long ret = c_a - c_b;
  if (ret)
    return (ret > 0 ? 1 : -1);
  else
    return 0;
}

/* Turn the roco for the catalogue data into a matrix of structures
 * storing index into a pool and sort code
 */
static void
cx_si_marshall(Cx *c)
{
  Hash *typevals = hash_create(128);
  
  int i;
  c->si_rows = calloc((1+c->r->nlines), sizeof(Fcell **));

  for (i = 0; i < c->r->nlines; ++i)
    c->si_rows[i] = calloc((1+c->r->maxcols), sizeof(Fcell));

  /* hash-index-pool of all the strings in the roco */
  sip = c->si_pool = ihpool_init();

  /* First we need to marshall all the field-values as a collection of
   * key-type values; multiple fields can map to the same key type, so
   * we have to assemble the values before setting sort codes and then
   * possibly sorting.
   *
   * We build an entire new matrix so we can emit both the full XMD
   * data with sort codes and the sortable subset that goes in
   * sortinfo.tab.
   */
  for (i = 0; c->r->rows[0][i]; ++i)
    {
      /* Only index fields that are sortable */
      const char *ktype = hash_find(c->k->keytypes,c->r->rows[0][i]);
      if (ktype && hash_find(c->k->sortable,(uccp)ktype))
	{
	  KD_key *kp = hash_find(c->k->hkeys, (uccp)ktype);
	  
	  /* Each key type has its own hash of field sort data; the
	   * sort data is a pointer to the field cell data and a list
	   * of cells that have the sort data in common
	   */
	  Hash *fh = hash_find(typevals, (uccp)ktype);
	  if (!fh)
	    {
	      fh = hash_create(1024);
	      hash_add(typevals, (uccp)ktype, fh);
	    }
	  int j;
	  for (j = 1; c->r->rows[j]; ++j)
	    {
	      /* We are iterating over each value. For key types that
	       * have a <val> list check to see if the value is known.
	       * Unknown values in closed sets generate a diagnostic;
	       * those in open sets get added silently to the set.
	       */
	      if (kp->hvals && !hash_find(kp->hvals, c->r->rows[j][i]))
		{
		  if (kp->closed)
		    {
		      fprintf(stderr, "cx: %s: field %s has unknown value %s\n",
			      c->r->rows[j][0],
			      c->r->rows[0][i],
			      c->r->rows[j][i]);
		      /* is it right to ignore these? */
		    }
		  else
		    {
		      list_add(kp->lvals, c->r->rows[j][i]);
		      hash_add(kp->hvals, (uccp)c->r->rows[j][i], "");
		    }
		}
	      
	      Fsort *fsp = hash_find(fh, c->r->rows[j][i]);
	      Fcell *fcp = &c->si_rows[j][i];
	      if (fsp)
		{
		  *fcp = *(Fcell*)list_first(fsp->cells);
		  list_add(fsp->cells, fcp);
		}
	      else
		{
		  size_t ix = ipool_copy(c->r->rows[j][i], c->si_pool);
		  fcp->type = FCELL_SORT;
		  fcp->u.index = ix;
		  fsp = memo_new(c->msort);
		  fsp->cp = fcp;
		  fsp->cells = list_create(LIST_SINGLE);
		  list_add(fsp->cells, fcp);
		  hash_add(fh, c->r->rows[j][i], fsp);
		}
	    }
	}
      else
	{
	  int j;
	  for (j = 1; c->r->rows[j]; ++j)
	    {
	      Fcell *fcp = &c->si_rows[j][i];
	      fcp->type = FCELL_STRP;
	      fcp->u.str = (ccp)c->r->rows[j][i];
	    }
	}
    }

  const char **typekeys = hash_keys(typevals);
  for (int i = 0; typekeys[i]; ++i)
    {
      KD_key *kp = hash_find(c->k->hkeys, (uccp)typekeys[i]);
      int nvals;
      const Fsort **vals = (const Fsort **)hash_vals2(hash_find(typevals, (uccp)typekeys[i]),
						      &nvals);
      /* Now set up the sort protocol for the key-type */
      if ((curr_vh = cx_sortcodes(c, kp, typekeys[i], vals)))
	qsort(vals, nvals, sizeof(Fsort *), (sort_cmp_func*)cx_vhcmp);
      else
	qsort(vals, nvals, sizeof(Fsort *), (sort_cmp_func*)cx_fccmp);
      int k;
      for (k = 0; k < nvals; ++k)
	{
	  Fcell *fcp;
	  for (fcp = list_first(vals[k]->cells); fcp; fcp = list_next(vals[k]->cells))
	    fcp->sort = k;
	}
    }
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

static void
cx_si_sortdata(Cx *c)
{
  fprintf(sifp, "#nmembers %ld\n", c->r->nlines);
  int i;
  for (i = 1; c->si_rows[i]; ++i)
    {
      fprintf(sifp, "%s\t", c->r->rows[i][0]);
      int j;
      for (j = 0; j < c->r->maxcols; ++j)
	{
	  if (c->si_rows[i][j].type == FCELL_SORT)
	    {
	      fprintf(sifp, "%ld=%ld", c->si_rows[i][j].sort, c->si_rows[i][j].u.index);
	      if (c->r->maxcols - j > 1)
		fputc('\t', sifp);
	    }
	}
      fputc('\n', sifp);
    }
}

static void
cx_si_pool(Cx *c)
{
  fprintf(sifp, "#nstring %ld\n", c->si_pool->nstr);
  ipool_write(c->si_pool, sifp);
}

void
cx_sortinfo(Cx *c)
{
  if (sortinfo_only)
    sifp = stdout;
  else
    sifp = xfopen("sortinfo.tab", "w");
  cx_si_marshall(c);
  cx_si_fields(c);
  cx_si_sortdata(c);
  cx_si_pool(c);
}
