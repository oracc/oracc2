#include <oraccsys.h>
#include "gdllib.h"

extern void gdllex_destroy(void);
void
gdlparse_deep(Node *np, void *mptr)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (gp->orig && '|' != *gp->orig && gp->sign && '|' == *gp->sign)
	{
	  Tree *tp = tree_init();
	  Mloc *m = mptr;
	  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
	  gdl_setup_buffer((char*)pool_copy((ucp)gp->sign, gdlpool));
	  /* do this before gdl_set_tree so that initial w-node gets the right line number */
	  if (m)
	    gdl_lex_init(m->file, m->line);
	  else
	    gdl_lex_init("<string>", 1);
	  gdl_set_tree(tp);
	  /*gdlparse();*/
	  gdllex_destroy();
	  gp->deep = tp->root->kids;
	}
    }
}

Tree *
gdlparse_string(Mloc *m, char *s)
{
  Tree *tp = tree_init();
  char *s2 = malloc(strlen(s)+2);

  strcpy(s2, s);
  strcat(s2, "\n");

  gdl_group_attach = lgp = NULL;

  /* must come before gdl_set_tree() */
  if (gdl_word_mode)
    wd_list = list_create(LIST_SINGLE);
  
  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);

  if (tp->root)
    tp->root->text = (ccp)pool_copy((uccp)s, gdlpool);

  gdl_setup_buffer(s2);
  
  /* do this before gdl_set_tree so that initial w-node gets the right line number */
  if (m)
    gdl_lex_init(m->file, m->line);
  else
    gdl_lex_init("<string>", 1);

  gdl_set_tree(tp);

  /*gdlparse();*/
  gdllex_destroy();
  free(s2);

  Node *w;
  nonw_found = 0;

  for (w = list_first(wd_list); w; w = list_next(wd_list))
    gdl_word_attr(w);

  if (nonw_found)
    wd_list = gdl_rem_nonw(wd_list);

  if (deep_parse)
    {
      if (gdl_word_mode)
	{
	  List *saved_wl = wd_list;
	  wd_list = list_create(LIST_SINGLE);
	  tree_iterator(tp, m, gdlparse_deep, NULL);
	  list_free(wd_list, NULL);
	  wd_list = saved_wl;
	}
      else
	tree_iterator(tp, m, gdlparse_deep, NULL);
    }
  
  return tp;
}

void
gdlparse_init(void)
{
  gdl_init();
}

void
gdlparse_reset(void)
{
}

void
gdlparse_term(void)
{
  gdl_term();
}
