#include <oraccsys.h>
#include "atf.h"

void
atf_bld_amp(Mloc l, const char *pqx, unsigned const char *name)
{
  Atfl *src = memo_new(atfmp->matfl);
  src->l = l;
  src->u.atf = atfmp->atf;
  list_add(atfmp->atf->atflines, src);
  atfmp->atf->src = src;
  atfmp->atf->pqx = pqx;
  const char *n = normalize_ws(name);
  if (strlen(n) < strlen((ccp)name))
    strcpy((char*)name, n);
  atfmp->atf->name = name;
}

void
atf_bld_link(Mloc l, Linkt lt, const unsigned char *siglum, const char *qid,
	     const unsigned char *name)
{
  Xlink *lp = memo_new(atfmp->mxlinks);
  lp->t = lt;
  lp->siglum = siglum;
  lp->qid = qid;
  lp->name = name;
  list_add(atfmp->llinks, lp);
}
