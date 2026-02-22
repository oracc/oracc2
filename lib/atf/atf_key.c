#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"

static Key *
key_parse(unsigned char *lp)
{
  Key *kp = memo_new(atfmp->mkeys);
  kp->key = (char*)pool_copy(lp, atfmp->pool);
  lp = (unsigned char *)kp->key;
  while (*lp && (*lp > 128 || (!isspace(*lp) && '=' != *lp)))
    ++lp;
  char *key_end = (char *)lp;
  while (isspace(*lp))
    ++lp;
  if (*lp)
    {
      int equals_mode = '=' == *lp;
      *key_end = '\0';
      if (equals_mode)
	{
	  ++lp;
	  while (isspace(*lp))
	    ++lp;
	}
      kp->val = (char*)lp;
      if (!equals_mode)
	{
	  /* This implements
	     "#key: see Krebernik 1999 https://whatever"
	     or
	     "key: see https://whatever"
	     or
	     "key: see Krebernik 1999"
	   */
	  if (!strncmp((ccp)lp,"http",4) || !strncmp((ccp)lp,"https",5))
	    {
	      kp->url = kp->val;
	      kp->val = "";
	    }
	  else
	    {
	      unsigned char *last_word = lp + strlen((ccp)lp);
	      while (last_word > lp && !isspace(last_word[-1]))
		--last_word;
	  
	      if (last_word > lp
		  && (!strncmp((ccp)last_word,"http",4) || !strncmp((ccp)last_word,"https",5)))
		{
		  kp->url = (ccp)last_word;
		  while (isspace(last_word[-1]))
		    --last_word;
		  *last_word = '\0';
		  kp->val = (ccp)lp; /* text between 'see/after' and URL */
		}
	      }
	}
    }
  return kp;
}

void
atf_bld_key(Mloc m, char *str)
{
  Key *kp = key_parse((ucp)str);
  if (atfp->edoc == EDOC_SCORE)
    {
      if (!strcmp(kp->key, "siglum-map"))
	{
	  char *arrow = strstr(kp->val, "=>");
	  if (arrow)
	    {
	      char *from = malloc(strlen(kp->val)+1);
	      strcpy(from,kp->val);
	      from[arrow-kp->val] = '\0';
	      /*symbolattr_map(atfp->pqx, from, arrow+2);*//* TO DO: port from otf/lib/symbolattr.c */
	      free(from);
	    }
	  else
	    {
	      warning("bad syntax in siglum-map, say, e.g., A1=>A");
	    }
	}
    }
  list_add(atfmp->lkeys, kp);
  abt_add_key_protocol(&m, kp);
}
