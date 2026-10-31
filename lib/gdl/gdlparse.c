#include <oraccsys.h>
#include <lng.h>
#include "gdl.tab.h"
#include "gdllib.h"
#include "gdllex.h"

extern void gdllex_destroy(void);

Stck *GDL_stack = NULL;
Tree *tcurr;

/* Require a pool to be set here and eventually freed by caller */
void
gdl_create_tree(Treemem *tmp, Pool *p, const char *lang)
{
  assert(p != NULL);
  tcurr = (tmp ? tree_init_mem(tmp) : tree_init());
  tcurr->user = calloc(1, sizeof(GDL));
  (void)tree_root(tcurr, NS_GDL, "g:gdl", 1, NULL);
  gdlpool = p;
  if (lang)
    word_lang_tag = lang;
  else
    word_lang_tag = "sux";
  /* lang_switch uses memo_auto so for the global_lang we clone it into persistent mem */
  struct lang_context *x = lang_switch(NULL,word_lang_tag,NULL,NULL,0);
  if (!global_lang)
    {
      global_lang = calloc(1, sizeof(struct lang_context));
      *global_lang = *x;
    }
  gdl_lang_context = global_lang;
}

void
gdl_push_tree(Tree *tp)
{
  stck_push(GDL_stack, (intptr_t)tp);
}

Tree *
gdl_pop_tree(void)
{
  intptr_t ptr = stck_pop(GDL_stack);
  if (-1 == ptr)
    return NULL;
  else
    return (Tree*)ptr;
}

/* This destroys tcurr which should be the last tree on GDL_stack */
void
gdl_destroy_tree(void)
{
  Tree *tp = (Tree*)stck_peek(GDL_stack);
  if (tp && tp == tcurr)
    {
      stck_pop(GDL_stack);
      if (wd_list)
	list_free(wd_list, NULL);
      if (c_dangling_gps)
	list_free(c_dangling_gps, NULL);
      if (c_explicit_gps)
	list_free(c_explicit_gps, NULL);
      if (c_implicit_gps)
	list_free(c_implicit_gps, NULL);
      if (modq)
	list_free(modq, NULL);
      if (break_stack)
	stck_term(break_stack);
      if (lang_stack)
	stck_term(lang_stack);
      if (state_stack)
	stck_term(state_stack);
      if (sstck)
	stck_term(sstck);
      if (mgstck)
	memo_term(mgstck);
      if (mlstck)
	memo_term(mlstck);
    }
  else
    abort();
}

void
gdl_stack_init(void)
{
  if (!GDL_stack)
    GDL_stack = stck_init(3);
}
void
gdl_stack_term(void)
{
  if (GDL_stack)
    {
      stck_term(GDL_stack);
      GDL_stack = NULL;
    }
}

static char *
gdlbuffer(const char *s)
{
  int len = strlen(s);
  char *s2 = malloc(len+4);
  strcpy(s2, s);
#if 0  
  if ('\n' != s2[len-1])
    s2[len++] = '\n';
#endif
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
	  /*gdl_set_tree(tp);*/
	  gdlparse();
	  gdllex_destroy();
	  free(s2);
	  gp->deep = tp->root->kids;
	}
    }
}

/* The parser's working tree, tcurr, must be set before calling this
   routine */
void
gdlparse_string(Mloc *m, char *s)
{
  assert(tcurr != NULL);
  tcurr->root->text = (ccp)pool_copy((uccp)s, gdlpool);
  char *s2 = gdlbuffer(s);
  
  gdl_group_attach = lgp = NULL;

  wd_list = list_create(LIST_SINGLE);

  if (m)
    gdl_lex_init(m->file, m->line);
  else
    gdl_lex_init("<string>", 1);

  gdlparse();
    
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
      List *saved_wl = wd_list;
      wd_list = list_create(LIST_SINGLE);
      tree_iterator(tcurr, m, gdlparse_deep, NULL);
      list_free(wd_list, NULL);
      wd_list = saved_wl;
    }
}

/* These may be redundant now; they are intended for init/term every
   time the parser is called; see gdllib_init/term for init/term
   before/after all calls to the parser */
void
gdlparse_init(void)
{
}

void
gdlparse_reset(void)
{
}

void
gdlparse_term(void)
{
}
