#include <oraccsys.h>
#include <roco.h>
#include <cbd.h>
#include "kis.h"

/* Scan a Kis and index the period keys where the hash key is the cgp
 * or cgpse and the values are a list period keys. Period keys are
 * checked against a hash of seen period keys to uniq them.
 *
 * Then the lists are reduced to arrays and sorted so the periods hash
 * is a set of precooked Kis_data arrays for each cgp/cgpse.
 */   
static Hash *
kis_periods(Kis *k)
{
  Hash *seen = hash_create(1024);
  Hash *periods = hash_create(1024);
  size_t i;
  for (i = 0; k->rows[i]; ++i)
    {
      const char *ap = strstr((ccp)k->rows[i][1], "\x01p");
      if (ap)
	{
	  ++ap; /* include the ^A in the key */
	  /*fprintf(stderr, "kis_periods: %s; ", k->rows[i][1]);*/
	  if (!hash_find(seen, k->rows[i][1]))
	    {
	      hash_add(seen, k->rows[i][1], "");
	      int len = ap - (ccp)k->rows[i][1];
	      char buf[len+1];
	      strncpy(buf, (ccp)k->rows[i][1], len);
	      buf[len] = '\0';
	      /*fprintf(stderr, "key = ::%s::\n", buf);*/
	      List *pl = hash_find(periods, (uccp)buf);
	      if (!pl)
		{
		  pl = list_create(LIST_SINGLE);
		  ap = (ccp)pool_copy((uccp)buf, csetp->pool);
		  hash_add(periods, (uccp)ap, pl);
		}
	      list_add(pl, (void*)k->rows[i]);
	    }
	}
    }
  const char **hk = hash_keys(periods);
  for (i = 0; hk[i]; ++i)
    {
      List *lp = hash_find(periods, (uccp)hk[i]);
      int n;
      Kis_data *kp = (Kis_data*)list2array_c(lp, &n);
      list_free(lp, NULL);
      /* sort kp here */
      hash_add(periods, (uccp)hk[i], kp);
    }
  hash_free(seen, NULL);
  return periods;
}

Kis *
kis_load(const char *kis_fn)
{
  Kis *kis = roco_load1(kis_fn);
  kis->hash_key_col = 1;
  kis->hdata = roco_hash_r(kis);
  kis->user = kis_periods(kis);
  return kis;
}
