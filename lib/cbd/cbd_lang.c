#include <oraccsys.h>
#include "cbd.h"

/* Routines to manage data divided by lang rather than glossary */

List *
cbd_sig_list(const char *l)
{
  List *lp = hash_find(csetp->hsiglangs, (uccp)l);
  if (!lp)
    {
      lp = list_create(LIST_SINGLE);
      hash_add(csetp->hsiglangs, (uccp)strdup((ccp)l), lp);
    }
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
  list_add(cbd_sig_list(buf), (void*)s);
}

void
cbd_sig_add_list(List *lp)
{
  const unsigned char *s;
  for (s = list_first(lp); s; s = list_next(lp))
    cbd_sig_add_one(s);
}
