#include <oraccsys.h>
#include <memo.h>
#include <pool.h>
#include <o2.h>
#include <lang.h>
#include <scan.h>
#include "etcsl.h"
#include "note.h"
#include "atf.h"

Atfm *atfmp;
ATF *atfp;

extern void atflex_destroy(void);

Tree *
atfyacc(void)
{
  /*curratffile = atffile;*/

  Tree *tp = tree_init();

  (void)tree_root(tp, NS_XTF, "xtf", 1, NULL);

  longtext_set_sep(" ");
  
  atf_set_tree(tp);

#if 0
  if (m)
    atf_lex_init(m->file, m->line);
  else
    atf_lex_init("<string>", 1);
#endif

  atfparse();

  atflex_destroy();

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
  atfmp->mgroups = memo_init(sizeof(Group),256);
  atfmp->mkeys = memo_init(sizeof(Key),16);
  atfmp->milem_forms = memo_init(sizeof(struct ilem_form),1024);
  atfmp->mlines = memo_init(sizeof(Line),256);
  atfmp->mnotes = memo_init(sizeof(Note),32);
  atfmp->mprotocols = memo_init(sizeof(Protocol),256);
  atfmp->mxis = memo_init(sizeof(struct xcl_ilem),1024);
  atfmp->mxlinks = memo_init(sizeof(Xlink),16);
  atfmp->pool = pool_init();
  scan_set_pool(atfmp->pool);
  atfp = atfmp->atf = calloc(1, sizeof(ATF));
  atfp->utype = N_U_ATF;
  atfp->man = atfmp;
  atfp->input = list_create(LIST_SINGLE);
  /* lang_init() and lang_term() must now be called by ax or equivalent before/after the entire run */
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
  if (etcsl_labels)
    {
      pool_term(etcsl_pool);
      hash_free(etcsl_labels, NULL);
      etcsl_labels = NULL;
      etcsl_pool = NULL;
    }
  list_free(atfmp->lprotocols, NULL);
  list_free(atfmp->llinks, NULL);
  list_free(atfmp->lkeys, NULL);
  memo_term(atfmp->matfls);
  memo_term(atfmp->mbibs);
  memo_term(atfmp->mblocks);
  memo_term(atfmp->mgroups);
  memo_term(atfmp->mkeys);
  memo_term(atfmp->milem_forms);
  memo_term(atfmp->mlines);
  memo_term(atfmp->mnotes);
  memo_term(atfmp->mprotocols);
  memo_term(atfmp->mxis);
  memo_term(atfmp->mxlinks);
  pool_term(atfmp->pool);
  lmemo_term();
  free(atfmp);
  atfmp = NULL;
  curr_line = NULL;
  text_lang = NULL;

  free(atfp->protocols);
  list_free(atfp->input, NULL);
  hash_free(atfp->hlabmap, NULL);
  /*list_free(atfp->protocols, NULL);*/
  free(atfp);
  atfp = NULL;

  atf_lex_term();
  label_term();
  note_term();
  tree_term();  
}

void
atf_tlat_wrapup(void)
{
  atr_para();
  atr_finish_labels();
}

void
atf_tlit_wrapup(void)
{
  atf_group_wrapup();
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
