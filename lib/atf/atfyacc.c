#include <oraccsys.h>
#include <memo.h>
#include <pool.h>
#include "atf.h"

Atfm *atfmp;
ATF *atfp;

ATF *
atfyacc(void)
{
  curratffile = atffile;
  atf_init();
  atfparse();
  return atfp;
}

void
atf_init(void)
{
  atfmp = calloc(1, sizeof(Atfm));
  atfmp->lpstart = list_create(LIST_SINGLE);
  atfmp->llinks = list_create(LIST_SINGLE);
  atfmp->lkeys = list_create(LIST_SINGLE);
  atfmp->lpstart = list_create(LIST_SINGLE);
  atfmp->mblks = memo_init(sizeof(Blk),256);
  atfmp->mxlinks = memo_init(sizeof(Xlink),16);
  atfmp->mkeys = memo_init(sizeof(Key),16);
  atfmp->matfl = memo_init(sizeof(Key),256);
  atfmp->pool = pool_init();
  atfp = atfmp->atf = calloc(1, sizeof(ATF));
  atfp->man = atfmp;
  atfp->atflines = list_create(LIST_SINGLE);
}

void
atf_term(void)
{
}
