#include <oraccsys.h>
#include "cbd.h"

/* Routines to manage data divided by lang rather than glossary */

void
cbd_sig_add_one(const unsigned char *s, int rank, MWE_type mtype)
{
  uccp t = s;

  if ('{' == *t)
    t = (uccp)strstr((ccp)s, "}::");

  uccp lng = (uccp)strchr((ccp)t, '%');
  ++lng;
  uccp col = (uccp)strchr((ccp)lng, ':');
  char *xlng;
  if (col)
    {
      int len = col - lng;
      char buf[len+1];
      strncpy(buf, (ccp)lng, len);
      buf[len] = '\0';
      xlng = strdup(buf);
    }
  else
    xlng = strdup((ccp)curr_cbd->lang);
  
  List *lp = hash_find(csetp->hhm[mtype], (uccp)xlng);
  if (!lp)
    hash_add(csetp->hhm[mtype],
	     pool_copy((uccp)xlng, csetp->pool),
	     (lp = list_create(LIST_SINGLE)));

  free(xlng);
  
  Lemsig *lsp = memo_new(csetp->lsigmem);
  lsp->key = lsp->sig = s;
  if (t != s)
    {
      lsp->key = (uccp)memo_dup((ccp)lsp->sig);
      char *k = strstr((ccp)lsp->key, "}::");
      k[1] = '\0';
    }
  if (rank)
    lsp->rank = rank;
  list_add(lp, lsp);
}

void
cbd_sig_add_list(List *lp, MWE_type mtype)
{
  const unsigned char *s;
  for (s = list_first(lp); s; s = list_next(lp))
    cbd_sig_add_one(s, 0, mtype);
}
