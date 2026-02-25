#include <oraccsys.h>
#include <memo.h>
#include <pool.h>
#include "atf.h"

Atfm *atfmp;
ATF *atfp;

Tree *
atfyacc(void)
{
  curratffile = atffile;

  Tree *tp = tree_init();

  (void)tree_root(tp, NS_XTF, "xtf", 1, NULL);

  atf_set_tree(tp);
#if 0
  if (m)
    atf_lex_init(m->file, m->line);
  else
    atf_lex_init("<string>", 1);
#endif

  atfparse();

  atf_wrapup_buffer();

  return tp;
}

void
atf_init(void)
{
  atfmp = calloc(1, sizeof(Atfm));
  atfmp->lprotocols = list_create(LIST_SINGLE);
  atfmp->llinks = list_create(LIST_SINGLE);
  atfmp->lkeys = list_create(LIST_SINGLE);
  atfmp->matfls = memo_init(sizeof(Atfl),256);
  atfmp->mbibs = memo_init(sizeof(Bib),16);
  atfmp->mblocks = memo_init(sizeof(Block),256);
  atfmp->mkeys = memo_init(sizeof(Key),16);
  atfmp->mprotocols = memo_init(sizeof(Protocol),256);
  atfmp->mxlinks = memo_init(sizeof(Xlink),16);
  atfmp->pool = pool_init();
  atfp = atfmp->atf = calloc(1, sizeof(ATF));
  atfp->man = atfmp;
  atfp->input = list_create(LIST_SINGLE);
}

void
atf_input(Mloc l, Atflt t, void *p)
{
  Atfl a = { l , t , p };
  Atfl*ap = memo_new(atfmp->matfls);
  *ap = a;
  list_add(atfp->input, ap);
}

void
atf_term(void)
{
}

void
atf_wrapup(Wheret where)
{
  if (where == WH_PREAMBLE)
    {
      if (list_len(atfmp->lkeys))
	{
	  atfp->keys = (Key**)list2array_c(atfmp->lkeys, &atfp->nkeys);
	  list_free(atfmp->lkeys, NULL);
	  atfmp->lkeys = NULL;
	}
      if (list_len(atfmp->llinks))
	{
	  atfp->links = (Xlink**)list2array_c(atfmp->llinks, &atfp->nlinks);
	  list_free(atfmp->llinks, NULL);
	  atfmp->llinks = NULL;
	}
      if (list_len(atfmp->lprotocols))
	{
	  atfp->protocols = (Protocol**)list2array_c(atfmp->lprotocols, &atfp->nprotocols);
	  list_free(atfmp->lprotocols, NULL);
	  atfmp->lprotocols = NULL;
	}
      atf_clear_protocols();
      in_preamble = 0;
    }
  else if (where == WH_GROUP)
    {
      if (list_len(atfmp->llines))
	{
	  curr_group->lines = (Line**)list2array_c(atfmp->llines, &curr_group->nlines);
	  list_free(atfmp->llinks, NULL);
	  atfmp->llinks = NULL;
	}
    }
}
