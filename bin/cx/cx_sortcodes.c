#include <oraccsys.h>
#include "cx.h"
#include "keydata.h"

Hash *
cx_sc_from_file(Cx *c, const char *ktype)
{
  char *ok = NULL;
  Hash *h = NULL;
  char f[strlen(ktype)+strlen("-sort-codes.lst0")];
  sprintf(f, "%s-sort-codes.lst", ktype);
  if (!(ok = findfile_dir(c->project, f, NULL, "lib")))
    {
      char f2[strlen(f)+1];
      sprintf(f2, "%s-sort-order.lst", ktype);
      if (!(ok = findfile_dir(c->project, f2, NULL, "lib")))
	{
	  char *od = oracc_data(f);
	  if (!access(od, R_OK))
	    ok = strdup(od);
	  else
	    {
	      od = oracc_data(f2);
	      if (!access(od, R_OK))
		ok = strdup(od);
	    }
	}
    }
  
  if (ok)
    {
      h = hash_create(1024);
      if (strstr(ok, "-order"))
	{
	  unsigned char *m;
	  size_t n;
	  char **lp = (char**)loadfile_lines3((uccp)ok, &n, &m);
	  int i;
	  for (i = 0; lp[i]; ++i)
	    hash_add(h, (uccp)lp[i], (void*)(uintptr_t)i);
	}
      else
	{
	  Roco *r = roco_load1(ok);
	  int i;
	  for (i = 0; i < r->nlines; ++i)
	    hash_add(h, (uccp)r->rows[i][0], (void*)(uintptr_t)strtoul((ccp)r->rows[i][1], NULL, 10));
	}
    }

  if (!h && !strcmp(ktype, "names"))
    return cx_sc_from_file(c, "designation");
  
  return h;
}

Hash *
cx_sortcodes(Cx *c, KD_key *kp, const char *ktype, const Fsort **vals)
{
  Hash *vh = NULL;
  /* if a key type has a file [KEYTYPE]-sort-{codes|order}.lst load
     set the key values list from that */
  if (!(vh = cx_sc_from_file(c, ktype)))
    {
      if (!kp->reorder)
	{
	  /* if a non-reordered key type has a <val> list assign sortcodes based on the list */
	  if (kp->lvals)
	    {
	      vh = hash_create(1024);
	      int i;
	      const char *s;
	      for (i = 0, s = list_first(kp->lvals); s; s = list_next(kp->lvals), ++i)
		hash_add(vh, (uccp)s, (void*)(uintptr_t)i);
	    }
	}
    }
  return vh;
}
