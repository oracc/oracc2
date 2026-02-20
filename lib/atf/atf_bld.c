#include <oraccsys.h>
#include "atf.h"

void
atf_bld_amp(Mloc l, const char *pqx, unsigned const char *name)
{
  Atfl *src = memo_new(atfmp->matfl);
  src->l = l;
  src->u.atf = atfmp->atf;
  list_add(atfmp->atf->atflines, src);
  atfmp->atf->file = curratffile;
  atfmp->atf->src = src;
  atfmp->atf->pqx = pqx;
  atfmp->atf->name = name;
}

void
atf_bld_implicit_block(void)
{
  curr_block = memo_new(atfmp->mblks);
  /* for now curr_block->src == NULL means implicit */
  curr_group = memo_new(atfmp->mgroups);
  curr_block->lines = curr_group;
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

void
atf_bld_protocol(Mloc l, Prot pt, const char *s)
{
  Protocol *p = memo_new(atfmp->mprotocols);
  p->t = pt;
  switch (pt)
    {
    case PROT_BIB:
    case PROT_NOTE:
    case PROT_VERSION:
      p->u.str = (uccp)s;
      break;
    case PROT_LZR_SPARSE:
      {
	char *dup = strdup(s);
	char **ff = vec_from_str(dup, NULL, NULL);
	atfp->lzr_sparse = p->u.sparse = hash_create(0);
	int i;
	for (i = 0; ff[i]; ++i)
	  {
	    hash_add(p->u.sparse, pool_copy((uccp)ff[i], atfmp->pool), "");
	    free(ff[i]);
	  }
	free(ff);
	free(dup);
      }
      break;
    case PROT_LZR_STOP:
      p->u.stop = atoi(s);
      break;
    case PROT_TOP:
      break;
    }
  list_add(atfmp->lprotocols, p);
}

