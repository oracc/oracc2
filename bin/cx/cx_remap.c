#include <oraccsys.h>
#include "cx.h"
#include "keydata.h"

/* This routine only operates on a single input.
 *
 * Output the catalogue data augmenting it with remaps given in
 * keydata.xml key-types:
 *
 * if keys/@remap = 1
 *    Add a column keys/@remap-to
 *
 * For each value in a key-type with keys/@remap, output the field
 * and then the remapped field; emit a diagnostic for any value not
 * given with val/@remap.
 *
 */
int
cx_remap(Cx *c)
{
  Hash *remaps[c->k->nfields+1];
  KD_key *kt[c->k->nfields+1];
  int i, status = 0;
  unsigned char **fields = c->rr[0]->rows[0];
  FILE *rfp = stdout;

  memset(remaps, '\0', c->k->nfields+1 * sizeof(Hash*));
  
  for (i = 0; fields[i]; ++i)
    {
      if (i)
	fputc('\t', rfp);
      fputs((ccp)fields[i], rfp);
      
      const char *t = hash_find(c->k->keytypes, fields[i]);
      if (!t)
	{
	  fprintf(stderr, "cx: no key type for field %s in keydata.xml\n", fields[i]);
	  ++status;
	}
      else
	{
	  KD_key *kp = hash_find(c->k->hkeys, (uccp)t);
	  if (!kp)
	    {
	      fprintf(stderr, "cx: no <keys> entry for %s in keydata.xml\n", t);
	      ++status;
	      kt[i] = NULL;
	    }
	  else
	    {
	      kt[i] = kp;
	      if (kp->remap)
		{
		  fputc('\t', rfp);
		  fputs(kp->remapto, rfp);
		  remaps[i] = kp->rvals;
		}
	      else
		remaps[i] = NULL;
	    }
	}
    }
  fputc('\n', rfp);

  for (i = 1; i < c->rr[0]->nlines; ++i)
    {
      unsigned char **vals = c->rr[0]->rows[i];
      int j;
      for (j = 0; vals[j]; ++j)
	{
	  if (j)
	    fputc('\t', rfp);
	  fputs((ccp)vals[j], rfp);
	  if (remaps[j])
	    {
	      const char *r = hash_find(remaps[j], vals[j]);
	      if (r)
		{
		  fputc('\t', rfp);
		  fputs(r, rfp);
		}
	      else
		{
		  if (kt[j]->closed)
		    {
		      fprintf(stderr, "cx: no remap value for %s\n", vals[j]);
		      ++status;
		    }
		  fputs("\tunknown", rfp);
		}
	    }
	}
      fputc('\n', rfp);
    }
  return status;
}
