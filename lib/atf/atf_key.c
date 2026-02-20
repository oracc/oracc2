#include <oraccsys.h>
#include "atf.h"

static Key *
key_parse(unsigned char *lp)
{
  Key *kp = memo_new(atfmp->mkeys);
  kp->key = (char*)pool_copy(lp, atfmp->pool);
  lp = (unsigned char *)kp->key;
  while (*lp && (*lp > 128 || (!isspace(*lp) && '=' != *lp)))
    ++lp;
  if (*lp)
    {
      int equals_mode = '=' == *lp;
      *lp++ = '\0';
      while (*lp && (*lp > 128 || isspace(*lp)))
	++lp;
      kp->val = (char*)lp;
      if (!equals_mode)
	{
	  while (*lp && (*lp >128 || !isspace(*lp)))
	    ++lp;
	  if (*lp)
	    *lp++ = '\0';
	  while (*lp && (*lp > 128 || isspace(*lp)))
	    ++lp;
	  if (*lp)
	    kp->url = (char*)lp;
	  while (*lp && (*lp >128 || !isspace(*lp)))
	    ++lp;
	  if (*lp)
	    *lp = '\0';
	  if (!kp->url && kp->val && (!strncmp(kp->val,"http",4) || !strncmp(kp->val,"https",5)))
	    {
	      kp->url = kp->val;
	      kp->val = "";
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
#if 0
  if (!strcmp(kp->key, "after") || !strcmp(kp->key, "see"))
    type = (unsigned char*)kp->key;
#endif
}
