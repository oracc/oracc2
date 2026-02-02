#include <oraccsys.h>
#include "cbd.h"

/* Routines to manage data divided by lang rather than glossary */

List *
cbd_sig_list(const char *l)
{
  return lp;
}

void
cbd_sig_add_one(const unsigned char *s)
{
  uccp lng = (uccp)strchr((ccp)s, '%');
  ++lng;
  uccp col = (uccp)strchr((ccp)lng, ':');
  int len = col - lng;
  char buf[len+1];
  strncpy(buf, (ccp)lng, len);
  buf[len] = '\0';
  List *lp = hash_find(csetp->hsiglangs, (uccp)buf);
  if (!lp)
    hash_add(csetp->hsiglangs, pool_copy((uccp)buf, csetp->pool), (lp = list_create(LIST_SINGLE)));

  Lemsig *lsp = memo_new(csetp->lsigmem);
  lsp->sig = s;
  lsp->rank = 0; /* Need to ensure that rank travels with sig at this phase of processing */
  list_add(lp, lsp);
}

void
cbd_sig_add_list(List *lp)
{
  const unsigned char *s;
  for (s = list_first(lp); s; s = list_next(lp))
    cbd_sig_add_one(s);
}
