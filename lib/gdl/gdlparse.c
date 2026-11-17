#include <oraccsys.h>
#include <tree.h>
#include <lng.h>
#include "gdl.tab.h"
#include "gdllib.h"
#include "gdllex.h"

/* GDL parser tree management
 * ==========================
 *
 * GDL tree processing works on a global tree, tcurr.
 *
 * (1) One tree life-cycle is used by applications that use
 * gdlparse_string directly.  These need to do:
 *
 * gdllib_init() -- start of run
 *
 * gdl_GDL() -- before options parsing
 *
 * [[ set the GDL structure options as needed ]]
 *
 * gdlparse_init(Treemem *tm, GDL*) -- sets up tcurr with a new Tree but with common Treemem 
 *
 * Tree *tp = gdlparse_string(m, s) -- parses string and returns the Tree
 *
 * [[ do whatever is needed with the parse results ]]
 *
 * [[ gdlparse_reset(tp) -- gdlparse_string() -- repeat parse ]]
 *
 * gdlparse_term(tp) -- release all memory used by parser and GDL
 *
 *
 *
 * (2) The other tree life-cycle is used by applications that embed
 * GDL parsing in another parser--at present this is only done by ax.
 *
 * In this case the sequence is:
 *
 * gdllib_init() -- start of run
 *
 * gdl_GDL() -- start of ATF text, i.e., &-line; save the GDL* in the ATF Tree's user mem
 *
 * [[ set GDL opts from ATF protocols ]]
 *
 * gdlparse_start(Tree *tp,GDL*) -- sets tcurr to ATF_TREE with ->user = GDL*
 *
 * gdlparse_line(ATF_LINE_NODE)
 *
 * gdl_GDL_reset(gp) -- free all GDL resources at end of ATF text
 *
 * gdlparse_stop() -- sets tcurr=NULL; note that gdlparse_term() must not be used in (2)
 *
 * ATF inline text can include GDL parsing--this is done by using tree
 * life-cycle (1) within the current life-cycle (2) framework.
 *
 */

Tree *tcurr;

extern void gdllex_destroy(void);
static void gdlparse_deep(Node *np, void *mptr);

Tree*
gdl_literal(Mloc *m, char *s)
{
  Tree *tp = tree_clone(tcurr);
  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
  tp->root->text = s;
  return tp;
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

Tree *
gdlparse_string(Mloc *m, char *s)
{
  tcurr->root->text = (ccp)pool_copy((uccp)s, gdlpool);
  if (m)
    gdl_lex_init(m->file, m->line);
  else
    gdl_lex_init("<string>", 1);

  gdl_legacy_lexer(gdl_legacy);
  if (gdl_auto_id)
    gdl_auto_ids();
  
  char *s2 = gdlbuffer(s);

  gdl_group_attach = lgp = NULL;

  wd_list = list_create(LIST_SINGLE);
  gdl_new_word(tcurr);

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
    tree_iterator(tcurr, m, gdlparse_deep, NULL);

  return tcurr;
}

static void
gdlparse_deep(Node *np, void *mptr)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (gp->orig && '|' != *gp->orig && gp->sign && '|' == *gp->sign)
	{
	  Tree *xcurr = tcurr;
	  tcurr = tree_init_mem(xcurr->tm);
	  tcurr->user = gdl_GDL();
	  *((GDL*)tcurr->user) = *((GDL*)xcurr->user);

	  /* This is where we reset GDL members as necessary */
	  
	  (void)tree_root(tcurr, NS_GDL, "g:gdl", 1, NULL);
	  char *s2 = gdlbuffer((ccp)gp->sign);

	  /* do this before gdl_set_tree so that initial w-node gets the right line number */
	  Mloc *m = mptr;
	  if (m)
	    gdl_lex_init(m->file, m->line);
	  else
	    gdl_lex_init("<string>", 1);

	  wd_list = list_create(LIST_SINGLE);
	  gdlparse();
	  gdllex_destroy();
	  free(s2);
	  gp->deep = tcurr->root->kids;
	  gdl_GDL_free(tcurr->user);
	  tree_term(tcurr);
	  tcurr = xcurr;
	}
    }
}

void
gdlparse_init(Treemem *tm, GDL *gp)
{
  assert(gp != NULL);
  tcurr = tree_init_mem(tm);
  tcurr->user = gp;
  (void)tree_root(tcurr, NS_GDL, "g:gdl", 1, NULL);
  /* lang_switch uses memo_auto so for the global_lang we clone it into persistent mem */
  struct lang_context *x = lang_switch(NULL,word_lang_tag,NULL,NULL,0);
  if (!global_lang)
    {
      global_lang = calloc(1, sizeof(struct lang_context));
      *global_lang = *x;
    }
  gdl_lang_context = global_lang;
  gdl_balance_init();
  gdl_modq_init();
}

void
gdlparse_reset(void)
{
  Treemem *tm = tcurr->tm;
  GDL *tgp = tcurr->user;
  tree_term(tcurr);
  tcurr = tree_init_mem(tm);
  tcurr->user = gdl_GDL_reset(tgp);
  (void)tree_root(tcurr, NS_GDL, "g:gdl", 1, NULL);
  gdl_balance_init();
  gdl_modq_init();
}

void
gdlparse_term(void)
{
  gdl_balance_term();
  gdl_GDL_free(tcurr->user);
  tree_term(tcurr);
  /* Most gdlparse_term is taken care of in gdl_GDL_free */
}

void
gdlparse_start(Tree *tp, GDL *gp)
{
  tcurr = tp;
  tcurr->user = gp;
  gdl_lang_context = global_lang;
  gdl_balance_init();
  gdl_modq_init();
}

void
gdlparse_stop(void)
{
  gdl_GDL_free(tcurr->user);
  tcurr = NULL;
}
