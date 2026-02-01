#include <oraccsys.h>
#include <roco.h>
#include "gx.h"

void
lemm_sigs(const char *fn)
{
  char buf[strlen(fn)], *l;
  strcpy(buf, fn);
  l = strchr(fn, '-');
  if (l)
    {
      char *dot = strchr(l, '.');
      if (dot)
	{
	  *dot = '\0';
	  strcpy(buf, l);
	  Roco *r = roco_load1(fn);
	  if (r)
	    {
	      r->skip_initial_lines = 1;
	      Hash *h = roco_hash(r);
	      if (h)
		{
		  if (!csetp->lems)
		    csetp->lems = hash_create(10);
		  hash_add(csetp->lems, pool_copy(buf,csetp->pool), h);
		}
	      else
		fprintf(stderr, "lemm_sig: failed to make hash from loaded lemm file %s\n", fn);
	    }
	  else
	    fprintf(stderr, "lemm_sig: failed to load lemm file %s\n", fn);
	}
      else
	fprintf(stderr, "lemm_sig: no '.' after '-' in filename %s\n", fn);
    }
  else
    fprintf(stderr, "lemm_sig: no '-' in filename %s\n", fn);
}
