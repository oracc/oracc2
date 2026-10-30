#include <oraccsys.h>
#include "gdl.tab.h"
#include "gdllib.h"
#include "gdllex.h"

extern void gdllex_destroy(void);

static char *
gdlbuffer(const char *s)
{
  int len = strlen(s);
  char *s2 = malloc(len+4);
  strcpy(s2, s);
  if ('\n' != s2[len-1])
    s2[len++] = '\n';
  s2[len] = s2[len+1] = '\0';
  gdl_setup_buffer(s2, len);
  return s2;
}

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
	  char *s2 = gdlbuffer((ccp)gp->sign);
	  /* do this before gdl_set_tree so that initial w-node gets the right line number */
	  if (m)
	    gdl_lex_init(m->file, m->line);
	  else
	    gdl_lex_init("<string>", 1);
	  gdl_set_tree(tp);
	  gdlparse();
	  gdllex_destroy();
	  free(s2);
	  gp->deep = tp->root->kids;
	}
    }
}

Tree *
gdlparse_string(Mloc *m, char *s)
{
  char *s2 = gdlbuffer(s);
  
  gdl_group_attach = lgp = NULL;

  /* must come before gdl_set_tree() */
  if (gdl_word_mode)
    wd_list = list_create(LIST_SINGLE);

  Pool *p = pool_init();
  gdl_create_tree(NULL, NULL, p);
  
  if (tcurr->root)
    tcurr->root->text = (ccp)pool_copy((uccp)s, gdlpool);

  /* do this before gdl_set_tree so that initial w-node gets the right line number */
  if (m)
    gdl_lex_init(m->file, m->line);
  else
    gdl_lex_init("<string>", 1);

  gdl_set_tree(tcurr);

  gdlparse();
    
  gdllex_destroy();

  free(s2);
  pool_term(p);

  Node *w;
  nonw_found = 0;

  for (w = list_first(wd_list); w; w = list_next(wd_list))
    gdl_word_attr(w);

  if (nonw_found)
    wd_list = gdl_rem_nonw(wd_list);

  if (deep_parse)
    {
      List *saved_wl = wd_list;
      wd_list = list_create(LIST_SINGLE);
      tree_iterator(tcurr, m, gdlparse_deep, NULL);
      list_free(wd_list, NULL);
      wd_list = saved_wl;
    }
  
  return tcurr;
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
